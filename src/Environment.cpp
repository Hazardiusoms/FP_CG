#include "Environment.hpp"
#include "Math.hpp"
#include "SceneState.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace sw {
namespace {
// Deterministic pseudo-random values in [0,1), so the layout is identical after every reset.
float hash01(std::uint32_t& state) {
    state = state*1664525u + 1013904223u;
    return static_cast<float>(state>>8)/16777216.0f;
}

// Smooth ridged height field: a few sine octaves with sharp ridges and a falloff toward the edges.
float heightAt(float u,float v,const float phase[6]) {
    float h=0;
    h+=0.55f*std::sin(u*1.7f+phase[0])*std::cos(v*1.3f+phase[1]);
    h+=0.25f*std::sin(u*3.9f+phase[2])*std::cos(v*3.1f+phase[3]);
    h+=0.12f*std::sin(u*7.3f+phase[4])*std::cos(v*6.7f+phase[5]);
    h=1.0f-std::fabs(h)*0.7f;
    const float edge=std::max(0.0f,1.0f-std::max(std::fabs(u),std::fabs(v)));
    return std::max(0.0f,h)*edge*edge*1.8f;
}

// Triangulated grid over [-1,1]^2 with normals from neighbouring samples.
// Vertex: position (3), normal (3), crest factor (1) for shading from base to crest.
std::vector<float> terrainVertices(std::uint32_t& seed,float width,float height,std::vector<std::uint32_t>& indices) {
    constexpr int N=18;
    float phase[6];
    for(float& p:phase) p=hash01(seed)*6.2831853f;
    std::vector<float> heights((N+1)*(N+1));
    std::vector<Vec3> points((N+1)*(N+1));
    for(int j=0;j<=N;++j) for(int i=0;i<=N;++i) {
        const float u=-1.0f+2.0f*i/N, v=-1.0f+2.0f*j/N;
        const float h=heightAt(u,v,phase)*height;
        const int k=j*(N+1)+i;
        heights[k]=h;
        points[k]={u*width*0.5f,h,v*width*0.5f};
    }
    std::vector<float> data;
    for(int j=0;j<=N;++j) for(int i=0;i<=N;++i) {
        const int k=j*(N+1)+i;
        const int il=std::max(i-1,0), ir=std::min(i+1,N), jl=std::max(j-1,0), jr=std::min(j+1,N);
        const Vec3 dx=points[j*(N+1)+ir]-points[j*(N+1)+il];
        const Vec3 dz=points[jr*(N+1)+i]-points[jl*(N+1)+i];
        Vec3 n=normalized(cross(dz,dx));
        if(n.y<0) n=n*-1.0f;
        const float crest=std::clamp(heights[k]/(height+1e-4f),0.0f,1.0f);
        data.insert(data.end(),{points[k].x,points[k].y,points[k].z,n.x,n.y,n.z,crest});
    }
    for(int j=0;j<N;++j) for(int i=0;i<N;++i) {
        const std::uint32_t a=j*(N+1)+i, b=a+1, c=a+(N+1), d=c+1;
        indices.insert(indices.end(),{a,c,b, b,c,d});
    }
    return data;
}

// Axis-aligned box centred at (cx, cy, cz) with half sizes; flat-shaded faces (crest factor 0).
void appendBox(std::vector<float>& data,std::vector<std::uint32_t>& indices,Vec3 c,Vec3 h) {
    const Vec3 p[8]={
        {c.x-h.x,c.y-h.y,c.z-h.z},{c.x+h.x,c.y-h.y,c.z-h.z},{c.x+h.x,c.y+h.y,c.z-h.z},{c.x-h.x,c.y+h.y,c.z-h.z},
        {c.x-h.x,c.y-h.y,c.z+h.z},{c.x+h.x,c.y-h.y,c.z+h.z},{c.x+h.x,c.y+h.y,c.z+h.z},{c.x-h.x,c.y+h.y,c.z+h.z},
    };
    const int faces[6][4]={{0,3,2,1},{4,5,6,7},{0,4,7,3},{1,2,6,5},{3,7,6,2},{0,1,5,4}};
    const Vec3 normals[6]={{0,0,-1},{0,0,1},{-1,0,0},{1,0,0},{0,1,0},{0,-1,0}};
    for(int f=0;f<6;++f) {
        const std::uint32_t o=static_cast<std::uint32_t>(data.size()/7);
        for(int k=0;k<4;++k) {
            const Vec3 q=p[faces[f][k]], n=normals[f];
            data.insert(data.end(),{q.x,q.y,q.z,n.x,n.y,n.z,0.0f});
        }
        indices.insert(indices.end(),{o,o+1,o+2, o,o+2,o+3});
    }
}

// Upright cylinder approximated by a prism with `sides` faces (flat shading).
void appendCylinder(std::vector<float>& data,std::vector<std::uint32_t>& indices,Vec3 base,float radius,float height,int sides) {
    for(int i=0;i<sides;++i) {
        const float a0=6.2831853f*i/sides, a1=6.2831853f*(i+1)/sides;
        const Vec3 d0{std::cos(a0),0,std::sin(a0)}, d1{std::cos(a1),0,std::sin(a1)};
        const Vec3 p0=base+d0*radius, p1=base+d1*radius;
        const Vec3 t0=p0+Vec3{0,height,0}, t1=p1+Vec3{0,height,0};
        const Vec3 n=normalized(d0+d1);
        const std::uint32_t o=static_cast<std::uint32_t>(data.size()/7);
        for(Vec3 q:{p0,p1,t1,t0}) data.insert(data.end(),{q.x,q.y,q.z,n.x,n.y,n.z,0.0f});
        indices.insert(indices.end(),{o,o+1,o+2, o,o+2,o+3});
    }
}

ScenePart makePart(const std::vector<float>& data,const std::vector<std::uint32_t>& indices,Vec3 color,float flat) {
    ScenePart part;
    part.mesh=std::make_unique<Mesh>();
    part.mesh->initialize(data,7,indices,{{0,3,0},{1,3,3},{2,1,6}});
    part.color=color;
    part.flat=flat;
    return part;
}
}

