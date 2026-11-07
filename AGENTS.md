# AGENTS.md — Synthwave Drive: Complete Team Handbook

**Repository:** https://github.com/Hazardiusoms/FP_CG.git
**Course:** Computer Graphics Fundamentals
**Final project:** Topic 13 — Synthwave Drive
**Current stage:** Week 8 · **Defense:** Week 10
**Team:** Almas Alkhan, Turar Nurbauli, Muslim Syzdykov, Nursalim Onalbayev

## How to use this single file

This file contains the full project specification, schedule, shared technical agreements, Git workflow, and detailed Week 8 instructions for all four members. No separate planning or instruction files are required to start.

Place this file at the root of the cloned **FP_CG** repository with the exact name **AGENTS.md**. Send this same file to every teammate. Each member reads the common sections and their own detailed section. An AI assistant must read the mandatory rules first, identify the member it is helping, inspect existing code, and work only within that member's assigned scope.

Almas’s initial framework is implemented locally. The baseline section below distinguishes the working code from remaining team tasks. Other planned features remain targets until implemented and verified. Existing repository code takes precedence over assumptions about what has already been built; coordinate changes to the agreed interfaces.

**Publication is human-only. Every member personally reviews, stages, commits, and pushes their own work. AI must never push or publish changes through GitHub APIs, browser actions, IDE synchronization, or any other route. Pull requests and merges are also human actions.**

## Verified requirements and current status

The supplied **Final project requirements (2).pdf** and **Final project topics (1).pdf** are the authority. This update supersedes the original Week 8 assumptions. The source is a framework prototype, not a finished final submission.

- **R1 — Modern pipeline:** implemented locally. C++17, OpenGL core, separate GLSL vertex/fragment files, VAO/VBO/EBO and indexed road rendering. No fixed-function rendering.
- **R2 — At least five distinct scene objects:** pending. The road and reference block are insufficient. Plan road, car, left mountain formation, right mountain formation and sun, with visibly distinct geometry and placement; document the final object inventory. Do not count HUD letters as scene objects.
- **R3 — Model/view/projection and hierarchy:** matrix helpers and MVP are implemented; the required parent–child hierarchy is pending Turar's car. Demonstrate body motion carrying wheels/headlights and independent wheel rotation.
- **R4 — Per-fragment lighting:** pending Nursalim. Implement Phong or Blinn–Phong ambient/diffuse/specular with **at least two lights**; at least one light must move or toggle. Emissive grid colors do not satisfy lighting.
- **R5 — Texture or equivalent procedural shading:** procedural UV grid is implemented. Its periodic masks, derivative filtering, road-edge colors and distance fade provide a shader-based surface pattern. No image texture or mipmap chain is claimed.
- **R6 — Keyboard and mouse camera plus scene interaction:** implemented locally. Orbit uses I/J/K/L and right-mouse drag; U/O and scrolling zoom. Speed, road pause and palette are three distinct scene interactions. A/D steering remains pending.
- **R7 — Performance:** visible live FPS, default 1280×720, repeatable benchmark implemented. Local prototype results are in docs/progress/almas.md. The required stable ≥30 FPS on the TA machine is still unverified; ≥60 FPS is the full-performance target. Re-measure after all modules and effects are integrated.
- **R8 — Reproducible setup:** local CMake build uses the assignment's GLFW/GLEW setup. No new runtime library was added. Verify a fresh checkout on the TA setup; any additional dependency must be included or built by the repository without requiring another TA installation.

Missing any R1–R8 requirement caps the technical-depth section at 60%; a build failure caps the overall result at 50%. Two functioning stretch features are optional for basic completion but needed for the corresponding eight stretch points. Bloom and CRT are not implemented and must not be advertised as working.

## Submission and defense checklist

- Submit three days before defense, by **23:59 Asia/Almaty**. The exact defense date is not supplied; do not invent a calendar deadline. Week 10 is the defense week, so freeze the integrated build before the three-day cutoff.
- A human adds the TA as a repository member and checks access. All four members make regular, meaningful personal commits. A human creates the **v1.0** tag on the submission date. AI never stages, commits, pushes, tags, merges or creates PRs.
- The report follows the instructor's Word template in academic English, includes equations, architecture, honest individual contributions, screenshots of every implemented feature, hardware and FPS measurements, limitations, and IEEE or APA references.
- Review and compare **at least 15 credible Q1–Q2 journal sources**. A bibliography alone is insufficient. Verify the relevant journal rankings and year/category before claiming this condition; the current draft's quartile verification remains open.
- Include an AI Use note in both README and report: tool, where assistance was used, and actual human revisions. Do not invent human review or edits.
- Prepare **8–12 slides**, **7–10 minutes** total; all four members participate and can be questioned on any code.
- Record a **3–6 minute, 1080p** real-time demo with FPS visible throughout. Voiceover or captions identify features and R1–R7 evidence. Show each claimed stretch feature enabled/disabled. The final demo and slides are still pending.
- Keep source, separate shaders, report, slides, video/link and credits consistent with the same reviewed build. Exclude build folders, caches and personal files. Confirm the actual Moodle fields and upload successfully before the cutoff.

## Implemented framework baseline — Almas, Week 8

The local framework now builds and renders an indexed scrolling grid with follow and interactive orbit cameras, keyboard/mouse controls, live FPS/help, road pause and palette selection. Mesh owns VAO/VBO/EBO; the road uses four vertices and six indices. It includes shared math, state, centralized input, shader loading, resize handling, reset, tests, and a temporary stationary reference block. **The block is not a car implementation.** Car steering/geometry, mountains, sky/sun, lighting, bloom, and CRT are still assigned to the other modules or later milestones. A/D input is collected but does not move the temporary block. B/T requests are collected but do not enable nonexistent effects.

### Finalized shared API

The initial math decision is a small tested `sw` namespace implementation in `src/Math.hpp`, with no GLM dependency. Matrices are column-major; vectors are columns; transforms are `projection * view * model * position`; OpenGL uploads use `GL_FALSE`. Available functions include translation, scaling, rotationX/Y/Z (radians), lookAt, perspective, and inverse-transpose normalMatrix. Singular normal transforms throw an error.

- `Input::set(Key, bool)` receives events centrally; `consume()` returns held axes and one-shot actions; `clear()` handles focus loss.
- `SceneState` holds elapsedTime, speed, carX, steeringTilt, camera, and reserved effect flags. Only a future Car module writes carX/steeringTilt during normal updates.
- `updateScene(SceneState&, const InputState&, float dt)` updates speed/time/camera selection and handles reset. The main loop resets all module state when reset is requested.
- `Camera::update(const SceneState&, int width, int height, RenderContext&) const` writes camera data and returns false for a zero-sized framebuffer.
- `RenderContext` contains view/projection, camera position, orthonormal forward/right/up vectors, vertical FOV in radians, and framebuffer pixel dimensions. Muslim can reconstruct a sky ray using these fields without introducing a second camera implementation.
- `Shader::initialize(vertexPath, fragmentPath)` compiles/links or throws a filename-rich exception. `use()` binds it; `set(name, Mat4/Mat3/Vec3/float)` uploads required active uniforms. Its destructor/shutdown releases the owned program. Do not copy Shader objects.
- `Road::initialize(shaderDirectory)`, `update(float dt, float speed)`, `render(const RenderContext&, bool alternatePalette=false) const`, `reset()`, and `shutdown()` are implemented. Initializers return void and throw on failure; this replaces the earlier conceptual bool lifecycle example for these implemented modules.
- `ReferenceMarker` uses initialize/render/shutdown and is removed when Turar's car is integrated.

GPU owners are non-copyable. They must be destroyed before the GLFW window/context. Do not declare global GPU-owning instances. The road reserves vertex attribute 2 for UV; lit geometry still uses attribute 0 for position and 1 for normal.

The road has an 8-unit driving region (X in [-4,4]) on a larger ground plane. Car facing is -Z. Scenery travels toward +Z. The grid uses `uv.y = z/2` and subtracts a growing wrapped offset; thus a particular grid line moves toward +Z. Speed is 0–40 scene units/second, initially 12; acceleration is 8 units/second². The frame step is clamped to 0.05 seconds after pauses, and minimized windows pause simulation. Normal frame rates use actual elapsed time.

### Connecting the other modules

Add the author's source files to the executable source list in CMakeLists.txt. Place shaders in shaders/; the build copies that directory beside the executable on every build. The runtime finds shaders relative to the executable, independent of the shell's current directory; `--assets DIR` explicitly overrides this location. Re-run the build after editing shaders.

Main.cpp contains marked integration points:

1. Construct/initialize the new module after Window, beside Road, so GPU destruction precedes context destruction.
2. In the update lambda, call Car after updateScene and before calculating the camera; call Environment with the same dt/speed. When reset is true, reset every module before returning.
3. In the draw lambda, render the background after clearing, restore depth testing and writes, then draw the road and mountains, then the lit car. Remove the reference block when the car is available.
4. Nursalim provides the Lighting service and material interface to Turar; neither should duplicate Shader or own the other's mesh buffers.

Car/Environment/Lighting classes do not exist yet. Their owners must agree exact new signatures with Almas using the data above; this baseline does not pretend unfinished modules are implemented. All CMake and main-loop integration changes are coordinated through Almas.

### Build and local verification

Verified locally on macOS 26.6.1, Apple M3 Pro, Apple Clang 17.0.0, CMake 4.4.5, GLFW 3.5.1, GLEW 2.3.1, and OpenGL 4.1 Metal. Minimum CMake is 3.19. Windows/Linux branches are prepared but untested. A second-computer build is still pending.

On a Mac with Homebrew and Xcode Command Line Tools available:

```bash
brew install cmake glfw glew
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
ctest --test-dir build --output-on-failure
./build/synthwave_drive
```

Alternatively, double-click Run.command on macOS after dependencies are installed. Run from the repository root; launch the built executable using an absolute path when starting from another working directory. For a multi-configuration generator, use its configuration subdirectory and `ctest -C Debug`.

Actual local results: 40 CPU checks passed; real OpenGL checks passed for visible rendering, grid motion, exact repetition after a whole UV period, camera-switch key edges, speed change, reset restoring the initial image, portrait resize, zero-framebuffer protection, and absence of GL errors at the checked stages. Missing shader paths, malformed shader source, incompatible shader stages, and unknown CLI options produce nonzero exits with useful diagnostics. Running from an unrelated working directory was verified. These are automated event/state checks and real GPU renders, not a claim of human keyboard testing or a second-machine test.

