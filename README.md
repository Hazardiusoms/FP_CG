Synthwave Drive
An interactive 3D night drive through a retro-futuristic landscape, built with C++ and OpenGL.
A neon car faces a striped sun on the horizon. A glowing grid scrolls beneath it, low-poly mountains frame the road, and the player controls steering, speed, and the camera.
Computer Graphics Fundamentals · Final Project · Topic 13
Project status: planning. This repository currently contains the project specification and team plan. The features below describe the intended implementation; application source code, build instructions, and demonstration media will be added as development progresses.
Development plan and team responsibilities
Project goals
Build a coherent, interactive scene that satisfies the required features of Topic 13.
Demonstrate transformations, camera projection, procedural geometry, shader-based visuals, and lighting.
Keep the application modular so that four team members can develop and integrate their contributions.
Deliver a reproducible build, a documented implementation, and a clear live demonstration.
Core features and implementation
Scrolling grid ground
The ground will create the impression of an endless road moving toward the camera.
We will draw a ground plane with repeating UV coordinates. A fragment shader will generate the grid lines, and a speed-dependent UV offset will animate their movement. The offset will advance using elapsed frame time (deltaTime) so that travel speed remains consistent at different frame rates. Repeating coordinates and a wrapped offset will keep the grid continuous during long runs.
Low-poly mountain landscape
Mountains will form a silhouette on both sides of the road without blocking the driving area.
We will construct simple triangulated meshes with angular surfaces and a restrained purple and pink palette. Mountain sections will be arranged along the road. If the sections move with the scene, sections behind the camera will be repositioned ahead of it to maintain the illusion of an endless landscape.
Striped sun and gradient sky
The horizon will contain a large striped sun against a smooth night-sky gradient.
A background shader will blend sky colors according to vertical position. The sun will use a circular mask, a warm color gradient, and horizontal stripe masks. Keeping the background independent of camera translation will make it appear distant, while its placement will remain consistent with the scene's viewing direction.
Steerable neon car
The player will move a 3D car left and right within the road boundaries.
The car will be assembled from simple meshes for the body, wheels, lights, and neon details. Keyboard input will update its lateral position, clamped to the road width. A small smoothed body tilt will make steering visible. Neon details will use emissive colors, while body surfaces will respond to lighting.
Supporting features
Speed control
The player will be able to accelerate and slow down. A shared speed value will drive the road offset, wheel rotation, and any moving environment sections. Limits will prevent negative or excessively high speeds.
Camera modes
The scene will include a camera behind the car and a second presentation view. View and projection matrices will be shared across the 3D modules. The projection aspect ratio will update when the window is resized.
Hierarchical car model
The car body will act as the parent of the wheels, headlights, and decorative parts. Each child will have a local transformation combined with the parent's model matrix. This allows the entire car to move together while the wheels rotate independently.
Lighting and materials
The car will use ambient, diffuse, and specular lighting. Surface normals and light directions will determine shading, while material parameters will control body color and highlights. Emissive parts will remain visually distinct from lit surfaces.
Controls and reset
An on-screen control hint and a reset action will make the demonstration easy to operate. Reset will restore the initial car position, speed, and camera state.
Stretch goals
These effects will be attempted only after the core scene is complete and stable.
Bloom
The scene will render to an off-screen framebuffer. Bright emissive regions will be extracted, blurred in horizontal and vertical passes, and combined with the original image to create a glow around neon elements. Bloom will have an on/off toggle for visual and performance comparisons.
CRT scanlines
A final screen-space shader will apply subtle horizontal intensity modulation to imitate a CRT display. The effect will be switchable and kept light enough to preserve readability.
Instanced roadside palms are a reserve stretch goal if the selected effects prove impractical within the schedule. Only completed and verified features will be presented as implemented.
Technology
Planned stack:
C++17 for application logic and scene modules.
OpenGL 3.3 Core for rendering.
GLSL for geometry shading, the sky, the sun, the grid, and post-processing.
GLFW for the window, OpenGL context, and keyboard input.
GLEW for OpenGL function loading.
CMake for building the project.
Additional dependencies will be documented if introduced. The final repository will include tested build and run instructions, required dependency versions, and supported platforms once the application exists.
Architecture
The application will use one window, one main loop, and shared scene state.
main.cpp — initialization, input dispatch, frame timing, and the render loop.
SceneState — speed, car position, camera selection, and effect toggles.
Camera — view and projection matrices and camera mode transitions.
Road — ground geometry and animated grid coordinates.
Car — model parts, hierarchical transforms, steering, and wheel animation.
Environment — mountain sections, sky, and striped sun.
Lighting — light parameters and shared material settings.
PostProcessing — framebuffer resources, bloom, and CRT passes.
Coordinate and integration conventions
Y points upward, X is the lateral axis, and the road extends along Z.
The car remains near a fixed longitudinal position; scrolling geometry and UVs create forward motion.
The main loop provides deltaTime and input to the modules.
All 3D modules use the same camera and projection.
Modules share agreed interfaces and do not create their own window or event loop.
Asset paths must work from a fresh checkout without personal absolute paths.
Frame sequence
Read input and calculate elapsed frame time.
Update speed, steering, wheel rotation, the camera, and environment movement.
Render the sky and sun, followed by the road, mountains, and car with appropriate depth handling.
Apply enabled post-processing effects.
Draw control hints and present the frame.
Planned repository structure
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
Only README.md and PLAN.md exist at the planning stage. The remaining files and directories will be created during implementation.
Planned controls
A / D or Left / Right — steer the car.
W / S — increase or decrease speed.
C — change camera mode.
B — toggle bloom, if implemented.
T — toggle CRT scanlines, if implemented.
R — reset the scene.
Esc — exit.
The final control mapping will be verified against the running application.
Quality and validation
Before submission, we will verify that:
All four Topic 13 core features work together in one scene.
Grid repetition and environment recycling have no visible discontinuities.
Steering stays inside the road and child objects follow the car correctly.
Movement is consistent across frame rates.
Camera changes and window resizing preserve correct rendering.
Disabling post-processing leaves a fully working core scene.
Frame-rate measurements include the test computer, resolution, and effect settings.
A fresh checkout builds and runs on a second team member's computer.
The topic document also refers to general requirements R1–R8 without defining them. We will obtain the full requirements and map them to the implementation before confirming final compliance.
Team
Almas Alkhan — application framework, scrolling road, camera, and integration.
Turar Nurbauli — car geometry, steering, hierarchical transforms, and wheel animation.
Muslim Syzdykov — mountains, sky, striped sun, and environment composition.
Nursalim Onalbayev — lighting, post-processing, and coordination of validation results.
Each member will contribute implementation, documentation, testing, and an explanation of their own code. The schedule, milestones, and individual deliverables are maintained in PLAN.md.
Final deliverables
Source repository with tested setup instructions.
PDF report describing the implementation, individual contributions, algorithms, screenshots, validation, and limitations.
Presentation slides.
Demonstration video showing the implemented features and controls.
The topic document requires submission three days before the final exam. The exact date and Moodle upload requirements must be confirmed against the course schedule.
Credits and AI use
Original geometry and procedural visuals are preferred. Any external assets, libraries, or adapted code will be credited in CREDITS.md and the report, including applicable licenses.
AI assistance has been used to draft and organize this project specification and development plan. Any AI assistance used during implementation will also be documented. Every team member remains responsible for checking, understanding, and explaining the code they submit.
