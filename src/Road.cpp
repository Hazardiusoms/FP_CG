#include "Road.hpp"
#include <array>
namespace sw {
namespace {
// Each face has a constant tint so the bevel-free box remains readable in 3D.
void box(std::vector<float> &vertices, std::vector<uint32_t> &indices, Vec3 lo, Vec3 hi,
         Vec3 color) {
    const std::array<Vec3, 8> p = {Vec3{lo.x, lo.y, lo.z}, {hi.x, lo.y, lo.z}, {hi.x, hi.y, lo.z},
                                   {lo.x, hi.y, lo.z},     {lo.x, lo.y, hi.z}, {hi.x, lo.y, hi.z},
                                   {hi.x, hi.y, hi.z},     {lo.x, hi.y, hi.z}};
    const int faces[6][4] = {{0, 1, 2, 3}, {5, 4, 7, 6}, {4, 0, 3, 7},
                             {1, 5, 6, 2}, {3, 2, 6, 7}, {4, 5, 1, 0}};
    const float shade[6] = {0.7f, 0.85f, 0.65f, 0.8f, 1, 0.45f};
    for (int f = 0; f < 6; ++f) {
        const auto start = static_cast<uint32_t>(vertices.size() / 6);
        for (int i : faces[f])
            vertices.insert(vertices.end(), {p[i].x, p[i].y, p[i].z, color.x * shade[f],
                                             color.y * shade[f], color.z * shade[f]});
        indices.insert(indices.end(), {start, start + 1, start + 2, start, start + 2, start + 3});
    }
}
} // namespace
void Road::initialize(const std::filesystem::path &directory) {
    shutdown();
    reset();
    shader_.initialize(directory / "road.vert", directory / "road.frag");
    roadsideShader_.initialize(directory / "roadside.vert", directory / "roadside.frag");
    mesh_.initialize(
        {-80, 0, -300, -40, -150, -80, 0, 60, -40, 30, 80, 0, 60, 40, 30, 80, 0, -300, 40, -150}, 5,
        {0, 1, 2, 0, 2, 3}, {{0, 3, 0}, {2, 2, 3}});
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    for (float side : {-1.0f, 1.0f}) {
        const float x = side * 4.55f;
        box(vertices, indices, {x - 0.17f, 0, -300}, {x + 0.17f, 0.18f, 65}, {0.15f, 0.11f, 0.23f});
        box(vertices, indices, {x - 0.08f, 0.50f, -300}, {x + 0.08f, 0.65f, 65},
            {0.20f, 0.14f, 0.29f});
        box(vertices, indices, {x - 0.085f, 0.65f, -300}, {x + 0.085f, 0.69f, 65},
            {0.10f, 0.78f, 0.95f});
    }
    rails_.initialize(vertices, 6, indices, {{0, 3, 0}, {1, 3, 3}});
    vertices.clear();
    indices.clear();
    for (int z = -288; z <= 60; z += 12)
        for (float side : {-1.0f, 1.0f}) {
            const float x = side * 4.55f;
            box(vertices, indices, {x - 0.1f, 0, float(z) - 0.1f},
                {x + 0.1f, 0.63f, float(z) + 0.1f}, {0.22f, 0.14f, 0.3f});
            box(vertices, indices, {x - 0.14f, 0.70f, float(z) - 0.22f},
                {x + 0.14f, 0.83f, float(z) + 0.22f}, {1, 0.12f, 0.46f});
            const float pole = side * 5.15f;
            box(vertices, indices, {pole - 0.07f, 0, float(z) - 0.07f},
                {pole + 0.07f, 3.1f, float(z) + 0.07f}, {0.18f, 0.15f, 0.26f});
            const float inner = side * 4.0f;
            box(vertices, indices, {std::min(pole, inner) - 0.09f, 3.0f, float(z) - 0.10f},
                {std::max(pole, inner) + 0.09f, 3.13f, float(z) + 0.10f}, {0.25f, 0.19f, 0.32f});
            box(vertices, indices, {inner - 0.22f, 2.96f, float(z) - 0.14f},
                {inner + 0.22f, 3.0f, float(z) + 0.14f}, {1, 0.42f, 0.72f});
        }
    markers_.initialize(vertices, 6, indices, {{0, 3, 0}, {1, 3, 3}});
}
void Road::update(float dt, float speed) {
    offset_ = advanceGridOffset(offset_, speed, dt);
    travel_ = std::fmod(travel_ + static_cast<double>(speed) * dt, 12.0);
    if (travel_ < 0)
        travel_ += 12.0;
}
void Road::render(const RenderContext &ctx, bool alternatePalette) const {
    shader_.use();
    shader_.set("uModel", Mat4::identity());
    shader_.set("uView", ctx.view);
    shader_.set("uProjection", ctx.projection);
    shader_.set("uOffset", static_cast<float>(offset_));
    shader_.set("uTravel", static_cast<float>(travel_));
    shader_.set("uRoadHalfWidth", roadHalfWidth);
    shader_.set("uCameraPosition", ctx.cameraPosition);
    shader_.set("uAlternatePalette", alternatePalette ? 1.0f : 0.0f);
    mesh_.draw();
    roadsideShader_.use();
    roadsideShader_.set("uView", ctx.view);
    roadsideShader_.set("uProjection", ctx.projection);
    roadsideShader_.set("uCameraPosition", ctx.cameraPosition);
    roadsideShader_.set("uAlternatePalette", alternatePalette ? 1.0f : 0.0f);
    roadsideShader_.set("uShift", 0.0f);
    rails_.draw();
    roadsideShader_.set("uShift", static_cast<float>(travel_));
    markers_.draw();
}
void Road::shutdown() {
    mesh_.shutdown();
    rails_.shutdown();
    markers_.shutdown();
    shader_.shutdown();
    roadsideShader_.shutdown();
}
} // namespace sw