void Environment::initialize(const std::filesystem::path& directory) {
    shutdown();
    backgroundShader_.initialize(directory/"background.vert",directory/"background.frag");
    mountainShader_.initialize(directory/"mountain.vert",directory/"mountain.frag");
    glGenVertexArrays(1,&backgroundVao_);

    // Each massif variant has its own width and peak height; the shape comes from the seed.
    const float widths[kVariantCount]={14.0f,13.0f,15.0f,12.5f,14.0f,13.5f};
    const float heights[kVariantCount]={5.5f,6.5f,4.8f,6.0f,5.6f,5.0f};
    for(std::size_t v=0;v<kVariantCount;++v) {
        std::uint32_t seed=static_cast<std::uint32_t>(v*7919u+13u);
        std::vector<std::uint32_t> indices;
        const auto data=terrainVertices(seed,widths[v],heights[v],indices);
        auto mesh=std::make_unique<Mesh>();
        mesh->initialize(data,7,indices,{{0,3,0},{1,3,3},{2,1,6}});
        massifs_.push_back(std::move(mesh));
    }

    // Palm: tapered trunk and a crown of six fronds.
    {
        std::vector<float> trunk;
        std::vector<std::uint32_t> trunkIndices;
        const float r0=0.10f, r1=0.06f, h=3.2f;
        constexpr int S=6;
        for(int i=0;i<=S;++i) {
            const float a=6.2831853f*i/S, c=std::cos(a), s=std::sin(a);
            for(float y : {0.0f,h}) {
                const float r=y==0.0f ? r0 : r1;
                const Vec3 p{c*r,y,s*r};
                const Vec3 n{c,0,s};
                trunk.insert(trunk.end(),{p.x,p.y,p.z,n.x,n.y,n.z,0.0f});
            }
        }
        for(int i=0;i<S;++i) {
            const std::uint32_t a=2*i, b=2*i+1, c=2*i+2, d=2*i+3;
            trunkIndices.insert(trunkIndices.end(),{a,c,b, b,c,d});
        }
        palmTrunk_=std::make_unique<Mesh>();
        palmTrunk_->initialize(trunk,7,trunkIndices,{{0,3,0},{1,3,3},{2,1,6}});
    }
    {
        std::vector<float> fronds;
        std::vector<std::uint32_t> frondIndices;
        constexpr int L=6;
        for(int i=0;i<L;++i) {
            const float a=6.2831853f*i/L;
            const Vec3 tip{std::cos(a)*1.5f,-0.35f,std::sin(a)*1.5f};
            const Vec3 side{-std::sin(a)*0.22f,0.0f,std::cos(a)*0.22f};
            const Vec3 base{0,0,0};
            const Vec3 mid=tip*0.55f+Vec3{0,0.35f,0};
            const Vec3 pts[]{base+side,base-side,mid+side*0.5f,mid-side*0.5f,tip};
            const Vec3 n=normalized(cross(side,tip-base));
            const std::uint32_t o=static_cast<std::uint32_t>(fronds.size()/7);
            for(const Vec3 p:pts) fronds.insert(fronds.end(),{p.x,p.y,p.z,n.x,n.y,n.z,0.0f});
            frondIndices.insert(frondIndices.end(),{o,o+1,o+2, o+1,o+3,o+2, o+2,o+3,o+4});
        }
        palmFronds_=std::make_unique<Mesh>();
        palmFronds_->initialize(fronds,7,frondIndices,{{0,3,0},{1,3,3},{2,1,6}});
    }

    // Roadside props, each made of a few boxes and prisms, centred at the base.
    {
        // Shop: a low box with a lit sign on top and a door recess.
        std::vector<float> d; std::vector<std::uint32_t> ix;
        appendBox(d,ix,{0,1.0f,0},{1.6f,1.0f,1.2f});
        appendBox(d,ix,{0,2.2f,0},{1.8f,0.2f,1.4f});
        props_[0].push_back(makePart(d,ix,{0.32f,0.22f,0.38f},0.0f));
        d.clear(); ix.clear();
        appendBox(d,ix,{0,2.45f,0.9f},{1.2f,0.25f,0.05f});
        props_[0].push_back(makePart(d,ix,{1.0f,0.35f,0.75f},1.0f));
    }
    {
        // Service station: a canopy on two posts, a pump, and a sign.
        std::vector<float> d; std::vector<std::uint32_t> ix;
        appendBox(d,ix,{-1.6f,2.8f,0},{0.08f,2.8f,0.08f});
        appendBox(d,ix,{1.6f,2.8f,0},{0.08f,2.8f,0.08f});
        appendBox(d,ix,{0,5.6f,0},{2.2f,0.15f,1.6f});
        props_[1].push_back(makePart(d,ix,{0.30f,0.30f,0.36f},0.0f));
        d.clear(); ix.clear();
        appendBox(d,ix,{0,0.6f,1.2f},{0.4f,0.6f,0.3f});
        props_[1].push_back(makePart(d,ix,{0.20f,0.55f,0.62f},0.0f));
        d.clear(); ix.clear();
        appendBox(d,ix,{0,5.4f,0},{1.6f,0.12f,0.9f});
        props_[1].push_back(makePart(d,ix,{0.95f,0.40f,0.85f},1.0f));
    }
    {
        // Fountain: a round basin with a short column and a water disc.
        std::vector<float> d; std::vector<std::uint32_t> ix;
        appendCylinder(d,ix,{0,0,0},1.2f,0.4f,12);
        props_[2].push_back(makePart(d,ix,{0.38f,0.30f,0.46f},0.0f));
        d.clear(); ix.clear();
        appendCylinder(d,ix,{0,0.35f,0},1.0f,0.08f,12);
        props_[2].push_back(makePart(d,ix,{0.25f,0.60f,0.80f},1.0f));
        d.clear(); ix.clear();
        appendCylinder(d,ix,{0,0.4f,0},0.15f,1.3f,8);
        props_[2].push_back(makePart(d,ix,{0.50f,0.40f,0.55f},0.0f));
    }
    // Skyline far behind the massifs, centred on the sun. Built in world coordinates, so it is drawn
    // with an identity model matrix. Towers are flat silhouettes; window strips are emissive.
    {
        struct Tower { float x, z, halfW, height; };
        const Tower towers[]={
            {-34.0f,-172.0f,3.6f,20.0f},{-25.0f,-180.0f,2.8f,13.0f},{-18.0f,-168.0f,2.4f,27.0f},
            {-10.0f,-182.0f,3.2f,17.0f},{-2.0f,-174.0f,2.0f,44.0f},{5.0f,-180.0f,3.0f,23.0f},
            {12.0f,-170.0f,2.6f,33.0f},{20.0f,-178.0f,3.4f,15.0f},{28.0f,-172.0f,2.4f,25.0f},
            {35.0f,-182.0f,3.0f,14.0f}
        };
        const Vec3 windowColors[]{{1.0f,0.35f,0.75f},{0.35f,0.95f,1.0f}};
        std::uint32_t seed=77u;
        std::size_t index=0;
        for(const Tower& t:towers) {
            std::vector<float> body;
            std::vector<std::uint32_t> bodyIndices;
            appendBox(body,bodyIndices,{t.x,t.height*0.5f,t.z},{t.halfW,t.height*0.5f,t.halfW});
            city_.push_back(makePart(body,bodyIndices,{0.16f,0.08f,0.24f},1.0f));
            std::vector<float> windows;
            std::vector<std::uint32_t> windowIndices;
            for(float y=2.0f;y<t.height-1.0f;y+=2.0f) {
                for(float side : {-0.5f,0.5f}) {
                    seed=seed*1664525u+1013904223u;
                    if(hash01(seed)<0.4f) continue;
                    appendBox(windows,windowIndices,{t.x+side*t.halfW,y,t.z+t.halfW+0.03f},{t.halfW*0.2f,0.45f,0.03f});
                }
            }
            if(!windows.empty()) city_.push_back(makePart(windows,windowIndices,windowColors[index%2],1.0f));
            ++index;
        }
    }
    buildSections();
}

