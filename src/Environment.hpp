#pragma once
#include "Math.hpp"
#include "Mesh.hpp"
#include "RenderContext.hpp"
#include "Shader.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>

namespace sw {
// One drawable piece of a roadside prop, in the prop's local coordinates (base centre at the origin).
struct ScenePart {
    std::unique_ptr<Mesh> mesh;
    Vec3 color;
    float flat;  // 1 for emissive-looking signs (flat tone, no fog), 0 for lit surfaces.
};

// Gradient sky, striped sun, massifs on both sides of the road, plains between them,
// palms, and occasional roadside props (shop, service station, fountain).
// The background pass never writes depth; everything else is ordinary depth-tested geometry.
class Environment {
public:
    Environment()=default;
    ~Environment() { shutdown(); }
    Environment(const Environment&)=delete;
    Environment& operator=(const Environment&)=delete;
    void initialize(const std::filesystem::path& shaderDirectory);
    void update(float deltaTime,float speed);
    void renderBackground(const RenderContext& context) const;
    void renderMountains(const RenderContext& context) const;
    void reset();
    void shutdown();
private:
    struct Section {
        float z;                 // Centre along the road (world Z); scenery moves toward +Z.
        float x;                 // Lateral centre, always beyond the road edge.
        float scale;             // Depth layer: far massifs are slightly smaller.
        std::size_t variant;     // Index into massifs_.
        std::uint32_t seed;      // Per-section random state, advanced on every recycle.
        bool palm;               // Whether a palm stands at the road edge in front of this massif.
        float palmScale;
        bool plain;              // Flat plain: no massif in this section.
        std::uint8_t prop;       // 0 = none, 1 = shop, 2 = service station, 3 = fountain.
    };
    static constexpr float sectionLength=12.0f;
    static constexpr int kRowsPerSide=12;
    static constexpr float cameraRecycleZ=10.0f;
    static constexpr std::size_t kVariantCount=6;
    void buildSections();
    void assignSection(Section& section,std::uint32_t seed) const;
    float depthScale(float z) const;
    Shader backgroundShader_;
    Shader mountainShader_;
    GLuint backgroundVao_=0;
    std::vector<std::unique_ptr<Mesh>> massifs_;
    std::unique_ptr<Mesh> palmTrunk_;
    std::unique_ptr<Mesh> palmFronds_;
    std::array<std::vector<ScenePart>,3> props_;
    std::vector<ScenePart> city_;
    std::vector<Section> sections_;
    float skyTime_=0;
};
} // namespace sw
