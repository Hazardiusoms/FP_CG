#include "CarModel.hpp"

namespace sw {
namespace {
void triangle(CarGeometry& mesh,Vec3 a,Vec3 b,Vec3 c) {
    const Vec3 normal=normalized(cross(b-a,c-a));
    for(Vec3 p:{a,b,c}) {
        mesh.indices.push_back(static_cast<std::uint32_t>(mesh.vertices.size()/6));
        mesh.vertices.insert(mesh.vertices.end(),{p.x,p.y,p.z,normal.x,normal.y,normal.z});
    }
}
}
CarGeometry CarModel::box() {
    CarGeometry mesh;
    const Vec3 p[]={{-.5f,-.5f,-.5f},{.5f,-.5f,-.5f},{.5f,.5f,-.5f},{-.5f,.5f,-.5f},
                    {-.5f,-.5f,.5f},{.5f,-.5f,.5f},{.5f,.5f,.5f},{-.5f,.5f,.5f}};
    const int faces[][4]={{0,3,2,1},{4,5,6,7},{0,4,7,3},{1,2,6,5},{3,7,6,2},{0,1,5,4}};
    for(const auto& f:faces) { triangle(mesh,p[f[0]],p[f[1]],p[f[2]]); triangle(mesh,p[f[0]],p[f[2]],p[f[3]]); }
    return mesh;
}
CarGeometry CarModel::wedge(bool cabin) {
    const auto source=box(); CarGeometry mesh;
    auto shape=[&](std::size_t i) {
        Vec3 p{source.vertices[i],source.vertices[i+1],source.vertices[i+2]};
        if(p.y>0) {
            if(cabin) { p.x*=.76f; p.z=p.z*.48f+.04f; }
            else { p.y=p.z<0 ? -.12f : .5f; p.x*=.94f; }
        }
        return p;
    };
    for(std::size_t i=0;i<source.vertices.size();i+=18)
        triangle(mesh,shape(i),shape(i+6),shape(i+12));
    return mesh;
}
CarGeometry CarModel::wheel() {
    CarGeometry mesh;
    // Unit-radius cylinder along X; separate cap/side vertices preserve hard edges.
    for(int i=0;i<12;++i) {
        const float a=2*pi*i/12,b=2*pi*(i+1)/12;
        const Vec3 l{-.5f,std::cos(a),std::sin(a)},r{.5f,l.y,l.z};
        const Vec3 ln{-.5f,std::cos(b),std::sin(b)},rn{.5f,ln.y,ln.z};
        triangle(mesh,l,ln,rn); triangle(mesh,l,rn,r);
        triangle(mesh,{-.5f,0,0},ln,l); triangle(mesh,{.5f,0,0},r,rn);
    }
    return mesh;
}
void CarModel::update(float dt,const InputState& input,SceneState& state) {
    if(input.reset) { reset(state); return; }
    if(!std::isfinite(dt) || dt<=0) return;
    dt=std::min(dt,0.05f); // Same pause protection as the main loop.
    const float steer=std::isfinite(input.steering) ? std::clamp(input.steering,-1.0f,1.0f) : 0;
    state.carX=std::clamp(state.carX+steer*3.0f*dt,-lateralLimit,lateralLimit);
    const float target=-steer*radians(5);
    state.steeringTilt+=(target-state.steeringTilt)*(-std::expm1(-10*dt));
    if(!state.paused && std::isfinite(state.speed))
        wheelAngle_=std::remainder(wheelAngle_-std::max(0.0f,state.speed)*dt/wheelRadius,2*pi);
}
std::vector<CarPart> CarModel::parts(const SceneState& state) const {
    std::vector<CarPart> result;
    // Roll the body about its centre; wheels remain on the road and inherit translation.
    const Mat4 root=translation({state.carX,0,0});
    const Mat4 body=root*translation({0,.52f,0})*rotationZ(state.steeringTilt)*translation({0,-.52f,0});
    const CarMaterial paint{{.45f,.12f,.62f},{.018f,.004f,.028f}},glass{{.035f,.21f,.29f},{.008f,.025f,.035f}};
    const CarMaterial rubber{{.035f,.04f,.06f},{0,0,0}},metal{{.32f,.40f,.49f},{0,0,0}};
    const CarMaterial pink{{.9f,.03f,.45f},{.8f,.015f,.28f}},cyan{{.10f,.85f,1},{.08f,.65f,.9f}};
    auto add=[&](const char* name,Vec3 p,Vec3 scale,CarMaterial material) {
        result.push_back({name,0,body*translation(p)*scaling(scale),material});
    };
    result.push_back({"wedge body",2,body*translation({0,.48f,0})*scaling({1.7f,.44f,3.1f}),paint});
    result.push_back({"sloped glass canopy",3,body*translation({0,.82f,.15f})*scaling({1.38f,.40f,1.5f}),glass});
    add("roof",{0,1.035f,.21f},{1.07f,.045f,.74f},paint);
    add("front splitter",{0,.29f,-1.56f},{1.74f,.06f,.16f},rubber);
    add("rear diffuser",{0,.29f,1.53f},{1.65f,.13f,.10f},rubber);
    add("front neon edge",{0,.35f,-1.575f},{1.62f,.022f,.025f},cyan);
    add("rear neon edge",{0,.38f,1.565f},{1.62f,.024f,.025f},pink);
    add("left headlight",{-.53f,.45f,-1.56f},{.46f,.065f,.025f},cyan);
    add("right headlight",{.53f,.45f,-1.56f},{.46f,.065f,.025f},cyan);
    add("rear light recess",{0,.57f,1.56f},{1.55f,.16f,.03f},rubber);
    for(float side:{-1.0f,1.0f}) {
        for(int i=0;i<3;++i) add("segmented taillight",{side*(.29f+.19f*i),.58f,1.58f},{.14f,.075f,.018f},pink);
        add("spoiler support",{side*.58f,.76f,1.28f},{.045f,.24f,.075f},metal);
        add("exhaust",{side*.57f,.28f,1.60f},{.18f,.07f,.10f},metal);
        add("roof neon rail",{side*.54f,1.06f,.21f},{.018f,.018f,.76f},cyan);
        add("side intake",{side*.843f,.51f,.50f},{.025f,.14f,.44f},rubber);
    }
    for(int i=0;i<5;++i) {
        const float z=.60f+.065f*i;
        const float y=1.02f-(z-.57f)*(.4f/.33f);
        add("rear glass louver",{0,y+.012f,z},{1.16f,.025f,.035f},rubber);
    }
    add("left neon sill",{-.815f,.30f,0},{.035f,.04f,2.75f},cyan);
    add("right neon sill",{.815f,.30f,0},{.035f,.04f,2.75f},cyan);
    add("spoiler",{0,.89f,1.28f},{1.76f,.055f,.24f},paint);
    add("spoiler neon edge",{0,.89f,1.405f},{1.73f,.018f,.018f},pink);
    for(float x:{-.86f,.86f}) for(float z:{-.95f,.95f}) {
        const Mat4 axle=root*translation({x,wheelRadius,z})*rotationX(wheelAngle_);
        result.push_back({"wheel",1,axle*scaling({.22f,wheelRadius,wheelRadius}),rubber});
        const float side=x<0 ? -.116f : .116f;
        result.push_back({"hub",1,axle*translation({side,0,0})*scaling({.015f,.18f,.18f}),metal});
        result.push_back({"rim inset",1,axle*translation({side*1.08f,0,0})*scaling({.012f,.15f,.15f}),rubber});
        for(int i=0;i<5;++i)
            result.push_back({"wheel spoke",0,axle*translation({side*1.16f,0,0})*rotationX(i*2*pi/5)*translation({0,.085f,0})*scaling({.018f,.15f,.035f}),cyan});
    }
    return result;
}
}
