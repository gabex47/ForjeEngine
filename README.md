# ForjeEngine

ForjeEngine is a small C++17/OpenGL 3D engine foundation. Phase 2 replaces the original triangle demo with a navigable 3D scene, reusable indexed cube geometry, scene entities, and fixed-step rigid-body physics.

## Current features

- OpenGL 3.3 core renderer with model, view, and perspective projection matrices
- Indexed cube mesh, per-object colors, directional lighting, back-face culling, 4x MSAA, and depth buffering
- Free-flying editor camera with frame-rate-independent movement and smoothed mouse look
- Entity-based scene with independent transform, renderable, box collider, and rigid-body components
- Fixed 120 Hz semi-implicit physics with gravity, damping, impulse response, friction, penetration correction, and a sweep-and-prune broad phase
- Dynamic cube spawning for rendering and physics stress tests
- Dependency-free foundation tests for scene identity, transforms, gravity, ground contacts, and cube stacking

## Controls

| Action | Input |
| --- | --- |
| Move | `W` `A` `S` `D` |
| Move up | `E` or `Space` |
| Move down | `Q` or `Left Ctrl` |
| Move faster | Hold `Left Shift` |
| Look | Mouse |
| Adjust camera speed | Mouse wheel or `+` / `-` |
| Spawn a cube | `F` |
| Release/capture mouse | `Tab` |
| Quit | `Escape` |

The window title shows the current cube count, camera speed, and mouse-capture state. Cube creation is capped at 2,000 entities to prevent accidental unbounded allocation from input.

## Build

Requirements are CMake 3.20+, a C++17 compiler, OpenGL, Python with Jinja2, and Git. CMake uses installed GLFW/GLAD packages when available and otherwise fetches pinned versions.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/ForjeEngine
```

Runtime shaders are copied beside the executable automatically, so the program can be launched from any working directory.

On a cloud-backed macOS Desktop with storage optimization enabled, use a non-synced build directory (for example `-B /private/tmp/forje-engine-build`) if generated files are repeatedly evicted while CMake is running.

## Architecture

```text
src/
├── Camera/    Free-fly camera and projection/view state
├── Core/      Application lifetime and main-loop orchestration
├── Graphics/  Shader, mesh, and renderer abstractions
├── Input/     Keyboard, mouse, scroll, and cursor capture
├── Math/      Minimal vector and column-major matrix primitives
├── Physics/   Fixed-step integration, broad phase, and contact solver
└── Scene/     Entity IDs and reusable components
```

The application owns subsystem lifetime but does not implement their behavior. Rendering consumes read-only scene data, while physics updates entities that opt into rigid-body and collider components. This keeps future model, material, lighting, scripting, serialization, and editor systems from becoming coupled to the main loop.

Physics currently uses world-space AABBs derived from each box transform. Rotated cubes receive conservative bounds rather than full oriented-box contacts; this is deliberate Phase 2 scope and leaves a clear narrow-phase seam for OBB, convex, or external physics backends later.
