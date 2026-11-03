
# Synthwave Drive

> An interactive 3D night drive through a retro-futuristic landscape.

A neon car faces a striped sun on the horizon. A glowing grid scrolls beneath it, low-poly mountains frame the road, and the player controls steering, speed, and the camera.

**Computer Graphics Fundamentals · Final Project · Topic 13**

## Project Goals

- Build a coherent interactive scene that satisfies the required Topic 13 features.
- Demonstrate transformations, camera projection, procedural geometry, shader-based visuals, and lighting.
- Keep the application modular so four team members can develop and integrate their contributions.
- Deliver a reproducible build, documented implementation, and clear live demonstration.

## Planned Experience

| Feature | Description |
| --- | --- |
| Scrolling grid | An endless neon road animated with repeating UV coordinates and a speed-dependent shader offset. |
| Low-poly mountains | Angular mountain sections with purple and pink materials, recycled ahead of the camera for an endless landscape. |
| Striped sun and sky | A smooth night-sky gradient with a warm circular sun and horizontal stripe masks. |
| Steerable neon car | A hierarchical 3D car that moves within road boundaries, tilts while steering, and uses emissive details. |

### Supporting Features

- **Speed control** — acceleration and braking drive road movement, wheel rotation, and environment motion.
- **Camera modes** — a follow camera and a second presentation view with resize-aware projection.
- **Hierarchical model** — wheels, headlights, and decorative parts inherit the car body transform while retaining local animation.
- **Lighting and materials** — ambient, diffuse, specular, and emissive materials distinguish the car and its neon elements.
- **Controls and reset** — on-screen hints and a reset action simplify demonstrations.

## Stretch Goals

These effects will be attempted only after the core scene is complete and stable:

- **Bloom** — extract and blur bright regions, then combine them with the original frame.
- **CRT scanlines** — apply a subtle switchable screen-space scanline effect.
- **Instanced roadside palms** — a reserve goal if the selected effects prove practical within the schedule.

Only completed and verified features will be presented as implemented.

## Technology

| Layer | Planned Technology |
| --- | --- |
| Application | C++17 |
| Rendering | OpenGL 3.3 Core, GLSL |
| Window and input | GLFW |
| OpenGL loading | GLEW |
| Build system | CMake |

Additional dependencies, supported platforms, and tested versions will be documented once implementation begins.

## Architecture

The application will use one window, one main loop, and shared scene state:

```text
main.cpp          initialization, input dispatch, frame timing, render loop
SceneState        speed, car position, camera selection, effect toggles
Camera            view/projection matrices and camera transitions
Road              ground geometry and animated grid coordinates
Car               model parts, transforms, steering, wheel animation
Environment       mountain sections, sky, striped sun
Lighting          light parameters and shared material settings
PostProcessing    framebuffer resources, bloom, CRT passes
```

### Integration Conventions

- **Y** points upward, **X** is the lateral axis, and the road extends along **Z**.
- The car remains near a fixed longitudinal position; scrolling geometry and UVs create forward motion.
- The main loop provides `deltaTime` and input to the modules.
- All 3D modules use the same camera and projection.
- Modules share agreed interfaces and do not create their own window or event loop.
- Asset paths must work from a fresh checkout without personal absolute paths.

## Frame Sequence

1. Read input and calculate elapsed frame time.
2. Update speed, steering, wheel rotation, camera, and environment movement.
3. Render the sky and sun, followed by the road, mountains, and car with correct depth handling.
4. Apply enabled post-processing effects.
5. Draw control hints and present the frame.

## Planned Repository Structure

```text
synthwave_drive/
├── README.md
├── PLAN.md
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

## Planned Controls

| Key | Action |
| --- | --- |
| `A / D` or `Left / Right` | Steer the car |
| `W / S` | Increase or decrease speed |
| `C` | Change camera mode |
| `B` | Toggle bloom, if implemented |
| `T` | Toggle CRT scanlines, if implemented |
| `R` | Reset the scene |
| `Esc` | Exit |

The final control mapping will be verified against the running application.

## Quality and Validation

Before submission, the team will verify that:

- All four Topic 13 core features work together in one scene.
- Grid repetition and environment recycling have no visible discontinuities.
- Steering stays inside the road and child objects follow the car correctly.
- Movement is consistent across frame rates.
- Camera changes and window resizing preserve correct rendering.
- Disabling post-processing leaves a fully working core scene.
- Frame-rate measurements include the test computer, resolution, and effect settings.
- A fresh checkout builds and runs on a second team member's computer.

The topic document also refers to general requirements **R1–R8** without defining them. The full requirements will be obtained and mapped to the implementation before final compliance is confirmed.

## Team

| Member | Responsibilities |
| --- | --- |
| **Almas Alkhan** | Application framework, scrolling road, camera, integration |
| **Turar Nurbauli** | Car geometry, steering, hierarchical transforms, wheel animation |
| **Muslim Syzdykov** | Mountains, sky, striped sun, environment composition |
| **Nursalim Onalbayev** | Lighting, post-processing, validation coordination |

Each member will contribute implementation, documentation, testing, and an explanation of their own code. The schedule, milestones, and individual deliverables are maintained in [`PLAN.md`](PLAN.md).

## Final Deliverables

- Source repository with tested setup instructions.
- PDF report covering implementation, contributions, algorithms, screenshots, validation, and limitations.
- Presentation slides.
- Demonstration video showing the implemented features and controls.

The topic document requires submission three days before the final exam. The exact date and Moodle upload requirements must be confirmed against the course schedule.

## Credits and AI Use

Original geometry and procedural visuals are preferred. External assets, libraries, or adapted code will be credited in `CREDITS.md` and the report, including applicable licenses.

AI assistance has been used to draft and organize this project specification and development plan. Any AI assistance used during implementation will also be documented. Every team member remains responsible for checking, understanding, and explaining the code they submit.