float Environment::depthScale(float z) const {
    // Mild depth variation only: the perspective projection already makes distant ranges smaller.
    const float distance=std::fabs(z);
    return std::clamp(1.2f-distance/(sectionLength*kRowsPerSide)*0.25f,0.95f,1.2f);
}

void Environment::assignSection(Section& section,std::uint32_t seed) const {
    section.variant=static_cast<std::size_t>(seed%kVariantCount);
    section.seed=seed;
    section.scale=depthScale(section.z);
    // About a third of the sections are plains with no massif; the rest have one.
    section.plain=(seed>>24)%3!=0;
    // Roughly half carry a palm; a few carry a roadside prop instead of a palm.
    section.palm=(seed>>16)%2==0;
    section.palmScale=1.1f+0.4f*static_cast<float>((seed>>8)&0xFF)/255.0f;
    const std::uint32_t roll=(seed>>4)%10;
    section.prop=roll==0 ? 1 : roll==1 ? 2 : roll==2 ? 3 : 0;
}

void Environment::buildSections() {
    sections_.clear();
    // Left and right rows, beyond the road edge with a visible gap, spaced along -Z.
    std::uint32_t seed=2026u;
    for(int row=0;row<kRowsPerSide;++row) {
        const float z=-sectionLength*row;
        Section left{z,-roadHalfWidth-16.0f,1.0f,0,0,false,1.0f,false,0};
        Section right{z-sectionLength*0.5f,roadHalfWidth+16.0f,1.0f,0,0,false,1.0f,false,0};
        seed=seed*1664525u+1013904223u;
        assignSection(left,seed);
        seed=seed*1664525u+1013904223u;
        assignSection(right,seed);
        sections_.push_back(left);
        sections_.push_back(right);
    }
}

