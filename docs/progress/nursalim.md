# Nursalim Onalbayev — lighting and effects progress

Branch: `nursalim/lighting-effects` (from `main` at f89b03b). Status: first lighting pass written and checked on Linux with a software OpenGL renderer.

## Done in this pass

- `src/LightingModel.hpp`: CPU-only light description (sun + moving point light, orbit, light clock, palette colours).
- `src/Lighting.hpp/.cpp`: lit shader owner; `update`, `reset`, `beginLit`, `bindMaterial`, `drawLightMarker`.
- `shaders/lit.vert`, `shaders/lit.frag`: per-fragment Blinn-Phong, two lights, world space.
- Car integration through the existing `Car::drawParts` hook in `main.cpp` (the old `Car::render` preview path is kept but no longer called).
- F toggles the moving light (`Key::F`, `InputState::toggleLight`, `SceneState::movingLightEnabled`). HUD hint line updated.
- `tests/lighting_tests.cpp` (new CTest `lighting_model`) and extra GPU smoke checks in `main.cpp`.

## Shared files touched (please confirm with the owners)

| File | Change | Owner |
| --- | --- | --- |
| `src/CarModel.hpp/.cpp` | `CarMaterial` gets `specular` and `shininess` (defaults 0.35 / 32); paint, glass, rubber and metal set explicitly | Turar |
| `src/Input.hpp`, `src/SceneState.hpp` | `Key::F`, `toggleLight`, `movingLightEnabled` | Almas |
| `src/main.cpp`, `CMakeLists.txt`, `src/Hud.cpp` | Lighting lifecycle calls, F key, smoke checks, build entries, hint text (the "SCROLL ZOOM" hint was shortened to make room for F) | Almas |

## Checks actually run (Ubuntu 24.04, Xvfb, Mesa llvmpipe 25.2, OpenGL 3.3 core, Release)

| Check | Result |
| --- | --- |
| `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build` | passed (only a pre-existing `Mesh.cpp` indentation warning) |
| `ctest --test-dir build --output-on-failure` | passed, 3 of 3 suites (framework_core, car_model, lighting_model) |
| `synthwave_drive --smoke-test` under Xvfb | passed, including the new light toggle / motion / pause / reset checks |
| Normals on all car parts (non-uniform scale, tilt, rotation) | passed (every triangle, `lighting_model`) |
| `synthwave_drive --benchmark 300`, 1280x720 | 26.4 avg FPS with lighting vs 27.5 without, **on a CPU software renderer** |

The benchmark is not evidence for R7: it runs on a virtual Xeon with llvmpipe, not a GPU. It only shows the lit shader adds little cost relative to the rest of the frame there.

Evidence: [follow camera](../evidence/nursalim-lit-car-follow.png), [light on](../evidence/nursalim-light-on.png), [light off](../evidence/nursalim-light-off.png) (smoke-test frames, software renderer).

## Not checked / limitations

- Only the car is lit by the new model. Road, mountains, sky and palms keep their own shading; the road does not show the moving light's pool yet.
- Bloom and CRT are not started (Core and R1–R8 come first).

## Sources and AI help

Blinn-Phong is the standard model from the course material; no third-party code or assets were added. Claude (Anthropic) wrote the first version of the code, tests and this note. 
