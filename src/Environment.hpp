#pragma once
#include "Mesh.hpp"
#include "RenderContext.hpp"
#include "Shader.hpp"
#include <filesystem>
#include <memory>
#include <vector>

namespace sw {
// Gradient sky, striped sun and low-poly mountain sections on both sides of the road.
// The background pass never writes depth; mountains are ordinary depth-tested geometry.
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
        float z;             // Centre of the section along the road (world Z).
        float x;             // Lateral centre; always outside the road.
        std::size_t variant; // Index into meshes_.
    };
    static constexpr float sectionLength=12.0f;
    static constexpr int kRowsPerSide=12;
    static constexpr float cameraRecycleZ=10.0f;
    void buildSections();
    Shader backgroundShader_;
    Shader mountainShader_;
    GLuint backgroundVao_=0;
    std::vector<std::unique_ptr<Mesh>> meshes_;
    std::vector<Section> sections_;
};
} // namespace sw
