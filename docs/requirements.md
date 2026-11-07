# Final requirements and evidence

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
