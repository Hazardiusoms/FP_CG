# Almas Alkhan — Week 8 Framework Handoff

**Date:** 31 October 2026
**Intended branch:** `almas/core-road` (not created by AI)
**Local project:** `synthwave_drive/` in the CGF workspace
**Repository:** https://github.com/Hazardiusoms/FP_CG.git

## Completed locally

- C++17/CMake executable with GLFW, GLEW, OpenGL, and copied runtime shaders.
- One window, one loop, centralized keyboard input, focus-loss clearing, and clamped frame timing.
- Custom shared math: vectors, transformations, view/projection, and normal matrices for future lit meshes.
- SceneState and RenderContext with safe defaults and a stable shared coordinate convention.
- Shader compile/link diagnostics with cleanup on failure and explicit resource ownership.
- UV-scrolling grid with speed changes, wrapping, derivative-based line smoothing, distance fade, and driving-region edges.
- Follow and presentation cameras, high-DPI framebuffer aspect handling, and zero-size guard.
- Scene reset and a stationary unlit reference block for camera/depth checks.
- Launch script, CPU tests, real OpenGL smoke checks, and screenshot evidence.

Source ownership and the finalized API are recorded directly in AGENTS.md. Extra helper files Input.hpp and ReferenceMarker.* belong to the framework; they do not implement Turar's car or Nursalim's lighting.

## Validation performed

Environment: macOS 26.6.1, Apple M3 Pro, Apple Clang 17.0.0, CMake 4.4.5, GLFW 3.5.1, GLEW 2.3.1, OpenGL 4.1 Metal - 90.5.

Commands used from the project root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j 4
ctest --test-dir build --output-on-failure
./build/core_tests
./build/synthwave_drive --smoke-test docs/evidence/smoke
```

Results:

- Debug build succeeded without compiler warnings.
- All 32 CPU checks passed (matrix conventions, perspective endpoints, normal correction, state bounds, time independence, key edges, reset, and camera behavior).
- GPU smoke checks passed: nonblank scene, moving grid, pixel-identical repetition after one whole UV period, differing cameras, one-shot camera key handling, acceleration, reset restoring the first frame, portrait resize, zero-size guard, and checked GL error state.
- Framebuffer captures were inspected in follow, presentation, and portrait views. PNG files in docs/evidence are conversions of actual OpenGL readbacks.
- Launch from an unrelated working directory succeeded with default executable-relative shader paths.
- Missing shader directory, malformed fragment shader, vertex/fragment varying mismatch, and unknown CLI option all failed with nonzero exit codes and appropriate diagnostics.

The input tests inject events into the same state layer used by GLFW callbacks. They do not establish a human keyboard walkthrough. No FPS performance benchmark is claimed. A clean source-copy Release build also passed, including CTest and a real rendered capture while launched from an unrelated working directory. It was built in a new temporary directory without existing build files. This is not a second-machine test.

## Not yet complete

- Publishing: no local Git repository, branch, commit, push, PR, or merge was created by AI.
- Turar's car module, Muslim's environment, and Nursalim's lighting are not available to integrate.
- A/D is routed but intentionally does not move the reference block.
- Sun, sky gradient, mountains, lit materials, bloom, and CRT are absent from this baseline.
- The UI currently uses the window title for controls/status; a separate text overlay is not implemented.
- Windows/Linux and a second team computer remain untested.
- Topic registration, complete R1–R8, and the exact exam/submission date still require confirmation.

## Why the key pieces work

- The road's UV v coordinate is world Z divided by the 2-unit grid period. Subtracting a growing offset makes a fixed grid line travel toward +Z, matching forward-driving scenery motion.
- Speed uses elapsed seconds; the UV offset is wrapped to [0,1) so long runs do not accumulate large shader coordinates.
- Camera view and perspective are separate matrices. Framebuffer dimensions, rather than logical window dimensions, determine the aspect ratio.
- Every GPU-owning object is created after Window and destroyed before it. Shader compilation failures release partial resources before returning an error.
- The reference block is deliberately simple and unlit; differently colored faces make depth readable without preempting the lighting assignment.

## Manual Git handoff — Almas performs every command

1. If you do not already have a checkout, clone it yourself:

```bash
git clone https://github.com/Hazardiusoms/FP_CG.git
cd FP_CG
```

2. In an existing checkout, inspect changes and save your own work before switching. With a clean working tree, update main and create your branch yourself:

```bash
git status
git switch main
git pull --ff-only origin main
git switch -c almas/core-road
```

If the feature branch already exists, switch to it instead of recreating it. Do not overwrite newer teammates' files when copying the handoff.

3. Copy the contents of the source handoff into this repository root. Do not copy build/, .git/, or the temporary smoke PPM files. Review the README replacement because the remote currently has the earlier project description. Keep existing unrelated files.

4. Build, run the tests, launch the application, and review the diff. Then stage the intended files explicitly:

```bash
git status
git diff
git add .gitignore CMakeLists.txt Run.command README.md AGENTS.md CREDITS.md
git add src/main.cpp src/Math.hpp src/Input.hpp src/SceneState.hpp src/RenderContext.hpp
git add src/Camera.hpp src/Camera.cpp src/Shader.hpp src/Shader.cpp src/Road.hpp src/Road.cpp
git add src/ReferenceMarker.hpp src/ReferenceMarker.cpp
git add shaders/road.vert shaders/road.frag shaders/reference.vert shaders/reference.frag
git add tests/core_tests.cpp docs/progress/almas.md docs/evidence/almas-week8-follow.png docs/evidence/almas-week8-presentation.png docs/evidence/almas-week8-portrait.png
git diff --cached --check
git diff --cached
git commit -m "Add Week 8 framework, scrolling road, and cameras"
git push -u origin almas/core-road
```

5. Open the pull request yourself: base main, compare almas/core-road. Ask a teammate to review and test. Merge manually after review so the others can branch from the common framework.

These commands are instructions for a human; AI did not execute them.

## AI use

AI drafted and implemented this initial contribution and performed the listed automated checks. Almas must review and learn the code before submission. Publication and any representation of personal understanding remain Almas's responsibility.
