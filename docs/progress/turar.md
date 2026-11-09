# Turar car module

Date: 9 November 2026. Branch: `turar/car`. Base: `feccee7` (latest main, including Almas's road/dashboard update and Muslim's environment). Publication requested explicitly by Turar after local testing.

## Implemented

`CarModel.hpp/.cpp` generates indexed hard-face boxes and 12-sided cylinder wheels, with position at attribute 0 and unit normal at attribute 1. It uses existing `Math.hpp` and `SceneState.hpp`. `Car.hpp/.cpp` uploads four reusable meshes once through the common Mesh utility. Parts are composed with shared model matrices; no private camera, input loop or math library was added.

The model contains chassis, hood, cabin, roof, bumpers, headlights, rear light strip, neon sills, spoiler, four wheels, hubs and visible rotating spokes. Car faces -Z. Body is 1.7 by 3.1 units; wheel radius is 0.3. Root translation carries all parts. Body roll is around Y=0.52; wheels inherit translation but remain at ground level. Wheel rotation is local X and negative for forward travel along -Z.

A/D or arrow keys steer at 3 scene units/second. Position clamps to +/-2.9, preserving a conservative envelope and margin within the +/-4 road. Five-degree body roll uses exponential smoothing. Rotation uses speed/radius and wraps with remainder. Pause and zero speed stop wheels; steering remains available while the road is paused. Reset restores position, tilt and wheel angle.

## Integration

Main replaces ReferenceMarker with Car, updates it after updateScene and before the camera, resets it with the other modules and renders it after mountains. CMake compiles Car.cpp and adds CarModel.cpp to synthwave_core. HUD labels describe the new controls. These small integration edits need Almas's review.

`car_preview.vert/.frag` provide temporary orientation shading and emissive colors, not Phong, two lights or bloom. Common Lighting remains Nursalim's task. The `drawParts(state, binder)` interface supplies each model matrix, inverse-transpose normal matrix and CarMaterial to a binder before drawing. Nursalim can bind his common program and frame data, then upload these per-part values without changing mesh ownership. No light-service API is invented or overwritten.

## Local build environment

Windows, MSVC 19.44, Windows SDK 10.0.26100, Intel UHD Graphics, OpenGL 3.3 (driver 30.0.101.3111). Initial unmodified framework configuration failed because GLFW was not installed in a discoverable location. Existing course GLFW sources and existing GLEW 2.2.0 sources were compiled into ignored `build/local-deps`; no personal paths were added to tracked CMake.

With CMake/CTest on PATH and those dependencies installed locally, the commands used are equivalent to:

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH="$pwd/build/local-deps" -DGLEW_USE_STATIC_LIBS=ON -DGLEW_INCLUDE_DIR="$pwd/build/local-deps/include" -DGLEW_STATIC_LIBRARY_RELEASE="$pwd/build/local-deps/lib/glew.lib" '-DCMAKE_CXX_FLAGS=/DWIN32 /D_WINDOWS /W3 /GR /EHsc /DGLEW_STATIC'
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
./build/Release/synthwave_drive.exe --smoke-test build/smoke-car
./build/Release/synthwave_drive.exe
```

The local glew-cmake package exports a different target than FindGLEW expects. Its generated config folder was moved out of the local dependency prefix so FindGLEW uses the explicit header/library paths above. This is a local toolchain workaround, not a new project dependency or a claimed turnkey setup for every Windows machine. Other machines should use their course GLFW/GLEW packages. Build output and dependencies are not committed.

## Verification

- Release build passed on Windows.
- CTest passed framework_core (44 existing checks) and car_model.
- Car tests cover nondegenerate outward triangles, unit normals, steering/wheel frame independence at 30 and 144 updates per second, every transformed vertex inside the road and above ground at both steering limits, no lateral drift on release, pause/zero-speed wheels, parent translation, local axle pivots and reset.
- Real OpenGL smoke test passed existing camera, road, reset, resize, mouse orbit, pause, palette and GL-error checks. Added steering checks exercise both limits and exact reset-frame restoration.
- Actual framebuffer captures: `docs/evidence/turar-car-follow.png` and `turar-car-presentation.png`. These are application readbacks converted from PPM to PNG; FPS shows -- in deterministic captures.
- Human keyboard/mouse usability review, TA-machine performance, common lighting and bloom remain unverified/pending. No FPS benchmark result is claimed.

## Sources and AI use

OpenAI Codex generated the car geometry, control implementation, preview shaders, tests and integration changes, and performed the recorded local build/checks. Geometry was constructed from primitives without external models or textures. Existing team Math, Mesh, Shader, input and scene code are reused. Turar must review and explain the implementation; no human review is claimed yet.

## Human publication

Review `git status`, `git diff`, new files and the running scene. Stage only the car implementation, two preview shaders, car tests, this progress record, actual evidence and the related CMake/main/HUD/README changes. Review with `git diff --cached --check` and `git diff --cached`; then personally commit and push `turar/car`, and open a PR to main. Include Almas for integration review and Nursalim for the lighting handoff.

## Visual revision

Refined the car into a low wedge-shaped retro sports car with a tapered sloped-glass canopy, rear louvers, segmented taillights, spoiler supports and neon edge, front splitter, side intakes, exhausts and five-spoke rims. Four shared meshes cover box, wheel, wedge body and canopy. Face normals are recalculated after shaping the wedge geometry. Updated geometry tests cover all four meshes and full-part road/ground clearance. Release build, both CTest suites and the OpenGL smoke test passed again; follow and orbit screenshots were refreshed and visually inspected. The preview shader remains temporary, without real specular lighting or bloom. Codex implemented and checked this revision at Turar's request.

## Integration with main f42b18d

Preserved a complete local car backup outside the repository before fast-forwarding turar/car to origin/main. Reapplied only the car CMake entries and main/HUD hooks. Kept the new road, guardrails, lamps, MSAA, dashboard and camera implementation. Car update precedes updateFollowCamera; smoke checks verify nonzero tracking in the first steering frame and reset of followX. Windows Release build, both CTest suites and real OpenGL smoke checks passed. Evidence images were refreshed from the integrated build. No commit or push was performed.

## Publication authorization

Turar explicitly requested a focused commit, push of turar/car and immediate merge into main. This task-specific instruction overrides the default human-only publication workflow for these actions. Tests were rerun successfully on feccee7 with the latest sun shader; evidence images were refreshed. No independent teammate review is claimed.
