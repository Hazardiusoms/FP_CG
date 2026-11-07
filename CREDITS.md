# Credits and AI Use

## Current baseline

- The grid and temporary reference geometry are generated in code; no external model, texture, font, or music asset is included.
- Math conventions build on the team's earlier Assignment 3 work and are implemented here as readable shared helpers with new tests. The Assignment 3 viewer/SMF loader is not part of this final project.
- Runtime dependencies: OpenGL, GLFW (zlib/libpng license), and GLEW (BSD/MIT-style licenses; see the installed package's license files). These dependencies are linked, not vendored in this handoff.
- Build tool: CMake. Test/evidence conversion used Python and Pillow locally; neither is needed to build or run the C++ application.

## AI assistance

An AI assistant prepared the project documentation and Almas's initial framework implementation: CMake setup, math/state/input helpers, shader loader, camera, scrolling road, reference geometry, tests, and handoff notes. It also ran the documented local build and validation checks. Almas must review, understand, and be able to explain the implementation before submission.

AI did not create Git commits, push, open pull requests, or otherwise publish repository changes. Each member performs these actions personally.

Add sources, licenses, and an honest AI-use record when the remaining modules introduce new code or assets.