void Environment::update(float deltaTime,float speed) {
    // Sky time advances with real time only, so stars and shooting stars do not depend on road speed.
    skyTime_+=deltaTime;
    // Scenery moves toward +Z, matching the road grid. A section that passes behind the camera
    // is moved back by one period and gets a new seed, so the recycled content looks different.
    const float period=sectionLength*kRowsPerSide;
    for(auto& s:sections_) {
        s.z+=speed*deltaTime;
        if(s.z>cameraRecycleZ) {
            s.z-=period;
            assignSection(s,s.seed*1664525u+1013904223u);
        }
        s.scale=depthScale(s.z);
    }
}

void Environment::reset() {
    skyTime_=0;
    buildSections();
}

void Environment::renderBackground(const RenderContext& ctx) const {
    if(!backgroundVao_) return;
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    backgroundShader_.use();
    backgroundShader_.set("uCameraForward",ctx.cameraForward);
    backgroundShader_.set("uCameraRight",ctx.cameraRight);
    backgroundShader_.set("uCameraUp",ctx.cameraUp);
    backgroundShader_.set("uTanHalfFov",std::tan(ctx.verticalFov*0.5f));
    backgroundShader_.set("uAspect",static_cast<float>(ctx.framebufferWidth)/static_cast<float>(ctx.framebufferHeight));
    backgroundShader_.set("uSunDirection",normalized({0.0f,0.03f,-1.0f}));
    backgroundShader_.set("uSunRadius",0.22f);
    backgroundShader_.set("uStripeCount",6.0f);
    backgroundShader_.set("uTime",skyTime_);
    glBindVertexArray(backgroundVao_);
    glDrawArrays(GL_TRIANGLES,0,3);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

void Environment::renderMountains(const RenderContext& ctx) const {
    mountainShader_.use();
    mountainShader_.set("uView",ctx.view);
    mountainShader_.set("uProjection",ctx.projection);
    mountainShader_.set("uLightDirection",normalized({0.0f,0.12f,-1.0f}));
    mountainShader_.set("uFogColor",Vec3{0.45f,0.08f,0.35f});
    mountainShader_.set("uRimColor",Vec3{1.0f,0.35f,0.75f});
    mountainShader_.set("uSnowColor",Vec3{0.85f,0.70f,0.90f});
    mountainShader_.set("uModel",Mat4::identity());
    mountainShader_.set("uNormalMatrix",normalMatrix(Mat4::identity()));
    mountainShader_.set("uDistance",0.0f);
    for(const ScenePart& part:city_) {
        mountainShader_.set("uBaseColor",part.color);
        mountainShader_.set("uPalm",part.flat);
        part.mesh->draw();
    }
    static const Vec3 rockTints[]{{0.30f,0.12f,0.38f},{0.40f,0.17f,0.46f},{0.24f,0.10f,0.32f},
                                  {0.36f,0.15f,0.42f},{0.28f,0.11f,0.36f},{0.44f,0.20f,0.48f}};
    for(const auto& s:sections_) {
        if(!s.plain) {
            const Mat4 model=translation({s.x,0,s.z})*scaling({s.scale,s.scale,s.scale});
            mountainShader_.set("uModel",model);
            mountainShader_.set("uNormalMatrix",normalMatrix(model));
            mountainShader_.set("uBaseColor",rockTints[s.variant]);
            mountainShader_.set("uDistance",std::fabs(s.z));
            mountainShader_.set("uPalm",0.0f);
            massifs_[s.variant]->draw();
        }
        if(s.palm) {
            // Palms lean toward the road: the sign of the section's X decides the side.
            const float side=s.x>0 ? 1.0f : -1.0f;
            const float roadEdgeX=side*(roadHalfWidth+3.5f);
            const Mat4 palmBase=translation({roadEdgeX,0.0f,s.z-2.0f})*rotationZ(-side*0.12f)*scaling({s.palmScale,s.palmScale,s.palmScale});
            mountainShader_.set("uModel",palmBase);
            mountainShader_.set("uNormalMatrix",normalMatrix(palmBase));
            mountainShader_.set("uBaseColor",Vec3{0.30f,0.20f,0.16f});
            mountainShader_.set("uPalm",0.0f);
            palmTrunk_->draw();
            const Mat4 crown=palmBase*translation({0,3.2f,0});
            mountainShader_.set("uModel",crown);
            mountainShader_.set("uNormalMatrix",normalMatrix(crown));
            mountainShader_.set("uBaseColor",Vec3{0.12f,0.30f,0.24f});
            mountainShader_.set("uPalm",1.0f);
            palmFronds_->draw();
        }
        if(s.prop!=0) {
            const std::size_t kind=s.prop-1;
            const float side=s.x>0 ? 1.0f : -1.0f;
            // Props sit on the far side of the plain, facing the road.
            const Mat4 base=translation({side*(roadHalfWidth+6.0f),0.0f,s.z-4.0f})*rotationY(side>0 ? 3.14159f : 0.0f);
            for(const ScenePart& part:props_[kind]) {
                mountainShader_.set("uModel",base);
                mountainShader_.set("uNormalMatrix",normalMatrix(base));
                mountainShader_.set("uBaseColor",part.color);
                mountainShader_.set("uDistance",part.flat>0.5f ? 0.0f : std::fabs(s.z));
                mountainShader_.set("uPalm",part.flat);
                part.mesh->draw();
            }
        }
    }
}

void Environment::shutdown() {
    massifs_.clear();
    palmTrunk_.reset();
    palmFronds_.reset();
    for(auto& kind:props_) kind.clear();
    city_.clear();
    sections_.clear();
    if(backgroundVao_) glDeleteVertexArrays(1,&backgroundVao_);
    backgroundVao_=0;
    backgroundShader_.shutdown();
    mountainShader_.shutdown();
}
}
