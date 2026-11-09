# Almas Alkhan framework progress

Updated 7 November 2026. Group SE-2425. Local repository: FP_CG, branch almas/core-road. The earlier baseline is human-published commit 6e5582c. The present update is local and uncommitted; no AI Git mutations or remote writes were performed.

## Implemented changes

- Reusable non-copyable Mesh owns a VAO, VBO and EBO. The road now uses four vertices and six indices with glDrawElements.
- Explicit model uniforms in road/reference vertex shaders preserve the shared MVP convention.
- Interactive orbit camera: keyboard I/J/K/L, right-mouse drag, U/O and scroll zoom; pitch/radius bounds and frame-independent mouse displacement.
- Input handles held/one-shot actions, mouse accumulation and focus-loss clearing centrally.
- Space pauses the road; G changes its neon palette; H hides help while keeping FPS visible; R resets all implemented state.
- HUD uses original glyph arrays and modern shaders, with no new library. FPS updates every half-second.
- Default framebuffer is 1280×720. Startup event processing fixes hidden-window Retina capture sizing on macOS.
- Benchmark mode samples completed frames after warm-up. Capture/smoke/benchmark modes are mutually exclusive.
- Updated README, requirements, handbook, Weeks 8–10 plan, credits, demo/slide plans and template-based progress report.

## Validation

Release build succeeded with Apple Clang 17, GLFW 3.5.1, GLEW 2.3.1 and OpenGL 4.1 Metal on Apple M3 Pro. CTest passed; core_tests reports 40 checks. Actual GPU smoke checks passed for nonblank frames, road motion/repetition, camera selection, held-key behavior, speed, reset, portrait resize, mouse orbit, pause, palette, zero-size handling and GL errors.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
ctest --test-dir build --output-on-failure
./build/core_tests
./build/synthwave_drive --smoke-test build/smoke
./build/synthwave_drive --benchmark 2400
```

Benchmark: 1280×720, 60 warm-up frames then 2400 samples, vsync off and glFinish on. Mean throughput 1388.04 FPS; p95 frame time 1.45321 ms; worst 12.5889 ms. Timing includes event polling, update, draw, swap and GPU completion; the occasional window-title update is outside the sample. This short local prototype test does not establish sustained final-scene/TA performance. Raw output: ../evidence/benchmark-almas.txt.

New evidence: almas-follow.png, almas-presentation.png, almas-mouse-orbit.png, almas-palette.png and almas-portrait.png. Existing almas-week8-* images are historical. Single-frame captures display FPS --; the real-time window displays measured FPS.

## Remaining work

Review and understand the code personally. Integrate teammates' car hierarchy, environment and two-light shader; inventory at least five distinct scene objects. Verify R1–R8 on the integrated build and TA setup. Finalize journal quartile checks, report results, 8–12 slides and a 3–6 minute 1080p demo before the three-day submission cutoff. No final demo or complete final-scene report is claimed.

OpenAI Codex assisted with implementation, testing and documentation. Human revisions for this update are not yet recorded. Members should add specific changes after review; publication remains entirely human-operated.

## 9 November — road presentation and dashboard

Implemented locally on the existing almas/week10-update branch, without Git state
changes or publication. Added three-lane asphalt, filtered moving lane dashes,
curbs, guardrails, repeated lamps/reflectors, palette-aware edge colors and
analytic surface light spill. Road geometry uses shared indexed Mesh resources;
no additional library. Grid wraps at two metres and road furniture at twelve.
Recycling happens beyond the visible fog range and behind the camera limits.

Replaced the large status block with title/FPS, km/h dashboard and compact help.
Follow tracking uses an exponential response (rate 6 per second); it must run
immediately after future car updates. Camera framing is lower and looks farther
ahead. Requested 4x default-framebuffer MSAA. Existing teammate files are untouched.

Release build and 44 CPU checks pass. Real GPU checks pass: motion, complete
12-metre repeat, zero-speed stop, 12,000-second exact-cycle simulation, reset,
keyboard/mouse cameras, pause, palette, portrait and 320x240 resizing, zero-size
protection and GL errors. Captures were visually checked at 1280x720, 640x900 and
320x240. Latest evidence is almas-road-finished-follow.png and
almas-road-finished-orbit.png; earlier screenshots/report describe earlier builds.
Benchmark: Apple M3 Pro, OpenGL 4.1 Metal, 1280x720, 60 warm-up + 600 measured
frames, vsync off, glFinish on: 725.188 average FPS, p95 3.78621 ms,
worst 18.298 ms. This is a short local road-only measurement, not full-team or TA
performance certification. Raw log: ../evidence/almas-road-benchmark.txt.

OpenAI Codex implemented and tested this local update. Human review remains for
Almas. No claim of completed car, mountains, sun, two-light shading or bloom.
