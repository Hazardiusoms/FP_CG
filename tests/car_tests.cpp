#include "CarModel.hpp"
#include <iostream>
#include <stdexcept>
using namespace sw;
void check(bool ok,const char* message) { if(!ok) throw std::runtime_error(message); }
int main() {
    try {
        const auto box=CarModel::box(),wheel=CarModel::wheel(),wedge=CarModel::wedge(false),cabin=CarModel::wedge(true);
        for(const auto* mesh:{&box,&wheel,&wedge,&cabin}) {
            for(std::size_t i=0;i<mesh->indices.size();i+=3) {
                auto position=[&](std::size_t j) { const auto n=mesh->indices[j]*6; return Vec3{mesh->vertices[n],mesh->vertices[n+1],mesh->vertices[n+2]}; };
                const Vec3 a=position(i),b=position(i+1),c=position(i+2);
                const auto n=mesh->indices[i]*6;
                const Vec3 normal{mesh->vertices[n+3],mesh->vertices[n+4],mesh->vertices[n+5]};
                check(length(cross(b-a,c-a))>0.0001f,"Degenerate triangle");
                check(std::abs(length(normal)-1)<0.0001f,"Non-unit normal");
                check(dot(normal,(a+b+c)*(1.0f/3))>0,"Inward face");
            }
        }
        CarModel car,other; SceneState s,t; InputState input; input.steering=1;
        for(int i=0;i<30;++i)car.update(1.0f/30,input,s);
        for(int i=0;i<144;++i)other.update(1.0f/144,input,t);
        check(std::abs(s.carX-t.carX)<0.0001f && std::abs(s.steeringTilt-t.steeringTilt)<0.0001f,"Frame-dependent steering");
        check(std::abs(std::remainder(car.wheelAngle()-other.wheelAngle(),2*pi))<0.0001f,"Frame-dependent wheel rotation");
        for(float direction:{-1.0f,1.0f}) {
            input.steering=direction;
            for(int i=0;i<500;++i)car.update(.05f,input,s);
            check(s.carX==direction*CarModel::lateralLimit,"Boundary clamp");
            for(const auto& part:car.parts(s)) {
                const auto& mesh=part.mesh==0 ? box : part.mesh==1 ? wheel : part.mesh==2 ? wedge : cabin;
                for(std::size_t i=0;i<mesh.vertices.size();i+=6) {
                    const Vec4 p=part.model*Vec4{mesh.vertices[i],mesh.vertices[i+1],mesh.vertices[i+2],1};
                    check(std::abs(p.x)<roadHalfWidth-.09f,"Part outside road");
                    check(p.y>=-0.0001f,"Part below road");
                }
            }
        }
        input.steering=0; const float x=s.carX; car.update(.05f,input,s); check(s.carX==x,"Drift after release");
        s.speed=0; float angle=car.wheelAngle();car.update(.05f,input,s); check(angle==car.wheelAngle(),"Zero speed moves wheels");
        s.speed=20;s.paused=true;car.update(.05f,input,s);check(angle==car.wheelAngle(),"Pause moves wheels");
        auto before=car.parts(s);s.carX-=1;auto after=car.parts(s);
        for(std::size_t i=0;i<before.size();++i)check(std::abs(before[i].model.at(0,3)-after[i].model.at(0,3)-1)<.0001f,"Detached child");
        s.paused=false;car.update(.02f,input,s);auto spun=car.parts(s);
        for(std::size_t i=0;i<after.size();++i)if(after[i].mesh==1)
            check(length(Vec3{after[i].model.at(0,3)-spun[i].model.at(0,3),after[i].model.at(1,3)-spun[i].model.at(1,3),after[i].model.at(2,3)-spun[i].model.at(2,3)})<.0001f,"Wheel orbiting world origin");
        input.reset=true;car.update(.05f,input,s);check(s.carX==0&&s.steeringTilt==0&&car.wheelAngle()==0,"Reset");
        std::cout<<"PASS: geometry normals/winding, frame independence, both road bounds, ground clearance, release, pause, zero speed, hierarchy, axle pivots, reset\n";
    } catch(const std::exception& e) { std::cerr<<e.what()<<'\n'; return 1; }
}