```bash
./build/synthwave_drive --smoke-test build/smoke
./build/synthwave_drive --capture build/follow.ppm
```

Captures are genuine framebuffer readbacks, in PPM format. PNG evidence is in docs/evidence/. The detailed progress record is docs/progress/almas.md; this AGENTS.md remains sufficient for teammates to start.

### Current handoff status

- Completed locally: Almas's initial framework, road, camera, shared utilities, input routing, build/run setup, and tests.
- Pending for this update: personal code review and human-only publication, teammate module integration, second-computer testing, completion of the mapped R1–R8 gaps and confirmation of the exact deadline.
- No commits, branches, pushes, PRs, or other remote writes were performed by AI. The earlier framework was published manually by Almas as commit 6e5582c; this update is local and uncommitted.
- Implementation and documentation were prepared with AI assistance. Almas must review and understand them before submitting or defending the work.


## Contents

- [Mandatory rules for AI assistants](#rules)
- [Project specification and implementation approach](#specification)
- [Team responsibilities and Weeks 8–10 plan](#plan)
- [Week 8 setup and human Git workflow](#getting-started)
- [Shared interfaces and integration contract](#interfaces)
- [Almas Alkhan — detailed Week 8 instructions](#almas)
- [Turar Nurbauli — detailed Week 8 instructions](#turar)
- [Muslim Syzdykov — detailed Week 8 instructions](#muslim)
- [Nursalim Onalbayev — detailed Week 8 instructions](#nursalim)

<a id="rules"></a>

## Mandatory rules for AI assistants

### Scope and first steps
- These instructions apply to the entire FP_CG repository.
- Project: Topic 13, Synthwave Drive; C++17 and OpenGL; defense in Week 10.
- Repository: https://github.com/Hazardiusoms/FP_CG.git (default branch: main).
- Read the specification, plan, setup, and shared-interface sections of this AGENTS.md before editing code.
- Read the relevant detailed member section for the assigned implementation task. If the member is unknown, ask which member you are helping before changing source files.
- Inspect existing code and Git status first. The repository was documentation-only when this plan was written; do not assume planned code already exists.

### Human-only publication — mandatory
- NEVER run git push, including ordinary, force, tag, mirror, or deletion pushes.
- NEVER publish through GitHub APIs, connectors, browser automation, IDE sync buttons, scheduled jobs, or scripts as a substitute for git push.
- NEVER create or update remote branches, files, commits, pull requests, releases, issues, or comments, and never merge remotely.
- Do not create automation that publishes changes later.
- Local editing, local builds/tests, and read-only inspection of GitHub are allowed within the assigned task.
- Leave staging, commits, branch synchronization, pushing, pull requests, and merges to the human member under this team's workflow.
- Supply exact suggested commands in the handoff, but DO NOT execute commands that change Git state or publish work.
- A request such as “finish my module” or “prepare it for GitHub” does not authorize publication.
- If asked to push under these rules, explain that the member must run the push personally and provide the command.
- Never request, print, store, or embed credentials in files or commands.

### Implementation boundaries
- Work on the assigned member's module; preserve teammates' work and uncommitted edits.
- Do not switch branches, reset, clean, discard, stash, or rewrite history automatically.
- Do not silently change shared interfaces. Propose the change to the human and document the integration impact first.
- Keep one application window, one main loop, one input system, and one camera system.
- Use Y up, X lateral, and the road along Z; see the shared-interface section below for signs and units.
- Implement Week 8 fundamentals first. Bloom, CRT, instancing, and advanced effects belong after the core is stable.
- Reuse agreed shared utilities rather than introducing duplicate shader loaders, math libraries, or private application loops.
- Preserve C++17/OpenGL 3.3-compatible features; account for the macOS core-context requirements in the framework guide.
- Use portable resource paths and explicit OpenGL state ownership. Release GPU resources before destroying the context.
- Never treat Assignment 3 or the GLUT practice as the final-project specification.

### Verification and handoff
- Build and run relevant local checks where available. Do not fabricate tests, screenshots, FPS, or completed features.
- If a required shared dependency is missing, identify it and continue independent work within the member's scope.
- End with changed files, implemented behavior, checks actually run, remaining limitations, and human-run Git commands.
- Record substantive AI assistance in the member's progress document. Do not misrepresent AI-assisted work as entirely unaided.
- Keep documentation and comments in English; explain implementation decisions so the student can defend their own work.

### Member sections
- Almas Alkhan: framework, road, camera, and integration — see the Almas section below.
- Turar Nurbauli: car, steering, and hierarchy — see the Turar section below.
- Muslim Syzdykov: mountains, sky, and sun — see the Muslim section below.
- Nursalim Onalbayev: lighting and validation — see the Nursalim section below.

---

<a id="specification"></a>

## Project specification and implementation approach

An interactive 3D night drive through a retro-futuristic landscape, built with C++ and OpenGL.

A neon car faces a striped sun on the horizon. A glowing grid scrolls beneath it, low-poly mountains frame the road, and the player controls steering, speed, and the camera.

**Computer Graphics Fundamentals · Final Project · Topic 13**

> **Project status: Week 8 framework.** The baseline section at the top records what is implemented and tested. The feature descriptions below remain the full-project target, not a claim that every module is complete.

[Development plan and team responsibilities](#plan)

### Project goals

- Build a coherent, interactive scene that satisfies the required features of Topic 13.
- Demonstrate transformations, camera projection, procedural geometry, shader-based visuals, and lighting.
- Keep the application modular so that four team members can develop and integrate their contributions.
- Deliver a reproducible build, a documented implementation, and a clear live demonstration.

### Core features and implementation

#### Scrolling grid ground

The ground will create the impression of an endless road moving toward the camera.

We will draw a ground plane with repeating UV coordinates. A fragment shader will generate the grid lines, and a speed-dependent UV offset will animate their movement. The offset will advance using elapsed frame time (`deltaTime`) so that travel speed remains consistent at different frame rates. Repeating coordinates and a wrapped offset will keep the grid continuous during long runs.

#### Low-poly mountain landscape

Mountains will form a silhouette on both sides of the road without blocking the driving area.

We will construct simple triangulated meshes with angular surfaces and a restrained purple and pink palette. Mountain sections will be arranged along the road. If the sections move with the scene, sections behind the camera will be repositioned ahead of it to maintain the illusion of an endless landscape.

#### Striped sun and gradient sky

The horizon will contain a large striped sun against a smooth night-sky gradient.

A background shader will blend sky colors according to vertical position. The sun will use a circular mask, a warm color gradient, and horizontal stripe masks. Keeping the background independent of camera translation will make it appear distant, while its placement will remain consistent with the scene's viewing direction.

#### Steerable neon car

The player will move a 3D car left and right within the road boundaries.

The car will be assembled from simple meshes for the body, wheels, lights, and neon details. Keyboard input will update its lateral position, clamped to the road width. A small smoothed body tilt will make steering visible. Neon details will use emissive colors, while body surfaces will respond to lighting.

### Supporting features

#### Speed control

The player will be able to accelerate and slow down. A shared speed value will drive the road offset, wheel rotation, and any moving environment sections. Limits will prevent negative or excessively high speeds.

#### Camera modes

The framework includes a camera behind the car and an orbit presentation view controlled by keyboard and mouse. View and projection matrices will be shared across the 3D modules. The projection aspect ratio will update when the window is resized.

#### Hierarchical car model

The car body will act as the parent of the wheels, headlights, and decorative parts. Each child will have a local transformation combined with the parent's model matrix. This allows the entire car to move together while the wheels rotate independently.

#### Lighting and materials

The car will use per-fragment ambient, diffuse, and specular lighting from at least two lights, with at least one movable or toggleable light. Surface normals and light directions will determine shading, while material parameters will control body color and highlights. Emissive parts will remain visually distinct from lit surfaces.

#### Controls and reset

An on-screen control hint and a reset action will make the demonstration easy to operate. Reset will restore the initial car position, speed, and camera state.

### Stretch goals

These effects will be attempted only after the core scene is complete and stable.

#### Bloom

The scene will render to an off-screen framebuffer. Bright emissive regions will be extracted, blurred in horizontal and vertical passes, and combined with the original image to create a glow around neon elements. Bloom will have an on/off toggle for visual and performance comparisons.

#### CRT scanlines

A final screen-space shader will apply subtle horizontal intensity modulation to imitate a CRT display. The effect will be switchable and kept light enough to preserve readability.

Instanced roadside palms are a reserve stretch goal if the selected effects prove impractical within the schedule. Only completed and verified features will be presented as implemented.

### Technology

Planned stack:

- **C++17** for application logic and scene modules.
- **OpenGL 3.3 Core** for rendering.
- **GLSL** for geometry shading, the sky, the sun, the grid, and post-processing.
- **GLFW** for the window, OpenGL context, and keyboard input.
- **GLEW** for OpenGL function loading.
- **CMake** for building the project.

Additional dependencies will be documented if introduced. The final repository will include tested build and run instructions, required dependency versions, and supported platforms once the application exists.

### Architecture

The application will use one window, one main loop, and shared scene state.

- `main.cpp` — initialization, input dispatch, frame timing, and the render loop.
- `SceneState` — speed, car position, camera selection, and effect toggles.
- `Camera` — view and projection matrices and camera mode transitions.
- `Road` — ground geometry and animated grid coordinates.
- `Car` — model parts, hierarchical transforms, steering, and wheel animation.
- `Environment` — mountain sections, sky, and striped sun.
- `Lighting` — light parameters and shared material settings.
- `PostProcessing` — framebuffer resources, bloom, and CRT passes.

#### Coordinate and integration conventions

- Y points upward, X is the lateral axis, and the road extends along Z.
- The car remains near a fixed longitudinal position; scrolling geometry and UVs create forward motion.
- The main loop provides `deltaTime` and input to the modules.
- All 3D modules use the same camera and projection.
- Modules share agreed interfaces and do not create their own window or event loop.
- Asset paths must work from a fresh checkout without personal absolute paths.

#### Frame sequence

1. Read input and calculate elapsed frame time.
2. Update speed, steering, wheel rotation, the camera, and environment movement.
3. Render the sky and sun, followed by the road, mountains, and car with appropriate depth handling.
4. Apply enabled post-processing effects.
5. Draw control hints and present the frame.

#### Planned repository structure

```text
synthwave_drive/
├── README.md
├── AGENTS.md
├── CMakeLists.txt
├── CREDITS.md
├── src/
│   ├── main.cpp
│   ├── SceneState.*
│   ├── Camera.*
│   ├── Road.*
│   ├── Car.*
│   ├── Environment.*
│   ├── Lighting.*
│   └── PostProcessing.*
├── shaders/
├── assets/
└── docs/
    ├── report/
    ├── slides/
    └── demo/
```

This AGENTS.md contains all instructions needed for the handoff. Separate README/specification/plan files may be kept for presentation, but are optional companions. The Almas framework source, road/reference shaders, build setup, and tests now exist. The remaining team modules are still implementation targets; see the implemented baseline above.

### Planned controls

- **A / D** or **Left / Right** — steer the car.
- **W / S** — increase or decrease speed.
- **C** — change camera mode.
- **B** — toggle bloom, if implemented.
- **T** — toggle CRT scanlines, if implemented.
- **R** — reset the scene.
- **Esc** — exit.

The final control mapping will be verified against the running application.

### Quality and validation

Before submission, we will verify that:

- All four Topic 13 core features work together in one scene.
- Grid repetition and environment recycling have no visible discontinuities.
- Steering stays inside the road and child objects follow the car correctly.
- Movement is consistent across frame rates.
- Camera changes and window resizing preserve correct rendering.
- Disabling post-processing leaves a fully working core scene.
- Frame-rate measurements include the test computer, resolution, and effect settings.
- A fresh checkout builds and runs on a second team member's computer.

The full R1–R8 criteria are available and mapped in the verified requirements section above. Complete every pending item before claiming final compliance.

### Team

- **Almas Alkhan** — application framework, scrolling road, camera, and integration.
- **Turar Nurbauli** — car geometry, steering, hierarchical transforms, and wheel animation.
- **Muslim Syzdykov** — mountains, sky, striped sun, and environment composition.
- **Nursalim Onalbayev** — lighting, post-processing, and coordination of validation results.

Each member will contribute implementation, documentation, testing, and an explanation of their own code. The schedule, milestones, and individual deliverables are defined in the team-plan section of this file.

### Final deliverables

- Source repository with tested setup instructions.
- PDF report describing the implementation, individual contributions, algorithms, screenshots, validation, and limitations.
- Presentation slides.
- Demonstration video showing the implemented features and controls.

The topic document requires submission **three days before the final exam**. The exact date and Moodle upload requirements must be confirmed against the course schedule.

### Credits and AI use

Original geometry and procedural visuals are preferred. Any external assets, libraries, or adapted code will be credited in `CREDITS.md` and the report, including applicable licenses.

OpenAI Codex assisted with the framework implementation, tests, report and development documentation; the README AI Use note records the current scope. Human revisions must be recorded honestly after review. Every team member remains responsible for checking, understanding, and explaining the code they submit.

---

<a id="plan"></a>

## Team responsibilities and Weeks 8–10 plan

**Course:** Computer Graphics Fundamentals
**Project:** Topic 13 — Synthwave Drive
**Current week:** 8
**Defense:** Week 10
**Team:** Almas Alkhan, Turar Nurbauli, Muslim Syzdykov, Nursalim Onalbayev

This document defines responsibilities, integration milestones, and the delivery schedule. The feature specification and technical approach are described in [PROJECT_SPEC.md](#specification).

Detailed individual Week 8 instructions and the human-only publication workflow are in [the setup section](#getting-started).

### Delivery strategy

Complete the required scene first, integrate early, and add optional effects only after the core works. The target is a stable application during Week 9, with time reserved for documentation, submission, and defense preparation.

**Deadline dependency:** the topic document requires submission three days before the final exam. The exact exam date must be confirmed immediately. If that deadline falls in Week 9, all final packaging and submission tasks move into Week 9. Week 10 must not be treated as guaranteed development time.

### Team responsibilities

#### Almas Alkhan — Framework, road, camera, and integration

**Implementation**

- Set up C++17, OpenGL, GLFW, GLEW, and CMake.
- Implement initialization, shader loading, frame timing, and centralized input.
- Define shared state and module interfaces with the team.
- Build the scrolling ground grid and connect its speed to scene state.
- Implement the follow camera and second presentation view.
- Integrate modules together with their authors.

**Week 8 output:** a runnable base scene with the moving grid and camera that accepts the other modules.

**Week 9 output:** an integrated application with consistent input, movement, camera behavior, and build instructions.

**Defense focus:** main loop, `deltaTime`, view/projection matrices, UV scrolling, and module integration.

#### Turar Nurbauli — Car, steering, and hierarchy

**Implementation**

- Create the low-poly car body, wheels, headlights, and neon details.
- Implement parent-child transformations for the car parts.
- Add lateral steering, road boundary limits, and a smooth visual tilt.
- Connect wheel rotation to travel speed.
- Coordinate body materials and emissive elements with Nursalim.

**Week 8 output:** a visible car that moves left and right within the road boundaries.

**Week 9 output:** the integrated car with working hierarchy, wheel animation, steering tilt, materials, and neon details.

**Defense focus:** model matrices, local and world coordinates, hierarchical transformations, and steering logic.

#### Muslim Syzdykov — Mountains, sun, and sky

**Implementation**

- Construct low-poly mountain meshes and place them on both sides of the road.
- Implement the gradient sky shader.
- Implement the sun's circular mask, horizontal stripes, and color gradient.
- Establish the horizon composition and scene palette with the team.
- Recycle mountain sections if the environment moves with the road.

**Week 8 output:** mountains, gradient sky, and striped sun rendered in the shared application framework.

**Week 9 output:** the complete environment integrated with the road and car, without obvious gaps or depth-order problems during camera changes.

**Defense focus:** mountain geometry, shader masks, color gradients, and environment placement.

#### Nursalim Onalbayev — Lighting, effects, and validation coordination

**Implementation**

- Define shared light and material parameters with Turar.
- Implement lighting for the car and relevant scene surfaces.
- Keep emissive elements distinct from lit materials.
- Once the core is stable, implement framebuffer rendering, bloom, and CRT scanlines.
- Check image quality at different window sizes and with effects disabled.
- Collect performance measurements and validation results from all members.

**Week 8 output:** working lighting on a temporary object and agreed light/material interfaces.

**Week 9 output:** integrated lighting, followed by optional effects if time allows, with recorded validation results.

**Defense focus:** ambient/diffuse/specular lighting, emissive color, framebuffers, rendering passes, and performance tradeoffs.

### Shared responsibilities

- Each member writes and checks their own module.
- Each member contributes report text, selected code excerpts, screenshots, and presentation slides for their work.
- Each member records external sources and AI assistance used in their contribution.
- Almas assembles the report and slides from team contributions; everyone reviews the result.
- Nursalim maintains the combined validation checklist; the relevant module author fixes each reported issue.
- The team agrees on a demonstration script and rehearses together before recording.

### Week 8 — Foundation and first integration

#### Initial coordination

- [ ] Confirm Topic 13 registration with the instructor.
- [ ] Read the supplied final requirements and use the verified R1–R8 mapping above.
- [ ] Confirm the exam date and calculate the submission deadline.
- [ ] Create the shared repository and verify the development setup on team computers.
- [ ] Agree on coordinate axes, scale, road width, car size, module interfaces, and visual references.

#### Parallel implementation

- [ ] Almas: application framework, camera, and scrolling grid.
- [ ] Turar: initial car geometry and lateral movement.
- [ ] Muslim: mountains, sky, and striped sun.
- [ ] Nursalim: basic lighting and material setup on a temporary object.

#### End-of-week milestone

Combine the first versions into one application window. Simple models and unfinished styling are acceptable, but the project must build and run as a shared scene.

**Acceptance check:** another team member obtains a fresh copy and launches the scene using the written instructions without assistance from the framework author.

### Week 9 — Complete features and prepare delivery

#### First half: finish the core

- [ ] Complete the scrolling grid, mountains, striped sun/gradient sky, and steerable neon car.
- [ ] Verify the applicable R1–R8 requirements against the implementation.
- [ ] Finish steering, car hierarchy, camera modes, and lighting.
- [ ] Fix integration, depth, resource-path, and environment-recycling issues.

#### Midweek checkpoint

- [ ] Demonstrate every required feature using a checklist.
- [ ] Preserve a stable version that works without post-processing.
- [ ] If the core is incomplete, suspend optional effects until it is finished.

#### Second half: polish and documentation

- [ ] Add bloom and CRT scanlines only if the stable core is complete.
- [ ] Measure performance with effects enabled and disabled.
- [ ] Capture screenshots of the actual application.
- [ ] Complete individual report sections and asset/source credits.
- [ ] Assemble slides and record a draft demonstration.
- [ ] Rehearse the defense with all four members.
- [ ] Finalize and submit during this week if required by the exam date.

**Acceptance check:** the application is stable, the core is complete, setup instructions are reproducible, and report/slides/video drafts exist. Each stretch feature is explicitly marked as completed or excluded.

### Week 10 — Final checks and defense

Complete any remaining delivery tasks before the confirmed submission deadline. If submission occurred in Week 9, use this week for verification, rehearsal, and backups.

- [ ] Freeze features; make only necessary bug fixes.
- [ ] Build from a fresh checkout on a second computer.
- [ ] Review the requirement checklist and all submission materials.
- [ ] Export and inspect the PDF report and presentation.
- [ ] Record and inspect the final demonstration video.
- [ ] Upload the required materials before the deadline and verify Moodle's confirmation.
- [ ] Rehearse transitions between presenters and individual code questions.
- [ ] Keep local backups of the source, working application, and video.

**Defense-day check:** verify the presentation computer, libraries, asset paths, controls, and offline playback before presenting.

### Integration workflow

#### Branches

- `almas/core-road`
- `turar/car`
- `muslim/environment`
- `nursalim/lighting-effects`

Keep the main branch runnable. Merge small working changes regularly rather than postponing integration until Week 10.

#### Interface agreement

- Use Y-up coordinates, X for steering, and Z for the road direction.
- Share one `SceneState`, one camera system, and one main loop.
- Pass frame timing and state into modules instead of duplicating input handling.
- Agree on interface changes before editing another member's integration points.
- Turar supplies car/headlight data; Almas supplies camera and speed; Muslim supplies environment geometry/background; Nursalim supplies lighting and post-processing.
- Use portable asset paths and document all dependencies.

#### Review and communication

Before merging, the author checks the build and a second member checks the visible behavior. At the end of each working day, each member posts a short update: completed work, blockers, and the next step.

### Release checklist

- [ ] Topic registration, R1–R8, and the exact deadline are confirmed.
- [ ] All four Topic 13 core features are implemented.
- [ ] Grid repetition and mountain recycling are visually continuous.
- [ ] The car stays inside the road and its parts follow the parent transformation.
- [ ] Movement uses elapsed time rather than frame count.
- [ ] Lighting and materials produce the intended appearance.
- [ ] Camera changes and window resizing work correctly.
- [ ] Effects can be disabled without breaking the core scene.
- [ ] Performance results include hardware, resolution, and effect settings.
- [ ] A fresh checkout builds and runs on a second computer.
- [ ] The README describes actual completed features and includes tested commands.
- [ ] Each member can explain their own implementation.
- [ ] The report and slides list all four members and their confirmed group numbers.
- [ ] External assets, licenses, references, and AI assistance are documented.
- [ ] The PDF report, slides, video, and repository link have been checked.
- [ ] Moodle confirms submission before the deadline.

Assignment 3 features, such as SMF loading, are not automatically final-project requirements. The final project must be checked against its own specification.

### Scope control

If progress falls behind, reduce model complexity and decorative additions first. Preserve all required Topic 13 features.

Do not claim incomplete bloom or CRT effects as implemented. A simple, stable scene with clear interaction takes priority over unfinished visual effects. Keep the last working version available before integrating major changes.

### Defense sequence

1. **Almas:** project objective, launch, scrolling road, and camera.
2. **Turar:** car construction, steering, wheel animation, and hierarchy.
3. **Muslim:** mountains, sky, sun, and shader techniques.
4. **Nursalim:** lighting, completed post-processing effects, and validation results.
5. **Team:** final accelerated drive and questions.

Allocate speaking time after the instructor confirms the presentation duration. Each member should be ready to explain both their module and how it connects to the shared application.

### Immediate next steps

1. Confirm topic registration, read the supplied R1–R8 mapping, and record the exact deadline.
2. Create the shared repository and application skeleton.
3. Agree on module interfaces and coordinate conventions.
4. Begin the four assigned modules.
5. Integrate the first shared scene before the end of Week 8.

---

<a id="getting-started"></a>

## Week 8 setup and human Git workflow

### What you are building this week

A first shared version of Synthwave Drive: one window containing a scrolling grid, a steerable car, mountains, a striped sun, a gradient sky, and basic lighting. Simple geometry is sufficient. The goal is to make the modules work together before visual polish.

**Repository:** https://github.com/Hazardiusoms/FP_CG
**Clone URL:** https://github.com/Hazardiusoms/FP_CG.git
**Default branch:** `main`
**Defense:** Week 10

At the initial documentation inspection on 31 October 2026, the remote root contained an empty README.md. At the implementation inspection, README.md had been filled in, but no source code was present. Almas’s local framework is now available in this handoff; the remote may differ until he pushes it personally. Check the current repository and the baseline section above before starting.

### Read in this order

1. Mandatory AI rules at the beginning of this file.
2. Project specification and team plan.
3. This setup section and the shared-interface agreement.
4. Your own detailed member section.

Keep the full AGENTS.md at the repository root. Do not replace it with one member's section or omit the common rules.

### The most important rule: publish personally

AI may help explain, edit, debug, and test local code. It must not push or otherwise publish anything to GitHub. Do not use an AI agent to click Sync, create remote files, open PRs, or run a publishing script.

Under this workflow, the member personally reviews the diff, stages files, creates commits, pushes their branch, opens a pull request, and performs any merge. Authenticate using your own GitHub account through your local Git client. Never paste a token or password into an AI chat.

Do not share accounts. If you cannot push because access is missing, ask the repository owner for collaborator access; do not give your credentials to another member or an AI tool.

### First repository setup — Almas

Copy this single **AGENTS.md** into the root of the cloned FP_CG repository. Do not put it inside an extra nested synthwave_drive/ directory. A short README can be maintained separately for visitors; it is not required to understand these instructions.

Almas should manually publish this handbook and then the minimal framework. Other members can read this file and start their preparation immediately, but should base their implementation branches on main after the framework is merged.

If code is not yet available, follow the independent preparation steps in your member section. Do not create four unrelated application frameworks.

### Clone and inspect — commands for humans only

Run these yourself in a terminal, from the directory where you want the project stored:

```bash
git clone https://github.com/Hazardiusoms/FP_CG.git
cd FP_CG
git remote -v
git status
git branch --show-current
```

If you already have a checkout, open it instead of cloning over it. Confirm that origin points to Hazardiusoms/FP_CG and inspect any uncommitted changes before switching branches.

Use the branch name from your individual guide. Once the shared framework is on main and your working tree is clean:

```bash
git switch main
git pull --ff-only origin main
```

Then run your guide's `git switch -c ...` command. If that branch already exists locally, use `git switch ...` without `-c`. If only a remote branch exists, fetch it and create a tracking branch rather than creating an unrelated duplicate.

### Local build contract

Before the framework exists, there is nothing to build. Almas must supply and test CMake setup and exact dependency installation instructions for the team's operating systems.

The intended standard commands are:

```bash
cmake -S . -B build
cmake --build build --config Debug
```

The executable target will be named `synthwave_drive`. Its actual output location depends on the CMake generator; on a typical single-configuration build it is `build/synthwave_drive`, while some Windows generators use `build/Debug/synthwave_drive.exe`. Use the tested command recorded by Almas rather than assuming the same path on every machine.

If configuration fails, collect the first error, your OS, compiler, and dependency setup. Do not “fix” it by copying libraries from another computer or changing everyone's dependency versions independently.

### Daily routine

1. Check the latest integration notes and your module's dependency status.
2. Work on one small behavior from your guide.
3. Build locally and inspect the result in the shared application when available.
4. Record completed work, checks, blockers, and AI assistance in your progress file.
5. Review your changes personally and publish a small commit when it is useful to the team.
6. Send teammates a short update with the branch, changed interface if any, and what is ready to integrate.

A useful update is: “Car steering works on turar/car; tested both boundaries and reset. Waiting for lit shader. No shared interface changes.”

### How to commit and push manually

First inspect your changes:

```bash
git status
git diff
```

Stage only the files you intend to share. Each individual guide includes an example explicit file list. Do not blindly stage generated build files, private files, or unrelated work.

Then inspect the staged result:

```bash
git diff --cached --check
git diff --cached
```

Create the commit yourself using a short description of the actual change, and run the exact push command from your guide. These are human actions; an AI assistant must leave them for you.

After pushing, check that GitHub displays the expected branch and files. Open a pull request in the browser with **base: main** and **compare: your branch**. Include:

- What now works.
- How you ran it and what you checked.
- Screenshots if they clarify the result.
- Any dependency or interface change.
- Known limitations.

Ask a teammate to review it. The human reviewer or maintainer merges it after checking. Do not push directly to main during normal module development, and do not force-push.

### Receiving changes from main

When main changes, finish or commit your local edits first. On your own feature branch, run these commands personally:

```bash
git fetch origin
git merge origin/main
```

If Git reports a conflict, stop and read the conflicting files with the relevant author. AI may explain or edit a local conflict resolution, but you must review it, stage it, and complete the merge yourself. Do not use reset/clean or choose “ours/theirs” for entire files without understanding the lost changes. Rebuild after the merge.

### Minimum evidence from every member

Create `docs/progress/<member>.md` using the filename in your guide. Include:

- Date and branch.
- Implemented behavior and files.
- Commands actually used to build/run.
- Checks actually performed and their results.
- Remaining issues or dependencies.
- Sources/assets and licenses, if used.
- AI assistance used and what you reviewed yourself.

Capture at least one real application screenshot once your module is integrated. Put it in `docs/evidence/` with a descriptive member-prefixed name. Do not generate a fake result image or claim an unrun test passed.

### Week 8 integration meeting

Almas brings the shared framework and road; Turar brings the car; Muslim brings the environment; Nursalim brings the lit shader and parameters. Connect them in the same application and verify:

- One window opens with no shader compilation error.
- The grid scrolls; the car can steer and remains within road boundaries.
- Mountains appear on both sides, with a gradient sky and striped sun behind them.
- At least the car body or a temporary object responds to the agreed light.
- Resizing and reset work without broken geometry or state.
- A second member can build and run a fresh copy.

Bloom and CRT are not blockers for this meeting. The framework, visible core objects, shared interfaces, and reproducible setup are.

### Requirements and deadline

Use the complete R1–R8 mapping from the supplied requirements PDF above; resolve only genuine ambiguities with the instructor. Confirm the exam date and the required submission three days before it. If this means submission in Week 9, move the release work earlier. Do not wait until the defense to discover a missing required item.

---

<a id="interfaces"></a>

## Shared interfaces and integration contract

### Status

The initial contract below is supplemented by the finalized framework API at the top of this file. Shared framework headers now exist; lifecycle shapes for the remaining team modules are still proposals to confirm with their authors. If existing repository code differs, discuss and update the contract; do not silently replace working interfaces.

Once the baseline is accepted, changes to shared headers require coordination with the affected module owners. Interface proposals can be written locally by AI, but publication and merging remain human actions.

### Runtime and math

- C++17; rendering uses OpenGL 3.3 Core features and GLSL 330.
- GLFW owns the window/context and input; GLEW loads OpenGL functions.
- The framework must account for macOS core contexts: request an appropriate supported core context (typically 4.1, forward-compatible), while keeping shader/code features within the agreed baseline. Other platforms should request the agreed 3.3 core context.
- Use one shared math implementation for vectors and matrices. **Initial decision: the tested custom sw math helpers in src/Math.hpp.** Do not introduce a separate math library in each module.
- `src/Math.hpp` exposes the agreed `Vec3` and `Mat4` types; these names below are contract names, not a complete header.
- Document matrix layout, multiplication order, and shader upload convention. Expected transform order is projection × view × model × position.
- Release all GPU objects while the context still exists.

### Coordinates and starting layout

Proposed initial values below provide a common scale and can be tuned together after integration:

- Right-handed coordinates: +Y up, +X right; the car faces -Z.
- Car body center approximately `(0, 0.5, 0)`; ground at Y=0.
- Road extends along Z with lateral limits approximately X=-4 to X=4.
- The follow camera starts near `(0, 3, 7)` and looks toward `(0, 0.6, -8)`.
- A car approximately 1.6 units wide and 3 units long fits this scale.
- Roadside geometry starts beyond the road limits with a visible safety gap.
- World scenery moving toward the viewer advances along +Z. Grid UV direction must be checked visually against that motion.
- Keep every vertex of the car inside the road, not just its center. Derive the steering limit from road half-width minus the car's maximum lateral extent and a margin; allow for steering tilt.
- `deltaTime` is measured in seconds; speed is scene units per second; public rotation parameters use radians.

These values are a starting convention, not a grading requirement.

### Shared files and ownership

- Almas owns `CMakeLists.txt`, `.gitignore`, `src/main.cpp`, `src/Math.hpp`, `src/SceneState.hpp`, `src/RenderContext.hpp`, `src/Shader.hpp`, `src/Shader.cpp`, and camera/road files.
- Turar owns `src/Car.hpp`, `src/Car.cpp`, and car-specific geometry/data.
- Muslim owns `src/Environment.hpp`, `src/Environment.cpp`, `shaders/background.vert`, and `shaders/background.frag`.
- Nursalim owns `src/Lighting.hpp`, `src/Lighting.cpp`, `shaders/lit.vert`, and `shaders/lit.frag`.
- Each person owns their progress note and member-prefixed evidence files.
- `AGENTS.md`, `README.md`, and credits are shared documents: use small edits and avoid replacing another member's content.
- Almas makes source-list and main-loop integration changes after receiving the exact requests from each module author.

### Shared state

The first headers should define these concepts with initialized defaults:

- `InputState`: steering direction in [-1, 1], speed adjustment direction in [-1, 1], and one-shot reset/camera/effect actions.
- `SceneState`: elapsed time, speed, car X position, steering tilt, selected camera, and effect toggles.
- `RenderContext`: view matrix, projection matrix, world-space camera position, and actual framebuffer width/height.

Use framebuffer pixel dimensions for the OpenGL viewport, which may differ from logical window dimensions on high-DPI displays. Skip projection/render work when the framebuffer height or width is zero.

#### Single-writer rules

- Main handles elapsed time, speed adjustment, and resets.
- Turar's car update handles car X position, steering tilt, and wheel angle.
- Camera reads the updated car state and handles view selection.
- Road owns its wrapped UV offset and reads speed.
- Environment owns section positions and reads speed.
- Lighting owns light/material parameters and reads the camera position for specular calculations.
- Input keys are read centrally, never inside each module.

No module should independently increment the same shared position or speed.

### Module lifecycle

Each module needs an initialization phase, update if required, rendering, reset if required, and cleanup. Agree on exact signatures in headers before merging implementations. Suggested shapes are:

```cpp
// Conceptual contract: types and ownership must be defined in the shared headers.
bool initialize(/* agreed shared services */);
void update(float deltaTime, /* input/state as required */);
void render(const RenderContext& context, /* read-only state as required */);
void reset();
void shutdown();
```

Use const references for state that a module only reads. Initialization failure must produce a useful error and stop startup cleanly. Avoid copying shader ownership or deleting GPU handles from multiple owners.

- Road update receives speed and time; render receives camera matrices.
- Car update receives input, time, and mutable car state; render receives camera matrices, read-only car state, and the agreed lighting service.
- Environment separates background rendering from mountain rendering so the background can use its own depth state.
- Lighting initializes the common lit shader and exposes a way to bind camera, model, light, and material data for a draw.
- No module calls `glfwPollEvents`, swaps buffers, or owns the application loop.

### Geometry and lit-shader contract

For the first lit meshes, agree on this vertex layout:

- Attribute location 0: position, three floats, object space.
- Attribute location 1: normal, three floats, object space.

A separate road shader may additionally use UVs at a documented location. Keep that choice explicit in the road's VAO and shader.

Common lit uniforms should represent:

- Model, view, and projection matrices.
- Inverse-transpose model 3×3 normal matrix, calculated consistently with the chosen math utilities.
- Camera position and light position in world space.
- Light ambient/diffuse/specular colors.
- Material base color, ambient/diffuse/specular strengths, shininess, and emissive color.

Suggested names: `uModel`, `uView`, `uProjection`, `uNormalMatrix`, `uCameraPosition`, `uLightPosition`, `uLightAmbient`, `uLightDiffuse`, `uLightSpecular`, `uBaseColor`, `uKa`, `uKd`, `uKs`, `uShininess`, `uEmissive`.

Use the same names and spaces on the C++ and GLSL sides. Nursalim documents the final names and Turar consumes them through the lighting service. Do not submit two competing car shaders. Week 8 requires a simple working light, not the unrelated Assignment 3 light controls or SMF loader.

### Render state and draw order

1. Main clears color and depth and selects the current framebuffer.
2. Environment draws the background without modifying scene depth, then restores the agreed depth state.
3. Main enables depth testing and depth writes for opaque 3D objects.
4. Road, mountains, and car draw with the shared view/projection.
5. Any overlay is drawn with explicit state restoration.
6. Main presents the frame.

Background and overlay code must not leave depth writes disabled for the next frame. Record any changed blend/cull/depth state. Post-processing will be added later without changing the modules into separate application loops.

### Files required in the first framework handoff

Almas should supply:

- Tested CMake setup and dependency instructions.
- Shared types and finalized lifecycle signatures.
- Shader compile/link logging and resource path handling.
- Window, time, input, camera, and resize handling.
- A small visible reference object or road so newcomers can confirm that rendering works.
- Instructions for registering each module's files in the build.

The other members provide their source/shader paths, initialization arguments, update/render calls, and screenshot/check results. Almas should not have to guess how their modules are used.

---

<a id="almas"></a>

## Almas Alkhan — detailed Week 8 instructions

**Role:** application framework, scrolling road, camera, and integration
**Branch:** `almas/core-road`
**Progress note:** `docs/progress/almas.md`

Read [START_HERE.md](#getting-started) and [INTERFACES.md](#interfaces) first. You establish the common foundation that the other three modules depend on. Publish the smallest usable framework early; do not wait for a polished road.

### Required outcome

By the end of Week 8, another member can build and run a shared scene containing the first versions of the road, car, environment, and lighting. Your own contribution is the working application loop, shared interfaces, camera, and animated grid.

### Files you own

- `CMakeLists.txt`, `.gitignore`, and documented build setup.
- `src/main.cpp`.
- `src/Math.hpp`, `src/SceneState.hpp`, and `src/RenderContext.hpp`.
- `src/Shader.hpp` and `src/Shader.cpp`.
- `src/Camera.hpp` and `src/Camera.cpp`.
- `src/Road.hpp` and `src/Road.cpp`.
- `shaders/road.vert` and `shaders/road.frag`.
- `docs/progress/almas.md` and your evidence files.

Coordinate edits to the other members' files with them. Integration ownership does not mean replacing their implementation without review.

### Step 1 — Publish the documentation baseline personally

Clone the repository as described in the setup section. Copy this AGENTS.md into the repository root. Keep any existing README and update it separately only as needed. Review the files and create your branch yourself:

```bash
git switch -c almas/core-road
```

Before publishing, check that all four names, repository links, and the no-AI-push rule are correct. Commit the documentation, push your branch personally, and open a pull request. Keep this initial documentation change small enough for a teammate to review.

Read the supplied R1–R8 requirements, then confirm topic registration, exam date and submission deadline with the instructor. Record the results in the plan section of this file when known; do not fill gaps with guesses.

### Step 2 — Establish the build

1. Check which operating systems the four members use.
2. Configure a C++17 executable target named `synthwave_drive`.
3. Add GLFW, GLEW, and OpenGL through a documented CMake dependency strategy.
4. Choose the shared math implementation before the first framework merge. Either provide a tested helper or agree on GLM and document it. Expose the agreed types through Math.hpp.
5. Keep all dependencies and compiler settings in CMake rather than private IDE project files.
6. Ignore generated build directories, platform metadata, and local editor state in .gitignore. Do not ignore source assets needed to run.
7. Configure and build from a clean build directory.
8. Write the commands that actually worked, including platform-specific dependency setup and executable location.

Do not promise Windows/macOS/Linux support without testing it. List verified platforms and any unverified ones honestly.

**Checkpoint:** a small executable builds on your computer, and the dependency instructions are sufficient for a teammate to attempt the same build.

### Step 3 — Create one reliable application loop

1. Initialize GLFW and check failure results.
2. Request a core OpenGL context appropriate to the platform; follow the macOS note in the shared-interface section.
3. Create the window, make its context current, and initialize GLEW.
4. Print the context information once and report initialization failures clearly.
5. Obtain actual framebuffer dimensions and install resize handling.
6. Calculate frame time from a monotonic clock. Prevent an abnormally long pause from moving the car far across the scene in one update; document any time-step clamp.
7. Read held controls into InputState and detect one-shot actions such as reset and camera changes.
8. Update state, update modules, render, swap buffers, and poll events in a consistent order.
9. On exit, destroy module GPU resources before destroying the window/context.

**Checkpoint:** one window opens, remains responsive, resizes, and closes cleanly. A startup failure should explain what failed rather than leaving a blank window.

### Step 4 — Provide shared types and shader utilities

Define the agreed fields from the shared-interface section with safe defaults. Avoid a large global variable collection. Make the state ownership rules clear in code.

The shader utility must:

- Load source files from a portable resource location.
- Report shader filenames and compile logs on failure.
- Report link errors separately.
- Avoid leaking failed shader/program objects.
- Define who owns and deletes each program.

Choose how shader files reach the runtime directory. A CMake copy step is acceptable; absolute paths to your own home directory are not. Test launching from a different working directory and either support it or explain the required invocation clearly.

Agree on module lifecycle signatures with the other members. Publish the headers early enough for them to compile against them.

**Handoff A:** send the framework branch/commit, tested build commands, shared headers, and a short example showing how a module is initialized and rendered. You send this yourself; AI must not message or publish for you.

### Step 5 — Implement the camera

1. Use the agreed Y-up axes and initial camera/car positions.
2. Calculate a view matrix looking along the road toward -Z.
3. Calculate a perspective matrix with a sensible near/far range covering the visible scene.
4. Derive aspect ratio from framebuffer width divided by height using floating-point arithmetic.
5. Skip rendering/projection recalculation while either framebuffer dimension is zero.
6. Add a second useful presentation angle and select it with C.
7. Keep camera position available in world space for Nursalim's specular lighting.
8. Make reset return to the initial camera.

**Checkpoint:** a reference object stays correctly proportioned when the window changes shape, and both camera modes show the road/car area.

### Step 6 — Implement the scrolling grid

1. Create a ground plane at Y=0 covering the visible road region.
2. Assign repeating UVs and create a VAO/VBO, with an index buffer if useful.
3. In the road vertex shader, apply the model, view, and projection matrices.
4. In the fragment shader, derive grid-line brightness from repeated UV coordinates and a line-width parameter.
5. Add a longitudinal offset driven by speed × deltaTime × a documented UV scale.
6. Wrap the offset by the grid's repetition period to avoid indefinite growth.
7. Check visually that the grid moves toward the camera, matching +Z environment motion.
8. Keep grid colors readable against the dark ground and avoid excessive flicker at the horizon. Start simple; tune filtering only if needed.
9. Connect W/S to bounded shared speed and R to reset.

Do not translate the car forward and also move the entire scene at the same intended speed. The project uses a car near fixed Z with scrolling surroundings.

**Checkpoint:** increasing speed makes the grid visibly faster, zero speed stops it, and no seam appears when the offset wraps.

### Step 7 — Integrate the other modules

1. Ask each author for exact source/shader paths and lifecycle calls.
2. Add their sources/resources to the shared build without overwriting their code.
3. Initialize modules after the context and shader utilities are ready.
4. Update the car before computing a camera that follows its position.
5. Render the environment background first, then opaque objects with depth testing/writing restored.
6. Connect Nursalim's lighting service to Turar's car materials.
7. Confirm that mountains and grid motion imply the same travel direction.
8. Perform a joint run and record remaining issues with owners.

Keep integrations small. If a module is not ready, use a clearly marked temporary placeholder and record the missing part; do not claim it is complete.

### Local verification

- Configure and build using the documented commands.
- Launch from a fresh checkout or ask a teammate to do so.
- Steer/speed/reset keys reach the expected module exactly once per action where appropriate.
- Both cameras work and resize preserves the aspect ratio.
- Grid speed changes continuously and repetition has no visible jump.
- Closing the window releases resources without a crash.
- Missing shader files cause an understandable error.

Capture a real screenshot such as `docs/evidence/almas-week8-grid.png`. In `docs/progress/almas.md`, record actual checks, platform, dependencies, blockers, and AI help.

### What to publish manually

For an initial handbook-only commit, review and stage AGENTS.md personally, then commit and push it yourself. For a later framework commit, explicitly stage only existing completed files, for example:

```bash
git status
git diff
git add CMakeLists.txt .gitignore src/main.cpp src/Math.hpp src/SceneState.hpp src/RenderContext.hpp
git add src/Shader.hpp src/Shader.cpp src/Camera.hpp src/Camera.cpp src/Road.hpp src/Road.cpp
git add shaders/road.vert shaders/road.frag docs/progress/almas.md
git diff --cached --check
git diff --cached
git commit -m "Add application framework, camera, and scrolling road"
git push -u origin almas/core-road
```

Run these yourself. Adjust the file list to the files that actually exist; split framework and road into separate commits if ready at different times. Open and merge PRs manually after review. Other members should start their implementation branches from main after your framework is merged.

### End-of-week acceptance

- [ ] Shared build setup and core headers are merged and usable by everyone.
- [ ] One window, loop, input system, camera system, and shader loader are used.
- [ ] Grid scrolling and speed controls work.
- [ ] Camera switch, reset, and resize work.
- [ ] First car, environment, and lighting modules run together.
- [ ] A second member can build and launch the shared scene.
- [ ] Progress and integration blockers are documented honestly.

### Explain at the defense

Be able to trace one frame from input to presentation, explain why deltaTime is needed, distinguish model/view/projection matrices, explain how UV scrolling creates motion, and show how resources are located and modules are connected.

### Suggested AI instruction

> Read this entire AGENTS.md, especially the shared rules and the Almas Alkhan Week 8 section. I am working on Almas Alkhan's assigned contribution. Help me implement only my Week 8 framework, road, camera, and integration tasks. Inspect the current files first. Work locally, explain key decisions, and verify what can actually be tested. Do not run any Git mutation, push, publish through an API, open a PR, or change remote state. Finish with a local handoff and commands for me to review and run manually.

---

<a id="turar"></a>

## Turar Nurbauli — detailed Week 8 instructions

**Role:** low-poly car, steering, and hierarchical transformations
**Branch:** `turar/car`
**Progress note:** `docs/progress/turar.md`

Read [START_HERE.md](#getting-started) and [INTERFACES.md](#interfaces) first. Your task is a reusable car module inside the common application, not a second standalone viewer.

### Required outcome

A recognizable 3D car is visible in the shared scene, moves left/right with input, stays within the road, and carries all its parts with it. Simple boxes and low-sided cylinders are sufficient for Week 8. Working integration matters more than a detailed model.

### Files you own

- `src/Car.hpp` and `src/Car.cpp`.
- Car-specific geometry/data files if needed, agreed with Almas before adding them.
- `docs/progress/turar.md`.
- Evidence files named `docs/evidence/turar-*`.

Do not replace main.cpp, the camera, the shared shader loader, or the common lighting shader. Ask Almas for build/main-loop changes and Nursalim for lighting/material integration.

### Before the framework is available

You can start these tasks immediately:

1. Sketch the car from the front and side.
2. List parts: chassis, cabin, four wheels, headlights, and neon strips.
3. Define approximate dimensions using the scale in the shared-interface section.
4. Choose an original mesh made from primitives rather than searching for a complex model.
5. Write down the hierarchy and each part's local offset.
6. Identify what your module needs: shared math, input, state, camera matrices, and lighting service.

Save these decisions in your progress note. Do not create a private GLFW window or a new math library while waiting.

### Step 1 — Start from the common framework

After the framework is merged, use a clean checkout and run these commands yourself:

```bash
git switch main
git pull --ff-only origin main
git switch -c turar/car
```

Build and launch the unchanged framework first. If it fails, report that baseline problem before adding car code. Confirm road width, coordinate signs, matrix conventions, and the finalized Car lifecycle with Almas.

### Step 2 — Design the geometry

1. Place the car facing -Z with wheels close to the Y=0 ground plane.
2. Start with a box-like chassis approximately 1.6 units wide and 3 units long.
3. Add a smaller cabin above the chassis.
4. Add four low-sided wheel meshes. Pick one cylinder-axis convention and keep rotation consistent with it.
5. Add small headlight and neon meshes. Keep their surfaces slightly separated from body surfaces where necessary to avoid flickering from coplanar geometry.
6. Use a consistent triangle winding so back-face culling does not unexpectedly hide parts.
7. Supply a unit normal for each lit vertex. For hard box edges, duplicate vertices per face instead of averaging all face normals into a rounded corner.
8. Create and upload GPU buffers once during initialization, not on every frame.

Use position at attribute 0 and normal at attribute 1 for the common lit shader. Document any additional vertex attributes before using them.

**Checkpoint:** the stationary car is recognizable from both agreed camera views; wheels sit near the ground and faces are not missing.

### Step 3 — Implement the transform hierarchy

Treat the car root transform as the parent of all parts. The conceptual relationship is:

```text
carRoot
  body
  cabin
  frontLeftWheel
  frontRightWheel
  rearLeftWheel
  rearRightWheel
  headlights
  neonDetails
```

Each child uses `parentWorld × localTransform`. The wheel's local rotation occurs around its own axle; it must not rotate around the world's origin.

1. Build the parent translation from the shared car position.
2. Apply a small steering tilt where appropriate.
3. Combine each part's local translation, rotation, and scale in the agreed matrix convention.
4. Check non-uniform scaling with Nursalim's normal-matrix handling.
5. Keep per-part data easy to inspect so you can explain the hierarchy.

**Checkpoint:** moving the parent moves every part together, and changing one wheel's local rotation does not move the cabin or other wheels.

### Step 4 — Implement steering

1. Receive input from the common InputState; do not poll GLFW inside Car.cpp.
2. Interpret steering input in [-1, 1].
3. Advance car X by steering input × lateral speed × deltaTime.
4. Clamp the car position using road width, the car's maximum lateral extent, and a small margin.
5. Make the bound conservative enough to account for the steering tilt.
6. Smooth visual tilt toward a small target angle when steering, and back toward zero when released.
7. Keep simulation position separate from visual tilt so the car does not drift after input is released.
8. Define reset behavior for X, tilt, and wheel angle.

**Checkpoint:** hold left for several seconds, then right. The whole car stays on the road, input release stops lateral movement, and reset returns it to the initial position.

### Step 5 — Connect speed and wheel rotation

This is a supporting feature; complete steering and hierarchy first.

1. Read the shared travel speed rather than a separate car speed variable.
2. Derive angular speed from linear speed divided by wheel radius.
3. Advance the wheel angle using deltaTime.
4. Wrap the angle to a bounded range.
5. Verify rotation direction visually against the scrolling grid.
6. When speed is zero, wheel rotation should stop.

If time runs short, record wheel animation as unfinished while delivering the working steerable car. Do not delay integration for wheel detail.

### Step 6 — Connect materials with Nursalim

Give Nursalim a short list of material needs:

- Body color with readable diffuse shading and highlights.
- Darker wheel material.
- Window/cabin material that remains opaque for the first version.
- Emissive headlight/neon colors.

Use the common lighting service and uniform contract. Avoid introducing a competing Phong shader just for the car. Emissive color can make a surface bright in Week 8; a glow around it requires later bloom and must not be claimed yet.

If lighting is not ready, coordinate a clearly marked temporary flat-color rendering path in the common application. Keep the geometry normals correct so it can switch to lighting without rebuilding the model design.

### Step 7 — Hand off integration

Provide Almas with:

- Exact source files to add to CMake.
- Initialization, update, render, reset, and cleanup calls.
- Input/state fields your module reads or writes.
- Required lighting-service calls and shader dependencies.
- Car dimensions and final lateral limits.
- A screenshot and known limitations.

Do not edit the shared main loop in parallel without coordinating. You remain responsible for fixing car-specific issues revealed during integration.

### Local verification

- Both camera views show a complete 3D car.
- Left/right steering and boundary clamps work.
- Children remain attached to the parent.
- Wheel transforms are local to their axle.
- Reset restores the intended state.
- Materials and normals are correct when the light is moved for testing.
- No geometry buffers are repeatedly allocated every frame.
- Behavior remains reasonable when the frame rate changes.

Capture `docs/evidence/turar-week8-car.png`. Record checks actually performed, dependencies, and AI help in `docs/progress/turar.md`.

### Publish manually

After building and inspecting your diff, run the following yourself. Stage additional car-specific files only if they exist and belong to your change:

```bash
git status
git diff
git add src/Car.hpp src/Car.cpp docs/progress/turar.md
git diff --cached --check
git diff --cached
git commit -m "Add steerable low-poly car and part hierarchy"
git push -u origin turar/car
```

Open a PR manually with base main and compare turar/car. Include build/run results, the screenshot, and integration instructions. Ask Almas to review the interface and Nursalim to check material compatibility. AI must not execute the commit/push or open the PR.

### End-of-week acceptance

- [ ] Recognizable 3D car with body, cabin, and wheels.
- [ ] Shared geometry and matrix conventions followed.
- [ ] Parent-child transformations work.
- [ ] Steering works and the full car stays within bounds.
- [ ] Reset works.
- [ ] Integrated rendering uses or is ready for the common lighting service.
- [ ] Module builds inside the shared application.
- [ ] Evidence, progress, and remaining limitations are recorded.

### Explain at the defense

Explain how a local part transform becomes a world transform, why translation/rotation/scale order matters, how steering depends on deltaTime, how road limits are calculated, and why face normals affect the appearance of hard edges.

### Suggested AI instruction

> Read this entire AGENTS.md, especially the shared rules and the Turar Nurbauli Week 8 section. I am working on Turar Nurbauli's assigned contribution. Help me implement the Week 8 car module only. Inspect shared headers before writing code. Use the existing loop, input, math, and lighting interfaces. Explain transformations and verify steering and hierarchy locally. Do not change Git state, push, use GitHub write APIs, or open a PR. Give me a handoff and the commands I should run personally.

---

<a id="muslim"></a>

## Muslim Syzdykov — detailed Week 8 instructions

**Role:** mountain landscape, striped sun, and gradient sky
**Branch:** `muslim/environment`
**Progress note:** `docs/progress/muslim.md`

Read [START_HERE.md](#getting-started) and [INTERFACES.md](#interfaces) first. Your work defines the environment around the road and car. It must run inside the common application.

### Required outcome

By the end of Week 8, mountains are visible on both sides of the road, a gradient sky fills the background, and a striped sun sits near the horizon. These are required Topic 13 features, so prioritize them over palms, clouds, textures, and decorative props.

### Files you own

- `src/Environment.hpp` and `src/Environment.cpp`.
- `shaders/background.vert` and `shaders/background.frag`.
- Environment-specific geometry/data files, if needed and agreed with Almas.
- `docs/progress/muslim.md`.
- Evidence files named `docs/evidence/muslim-*`.

Use the shared shader-loading and math utilities. Coordinate any extra shader files with Almas. Do not build a second camera system or application window.

### Before the framework is available

1. Sketch a composition showing the road, left/right mountain silhouettes, sun, and horizon.
2. Pick a small color palette: dark upper sky, brighter horizon, purple mountains, warm sun.
3. Design a few original low-poly mountain sections using vertex coordinates and triangles.
4. Work out a circular sun mask and horizontal stripe pattern on paper or in a short local calculation.
5. List the shared data you need: view/projection, camera orientation, framebuffer dimensions, speed, and deltaTime.

Record these choices in your progress note. Do not rely on a separate viewer that cannot integrate with the shared framework.

### Step 1 — Start from the common framework

After the baseline is merged, run these commands yourself from a clean checkout:

```bash
git switch main
git pull --ff-only origin main
git switch -c muslim/environment
```

Build and run the framework before changing it. Confirm road boundaries, camera orientation, shader resource paths, and how the background pass will be called.

### Step 2 — Create mountain geometry

1. Place the first mountain sections outside X=-4 and X=4, with an extra gap for the road edge.
2. Use triangulated peaks and ridges, keeping the silhouettes deliberately angular.
3. Vary height and spacing modestly so the landscape does not look like identical pyramids in a row.
4. Use consistent winding and normals for lit geometry. If mountains use a stylized unlit material initially, document that choice.
5. Upload the geometry once during initialization.
6. Reuse mesh buffers through different model transforms where possible.
7. Keep mountain tops and the sun visible from the follow camera.

**Checkpoint:** mountains appear on both sides without covering the road or clipping through the car. The second camera view does not reveal accidental holes caused by missing faces or incorrect winding.

### Step 3 — Implement the gradient background

1. Choose a background draw approach with Almas, such as a full-screen triangle whose shader reconstructs viewing direction.
2. Use the vertical component of that direction to blend upper-sky and horizon colors.
3. Use the same camera orientation as the 3D scene so the horizon responds consistently to camera rotation.
4. Ignore camera translation for the distant sky.
5. Handle different window aspect ratios through the agreed projection/framebuffer data.
6. Draw the background before the opaque scene and prevent it from writing scene depth.
7. Restore depth test/write state explicitly after the background pass.

Do not leave depth writes disabled, because the car and mountains would then overlap incorrectly. Confirm the state contract with Almas rather than relying on whatever the previous draw happened to leave active.

**Checkpoint:** the gradient fills the entire window at different sizes, and 3D objects render in front of it correctly.

### Step 4 — Add the striped sun

1. Choose a fixed distant sun direction near the horizon.
2. Derive coordinates relative to that direction in the background shader.
3. Use a circular mask for the disc. Make sure aspect ratio or the view-direction mapping does not stretch it into an ellipse.
4. Blend warm colors vertically across the disc.
5. Cut horizontal bands from the lower part or the agreed portion of the disc to create the synthwave striped look.
6. Use bounded, readable stripe spacing; keep the effect visible at the expected presentation resolution.
7. Composite the sun into the sky, then let opaque mountains naturally cover it where their geometry overlaps.
8. Check camera switching: the sun should appear distant and move consistently with viewing direction, not follow car translation like a nearby object.

A generated image of a sun is not a replacement for the required shader-based sun and sky.

**Checkpoint:** the disc is circular, the stripes are clear, and the sun does not incorrectly draw over mountains or the car.

### Step 5 — Add section recycling if needed

Start with a static landscape if necessary to get all required visuals integrated. If mountain sections move with the road:

1. Keep section positions in a bounded longitudinal range.
2. Advance positions by shared speed × deltaTime along +Z.
3. When a section is safely behind the camera, reposition it beyond the farthest section.
4. Preserve spacing and avoid visible pop-in at the front of the scene.
5. Reset positions to their initial arrangement on scene reset.
6. Stop section motion when shared speed is zero.

This must agree with the grid's perceived travel direction. Test several complete recycling cycles; do not judge continuity from a single screenshot.

If recycling is unfinished, report it clearly. Do not postpone the gradient sky, striped sun, or first integrated mountains to work on it.

### Step 6 — Integrate with Almas and Nursalim

Provide:

- Source and shader paths for CMake/resource copying.
- A background rendering call separate from mountain rendering.
- Any camera orientation or inverse-projection data required by your shader.
- Explicit depth/blend/cull states changed by your module and how they are restored.
- Mountain material needs and whether they use common lighting.
- Update/reset calls if sections move.
- The approved palette and a real screenshot.

Coordinate any required shared matrix-helper additions before relying on them. Keep the environment easy to turn on/off temporarily for debugging without changing the main architecture.

### Local verification

- Mountains exist on both sides and do not occupy the driving area.
- Sky and sun use shaders, not a substituted background image.
- The sun stays circular at wide and tall window sizes.
- Mountains and car occlude the background correctly.
- Both camera views preserve a coherent horizon.
- Any section recycling remains continuous over several loops.
- Reset and zero-speed behavior work where relevant.
- All shaders load through portable paths and errors include useful filenames.

Capture `docs/evidence/muslim-week8-environment.png` and record actual checks and AI use in `docs/progress/muslim.md`.

### Publish manually

Run these commands yourself after reviewing the source and result. Include additional environment files only when they exist and are part of your change:

```bash
git status
git diff
git add src/Environment.hpp src/Environment.cpp shaders/background.vert shaders/background.frag docs/progress/muslim.md
git diff --cached --check
git diff --cached
git commit -m "Add mountain landscape and shader-based sky and sun"
git push -u origin muslim/environment
```

Open a PR personally with base main and compare muslim/environment. Include the screenshot, tested camera/resize cases, and render-state requirements. Ask Almas to review integration and Nursalim to review material compatibility. AI must not perform the commit, push, or PR action.

### End-of-week acceptance

- [ ] Low-poly mountains on both sides of the road.
- [ ] Shader-generated gradient sky.
- [ ] Shader-generated circular sun with horizontal stripes.
- [ ] Correct depth relationship with the 3D scene.
- [ ] Consistent camera and resize behavior.
- [ ] Shared framework and resource conventions followed.
- [ ] Recycling status documented honestly.
- [ ] Evidence, progress, sources, and AI assistance recorded.

### Explain at the defense

Explain triangle-based mountain geometry, shader coordinate inputs, color interpolation, circular and stripe masks, why the background ignores translation, and how depth state lets foreground objects cover the sun.

### Suggested AI instruction

> Read this entire AGENTS.md, especially the shared rules and the Muslim Syzdykov Week 8 section. I am working on Muslim Syzdykov's assigned contribution. Help me implement only the Week 8 environment module: mountains, gradient sky, and striped sun. Reuse shared rendering utilities and coordinate any new shared matrix requirements. Explain masks, background coordinates, and depth state. Make local edits and checks only. Do not change Git state, push, publish through GitHub, or open a PR. Leave a clear human handoff.

---

<a id="nursalim"></a>

## Nursalim Onalbayev — detailed Week 8 instructions

**Role:** common lighting, material integration, and validation coordination
**Branch:** `nursalim/lighting-effects`
**Progress note:** `docs/progress/nursalim.md`

Read [START_HERE.md](#getting-started) and [INTERFACES.md](#interfaces) first. Week 8 is about a reliable lighting foundation. Bloom and CRT remain Week 9 stretch work after the required scene is stable.

### Required outcome

A common lit shader works in the shared application on a temporary mesh and, when available, Turar's car. The body has readable diffuse shading and highlights, and neon parts have a separate emissive contribution. The team has a small, reproducible visual-check procedure.

### Files you own

- `src/Lighting.hpp` and `src/Lighting.cpp`.
- `shaders/lit.vert` and `shaders/lit.frag`.
- `docs/progress/nursalim.md`.
- `docs/validation/week8.md` for aggregated checks.
- Evidence files named `docs/evidence/nursalim-*`.

Do not take over other members' geometry or main loop. Coordinate the test mesh and integration calls with Almas; coordinate normals, material parameters, and car draws with Turar.

### Before the framework is available

1. Define the initial light and material parameter list.
2. Write the lighting equations in your progress note in your own words.
3. Agree with Turar on vertex positions/normals and the common shader interface.
4. Implement at least two lights for R4, with one movable or toggleable; use separate controls and verify both contributions.
5. Draft a validation checklist covering normals, camera movement, material visibility, and resize.
6. List the shared math functions you need, especially normal-matrix calculation.

Do not implement a separate application loop or spend this time on bloom infrastructure. The shared framework and correct surface lighting are the immediate dependencies.

### Step 1 — Start from the common framework

After the baseline is merged, run these commands yourself from a clean checkout:

```bash
git switch main
git pull --ff-only origin main
git switch -c nursalim/lighting-effects
```

Build and run the untouched framework first. Confirm the shared math types, shader loader, uniform upload conventions, and GPU resource ownership.

### Step 2 — Establish the coordinate-space contract

Perform lighting calculations in world space for the first version:

1. Transform object-space position by the model matrix.
2. Transform the object-space normal by the inverse-transpose model 3×3 matrix.
3. Normalize the transformed normal before lighting calculations.
4. Pass world-space position and normal to the fragment shader.
5. Provide light and camera positions in world space.
6. Use model/view/projection only for clip-space position; do not accidentally use clip-space positions in the lighting equations.

Normal correction matters when Turar scales boxes differently along X, Y, and Z. Normalizing a wrongly transformed normal does not fix all non-uniform-scaling errors.

**Checkpoint:** lighting on a scaled reference mesh remains sensible and does not appear to move with the screen for the wrong reason.

### Step 3 — Implement basic lighting

Use the agreed common shader names and a documented material structure. A simple Phong-style model is sufficient for the first version:

- Ambient: a small base contribution that keeps shadowed surfaces visible.
- Diffuse: proportional to the clamped dot product between surface normal and direction to the light.
- Specular: a highlight controlled by the viewer direction, reflected light direction, and material shininess.
- Emissive: an additional surface color independent of illumination.

Implementation order:

1. Render normals as debug colors temporarily to inspect their direction.
2. Enable diffuse lighting with a known light position.
3. Add a small ambient term.
4. Add specular response and verify that it changes when the camera moves.
5. Suppress specular response on the unlit/back-facing side where appropriate.
6. Add emissive color as a separate contribution.
7. Use material values that produce visible differences without washing the whole car into white.

Keep the color-space assumptions consistent and document whether gamma correction is used. Do not add an incomplete color-management system that makes comparisons harder. Implement the required two-light system before optional shadows. Check attenuation and toggle behavior without confusing emissive colors with surface lighting.

### Step 4 — Expose a small reusable lighting service

The common service should make a car draw straightforward:

1. Bind the common lit program.
2. Upload frame data: view, projection, camera, and lights.
3. Upload per-object model and normal matrix.
4. Upload per-material color, strengths, shininess, and emissive value.
5. Let the geometry owner bind their VAO and draw.

Decide which data is uploaded once per frame and which changes per object. Do not hide ownership of Turar's mesh buffers inside Lighting. Avoid duplicating programs or deleting a program that another owner still uses.

Provide a short usage example in your progress note using the actual finalized interface. Label any preliminary pseudocode as such.

### Step 5 — Verify on a temporary mesh

Coordinate a small reference mesh with Almas, then test:

- Light on the front versus the side of the object.
- Camera movement with a stationary light.
- A non-uniformly scaled model.
- Different shininess and specular strengths.
- Emissive set to zero and then to a visible color.
- Darker wheel material beside brighter body material.

The object should be bright on the side facing the light. Specular highlights should respond to viewing direction. Emissive color should remain visible even when the diffuse term is small.

This is a controlled test to find coordinate/normal mistakes before diagnosing them on the entire car.

### Step 6 — Connect to Turar's car

1. Check that position and normal attributes use the agreed layout.
2. Inspect hard-edge normals on the chassis and cabin.
3. Provide initial body, wheel, and neon material settings.
4. Verify parent-child transformations and normal matrices for each part.
5. Check both camera views and steering tilt.
6. Confirm that neon surfaces look bright without pretending bloom already exists.
7. Send Turar specific geometry-normal issues with the affected part; do not silently rewrite his model.

**Checkpoint:** the car's 3D shape is readable, highlights are stable, and neon parts are visibly distinct from the body.

### Step 7 — Coordinate Week 8 validation

Create `docs/validation/week8.md` with short sections for:

- Build/platform information.
- Shared scene and required visuals.
- Steering, speed, camera, reset, and resize.
- Grid continuity and any environment recycling.
- Lighting/material correctness.
- Known issues with an owner and reproduction steps.

Ask each member to supply their actual results. Mark an untested case as “not tested”; do not turn an unchecked box into a pass because the code looks correct.

For a simple performance baseline, record hardware, framebuffer resolution, build configuration, whether VSync is enabled, approximate frame time/FPS, scene state, and sample duration. Treat VSync-capped FPS as a capped observation, not proof of spare performance. Do not add a large benchmarking system this week.

You coordinate checks; you are not responsible for fixing every module alone.

### Handoff to Almas

Provide source/shader paths, initialization/cleanup calls, the material structure, the actual draw-binding interface, world-space conventions, and the validation document. State any math helper or context requirement explicitly.

Do not start PostProcessing.cpp until the core checkpoint is complete and the team agrees. In Week 8, a note describing the planned framebuffer/bloom approach is enough.

### Local verification

- Both shader stages compile and link with useful error logs.
- Position, normal, light, and camera use compatible spaces.
- Non-uniform scaling does not corrupt normals.
- Diffuse and specular terms react to controlled changes.
- Emissive parts remain distinguishable without bloom.
- Materials render correctly on the actual car when available.
- Shader handles are not leaked or deleted by two owners.
- Validation entries distinguish tested, failed, blocked, and not-tested cases.

Capture `docs/evidence/nursalim-week8-lighting.png`. Record actual commands, checks, remaining issues, and AI assistance in your progress note.

### Publish manually

After reviewing your result, run the following personally. Stage the listed files only when they exist and are ready:

```bash
git status
git diff
git add src/Lighting.hpp src/Lighting.cpp shaders/lit.vert shaders/lit.frag docs/progress/nursalim.md docs/validation/week8.md
git diff --cached --check
git diff --cached
git commit -m "Add common lighting shader and Week 8 validation"
git push -u origin nursalim/lighting-effects
```

Open a PR manually with base main and compare nursalim/lighting-effects. Ask Turar to verify car compatibility and Almas to review framework integration. Include the lighting screenshot and controlled-test results. AI must not commit, push, open the PR, or publish by another route.

### End-of-week acceptance

- [ ] Common lit shader and material interface are documented and integrated.
- [ ] Correct world-space normals and positions, including non-uniform scale.
- [ ] Ambient, diffuse, specular, and emissive contributions work.
- [ ] Reference-mesh checks are recorded.
- [ ] Car lighting is integrated or its specific blocker is documented.
- [ ] Shared validation results identify owners for failures.
- [ ] Performance observations include their measurement conditions.
- [ ] Evidence, sources, and AI assistance are recorded.

### Explain at the defense

Explain each lighting term, why camera position is needed for specular highlights, why normals use an inverse-transpose matrix, how emissive color differs from bloom, and what the team's actual test evidence does and does not establish.

### Suggested AI instruction

> Read this entire AGENTS.md, especially the shared rules and the Nursalim Onalbayev Week 8 section. I am working on Nursalim Onalbayev's assigned contribution. Help me implement Week 8 lighting and local validation only. Reuse the framework and coordinate the material interface with the car module. Explain coordinate spaces and normal transforms. Do not start bloom/CRT before the core is stable. Do not change Git state, push, use GitHub write APIs, open PRs, or publish anything. Finish with verified results and a handoff for me to publish personally.

---

## November framework API additions

- `Mesh::initialize(vertices, strideFloats, indices, attributes)` owns a VAO/VBO/EBO; `draw()` issues indexed triangles. Reuse this for new geometry. Attributes carry location, component count and offset in floats. Position uses location 0; normals use 1; UV uses 2.
- `Input::mouse(dx,dy)` and `scroll(dy)` accumulate events and `consume()` clears each displacement once. Do not multiply mouse displacement by delta time. Held keyboard orbit uses delta time.
- Orbit yaw wraps; pitch is clamped to 0.08–1.4 radians and radius to 3–45 units. `CameraMode::Presentation` is the orbit mode, retained for compatibility.
- `SceneState` adds orbitYaw/orbitPitch/orbitRadius, paused, alternatePalette and showHelp. Road pause currently freezes road phase; integrate the same pause condition in future car/environment animation. Camera interaction must still work while paused.
- `Hud` renders last and restores depth-test/depth-write state. The FPS line remains when help is hidden. Keep FPS visible when adding bloom/CRT or recording.
- Default drawable is 1280×720. Retina scaling is disabled on macOS and initial events are processed before deterministic framebuffer capture.
- Read the current README controls before allocating teammate keys. I/J/K/L/U/O, G/H and Space are already used; choose non-conflicting lighting controls.
- The current code passes 40 CPU checks and GPU smoke checks. This does not prove the still-missing hierarchy, lighting or environment. Do not copy the prototype benchmark into final-scene results.
