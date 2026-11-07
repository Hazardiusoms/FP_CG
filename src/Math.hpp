#pragma once
#include <cmath>
#include <stdexcept>

namespace sw {
constexpr float pi = 3.14159265358979323846f;
inline float radians(float degrees) { return degrees * pi / 180.0f; }
struct Vec3 {
    float x = 0, y = 0, z = 0;
    Vec3 operator+(Vec3 b) const { return {x+b.x, y+b.y, z+b.z}; }
    Vec3 operator-(Vec3 b) const { return {x-b.x, y-b.y, z-b.z}; }
    Vec3 operator*(float s) const { return {x*s, y*s, z*s}; }
};
inline float dot(Vec3 a, Vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
inline Vec3 cross(Vec3 a, Vec3 b) {
    return {a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x};
}
inline float length(Vec3 v) { return std::sqrt(dot(v,v)); }
inline Vec3 normalized(Vec3 v) {
    const float n = length(v);
    if (!(n > 1e-8f)) throw std::invalid_argument("Cannot normalize a zero vector");
    return v * (1.0f/n);
}
struct Vec4 { float x, y, z, w; };
// Column-major storage; column vectors; upload with transpose=GL_FALSE.
struct Mat4 {
    float values[16]{};
    float& at(int row, int col) { return values[col*4+row]; }
    float at(int row, int col) const { return values[col*4+row]; }
    const float* data() const { return values; }
    static Mat4 identity() {
        Mat4 m; for (int i=0;i<4;++i) m.at(i,i)=1; return m;
    }
};
struct Mat3 {
    float values[9]{};
    float& at(int row,int col) { return values[col*3+row]; }
    float at(int row,int col) const { return values[col*3+row]; }
    const float* data() const { return values; }
};
inline Mat4 operator*(const Mat4& a, const Mat4& b) {
    Mat4 c;
    for(int row=0;row<4;++row)
        for(int col=0;col<4;++col)
            for(int k=0;k<4;++k) c.at(row,col)+=a.at(row,k)*b.at(k,col);
    return c;
}
inline Vec4 operator*(const Mat4& m, Vec4 v) {
    return {m.at(0,0)*v.x+m.at(0,1)*v.y+m.at(0,2)*v.z+m.at(0,3)*v.w,
            m.at(1,0)*v.x+m.at(1,1)*v.y+m.at(1,2)*v.z+m.at(1,3)*v.w,
            m.at(2,0)*v.x+m.at(2,1)*v.y+m.at(2,2)*v.z+m.at(2,3)*v.w,
            m.at(3,0)*v.x+m.at(3,1)*v.y+m.at(3,2)*v.z+m.at(3,3)*v.w};
}
inline Vec3 operator*(const Mat3& m, Vec3 v) {
    return {m.at(0,0)*v.x+m.at(0,1)*v.y+m.at(0,2)*v.z,
            m.at(1,0)*v.x+m.at(1,1)*v.y+m.at(1,2)*v.z,
            m.at(2,0)*v.x+m.at(2,1)*v.y+m.at(2,2)*v.z};
}
inline Mat4 translation(Vec3 v) {
    Mat4 m=Mat4::identity(); m.at(0,3)=v.x; m.at(1,3)=v.y; m.at(2,3)=v.z; return m;
}
inline Mat4 scaling(Vec3 v) {
    Mat4 m=Mat4::identity(); m.at(0,0)=v.x; m.at(1,1)=v.y; m.at(2,2)=v.z; return m;
}
inline Mat4 rotationX(float a) {
    Mat4 m=Mat4::identity(); const float c=std::cos(a),s=std::sin(a);
    m.at(1,1)=c; m.at(1,2)=-s; m.at(2,1)=s; m.at(2,2)=c; return m;
}
inline Mat4 rotationY(float a) {
    Mat4 m=Mat4::identity(); const float c=std::cos(a),s=std::sin(a);
    m.at(0,0)=c; m.at(0,2)=s; m.at(2,0)=-s; m.at(2,2)=c; return m;
}
inline Mat4 rotationZ(float a) {
    Mat4 m=Mat4::identity(); const float c=std::cos(a),s=std::sin(a);
    m.at(0,0)=c; m.at(0,1)=-s; m.at(1,0)=s; m.at(1,1)=c; return m;
}
inline Mat4 lookAt(Vec3 eye, Vec3 target, Vec3 up) {
    const Vec3 forward=normalized(target-eye);
    const Vec3 right=normalized(cross(forward,up));
    const Vec3 correctedUp=cross(right,forward);
    Mat4 m=Mat4::identity();
    m.at(0,0)=right.x; m.at(0,1)=right.y; m.at(0,2)=right.z;
    m.at(1,0)=correctedUp.x; m.at(1,1)=correctedUp.y; m.at(1,2)=correctedUp.z;
    m.at(2,0)=-forward.x; m.at(2,1)=-forward.y; m.at(2,2)=-forward.z;
    m.at(0,3)=-dot(right,eye); m.at(1,3)=-dot(correctedUp,eye); m.at(2,3)=dot(forward,eye);
    return m;
}
inline Mat4 perspective(float fov, float aspect, float nearPlane, float farPlane) {
    if (!(fov>0 && fov<pi && aspect>0 && nearPlane>0 && farPlane>nearPlane))
        throw std::invalid_argument("Invalid perspective parameters");
    Mat4 m; const float f=1.0f/std::tan(fov*0.5f);
    m.at(0,0)=f/aspect; m.at(1,1)=f;
    m.at(2,2)=(farPlane+nearPlane)/(nearPlane-farPlane);
    m.at(2,3)=2*farPlane*nearPlane/(nearPlane-farPlane);
    m.at(3,2)=-1; return m;
}
// Cofactor matrix / determinant = inverse transpose, including non-uniform scale.
inline Mat3 normalMatrix(const Mat4& m) {
    const Vec3 a{m.at(0,0),m.at(1,0),m.at(2,0)};
    const Vec3 b{m.at(0,1),m.at(1,1),m.at(2,1)};
    const Vec3 c{m.at(0,2),m.at(1,2),m.at(2,2)};
    const float determinant=dot(a,cross(b,c));
    if (std::abs(determinant)<1e-8f) throw std::invalid_argument("Singular normal matrix");
    const Vec3 columns[]{cross(b,c)*(1/determinant),cross(c,a)*(1/determinant),cross(a,b)*(1/determinant)};
    Mat3 n;
    for(int col=0;col<3;++col) {
        n.at(0,col)=columns[col].x; n.at(1,col)=columns[col].y; n.at(2,col)=columns[col].z;
    }
    return n;
}
} // namespace sw
