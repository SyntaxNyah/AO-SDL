# AO-SDL

## Project Overview

AO-SDL is a reimplementation of the Attorney Online 2 (AO2) client built on SDL2, OpenGL/Metal, and Dear ImGui. The codebase is organized into four strict layers: Engine, Platform, Plugins, and App. Focus is on correctness, performance, testability, and cross-platform support (Windows, Linux, macOS).

- **Architecture reference:** `Architecture.md`
- **Style reference:** `STYLE.md`
- **C++ standard:** C++20
- **Build system:** CMake 3.14+ with Ninja

---

## Build Commands

### Prerequisites

| Platform | Compiler | Tools | Graphics |
|---|---|---|---|
| macOS | Xcode command line tools | CMake, Ninja | Metal (system) |
| Linux (Ubuntu/Debian) | GCC or Clang (C++20) | CMake, Ninja, `libglew-dev`, `libssl-dev` | OpenGL + GLEW |
| Windows | MSVC (Visual Studio 2019+) | CMake, Ninja (via VS), VCPKG | Bundled GLEW in `third-party/` |

OpenSSL is optional — without it, HTTPS asset fetching is disabled (HTTP only).

### Clone

```sh
git clone --recursive <repo-url>
cd AO-SDL
# If already cloned without --recursive:
git submodule update --init --recursive
```

### Configure and Build

**macOS:**
```sh
cmake --preset macos-debug
cmake --build build
```

**Linux:**
```sh
sudo apt install cmake ninja-build libglew-dev libssl-dev
cmake --preset linux-debug
cmake --build out/build/linux-debug
```

**Windows (Developer Command Prompt / PowerShell with VS environment):**
```sh
cmake --preset x64-debug
cmake --build out/build/x64-debug
```

Available presets: `x64-debug`, `x64-release`, `x86-debug`, `x86-release`, `linux-debug`, `macos-debug`.

Build outputs land in `out/build/<preset-name>/` (Windows/Linux) or `build/` (macOS).

### Run Tests

```sh
# From the build directory's tests subfolder:
cd <build-dir>/tests
./aosdl_tests
# Or via CTest from the build directory:
ctest

# Run a specific test:
./aosdl_tests --gtest_filter="AOEmotePlayer*"
```

### Optional Targets

```sh
cmake --build <build-dir> --target docs           # Doxygen API docs (requires doxygen)
cmake --build <build-dir> --target run-clang-tidy # Static analysis (requires clang-tidy)
```

### Git Hooks (Auto-format)

```sh
git config core.hooksPath .githooks
```

The pre-commit hook auto-formats staged `.cpp`, `.h`, and `.mm` files with `clang-format`. Install via `brew install clang-format`, `apt install clang-format`, etc. If not found, the hook is skipped with a warning.

---

## Repository Structure

```
AO-SDL/
├── include/            # Public engine interfaces (headers only, no implementation)
│   ├── asset/          # AssetLibrary, AssetCache, MountManager interfaces
│   ├── event/          # EventManager, EventChannel<T>
│   ├── net/            # ProtocolHandler, HttpPool, WebSocket, ITcpSocket
│   ├── render/         # IRenderer, IScenePresenter, Layer, RenderState, Transform
│   ├── ui/             # UIManager, IUIRenderer
│   └── utils/          # Log, Base64, JsonValidation, Version.h.in
├── engine/             # Core engine implementation (no GL, no SDL)
│   ├── asset/          # AssetLibrary, AssetCache, MountManager, decoders (WebP, APNG, GIF, stbi, Opus)
│   ├── audio/          # AudioThread, AudioStream
│   ├── event/          # All event types (Chat, IC, UI, ServerList, etc.)
│   ├── game/           # GameThread, ServerList
│   ├── net/            # WebSocket, KissnetTcpSocket, NetworkThread, HttpPool
│   ├── render/         # RenderManager, StateBuffer, AnimationPlayer, TextRenderer, GlyphCache, Transform
│   ├── ui/             # UIManager
│   └── utils/          # Log, Base64, JsonValidation
├── platform/           # Platform-specific implementations (font discovery, hardware ID, time)
│   ├── macos/
│   ├── windows/
│   └── linux/
├── plugins/            # Game and renderer plugins — depend only on engine interfaces
│   ├── ao/             # AO2 game logic
│   │   ├── asset/      # AOAssetLibrary, AOCharacterSheet
│   │   ├── event/      # ICMessageEvent
│   │   ├── game/       # AOCourtroomPresenter, AOEmotePlayer, AOTextBox, ICMessageQueue, AOBackground, effects
│   │   └── ui/screens/ # ServerListScreen, CharSelectScreen, CourtroomScreen
│   ├── net/ao/         # AOClient, AOPacket, PacketTypes, PacketBehavior
│   └── render/
│       ├── gl/         # GLRenderer, GLTexture, GLMesh, GLSprite, Shader (OpenGL 4.5)
│       └── metal/      # MetalRenderer, MetalTexture (Apple Metal, Pimpl)
├── apps/
│   └── sdl/            # SDL application entry point — wires everything together
│       ├── main.cpp
│       ├── SDLGameWindow.cpp/.h
│       ├── audio/
│       ├── render/     # GLBackend.cpp, MetalBackend.mm, IGPUBackend.h
│       └── ui/
│           ├── ImGuiUIRenderer.cpp/.h
│           ├── controllers/  # ServerListController, CharSelectController, CourtroomController
│           └── widgets/      # ChatWidget, CourtroomWidget, ICChatWidget, ICLogWidget, MusicAreaWidget, etc.
├── tests/              # Google Test suite (fetches GTest 1.14.0 via FetchContent)
│   ├── ao/             # AO plugin tests (asset, event, game, net, ui)
│   ├── asset/          # Engine asset tests
│   ├── audio/
│   ├── event/
│   ├── game/
│   ├── net/            # WebSocket, HttpPool, NetworkThread + MockTcpSocket.h
│   ├── render/
│   ├── stubs/          # StubTexture.cpp (no-op ITexture for headless tests)
│   ├── ui/
│   └── utils/
├── third-party/        # All vendored libraries (SDL2, imgui, freetype, libwebp, bit7z, etc.)
├── assets/             # Embedded assets compiled into the binary via EmbedAssets.cmake
├── scripts/
│   └── ci-failures.py  # CI failure analysis script
├── CMakeLists.txt
├── CMakePresets.json
├── Architecture.md
├── STYLE.md
└── codecov.yml
```

---

## CMake Targets

| Target | Type | Description |
|---|---|---|
| `aoengine` | Static library | Core engine (no GL, no SDL) |
| `ao_game` | Static library | AO2 game logic plugin |
| `ao_net` | Static library | AO2 network protocol plugin |
| `ao_protocol` | Static library | Thin glue: ao_net + ao_game + UI screens |
| `aorender_gl` | Static library | OpenGL 4.5 renderer (Windows, Linux, macOS) |
| `aorender_metal` | Static library | Metal renderer (Apple only) |
| `aosdl` | Executable | SDL application binary |
| `aosdl_tests` | Executable | Google Test suite |
| `docs` | Custom target | Doxygen API docs (if doxygen found) |
| `run-clang-tidy` | Custom target | clang-tidy static analysis (if clang-tidy found) |

---

## Architecture

### Layered Dependency Rules

```
┌─────────────────────────────────────────────┐
│  App Layer (apps/sdl/)                      │
│  SDL window, ImGui widgets, controllers     │
│  Depends on: Engine, Plugins (via factory)  │
├─────────────────────────────────────────────┤
│  Plugin Layer (plugins/)                    │
│  AO game logic, GL/Metal renderers, AO net  │
│  Depends on: Engine interfaces only         │
├─────────────────────────────────────────────┤
│  Engine Layer (engine/, include/)           │
│  Assets, events, rendering, networking      │
│  Depends on: Nothing above                  │
├─────────────────────────────────────────────┤
│  Platform Layer (platform/)                 │
│  System fonts, hardware ID                  │
│  Depends on: OS APIs only                   │
└─────────────────────────────────────────────┘
```

**Critical rule:** Plugins only link against engine interfaces. The engine has zero knowledge of any plugin. The app layer wires everything together at startup through factory functions.

### Threading Model

```
┌────────────────────────────────┐
│ Main Thread                    │
│ SDL event loop + ImGui render  │
│ HTTP poll + UI controllers     │
│ Reads from StateBuffer         │
└───────────┬────────────────────┘
            │
    ┌───────┴───────┐
    │  StateBuffer   │ (triple-buffered RenderState)
    └───────┬───────┘
            │
┌───────────┴────────────────────┐
│ Game Thread (~10 Hz)           │
│ IScenePresenter::tick()        │
│ Consumes events, produces      │
│ RenderState, writes StateBuffer│
└───────────┬────────────────────┘
            │
    ┌───────┴───────┐
    │ EventManager   │ (typed FIFO channels)
    └───────┬───────┘
            │
┌───────────┴────────────────────┐
│ Network Thread                 │
│ ProtocolHandler::poll()        │
│ Publishes events from packets  │
└────────────────────────────────┘
```

- **EventChannel\<T\>** — Type-safe FIFO queues for discrete events between threads. Never share mutable state directly.
- **StateBuffer** — Triple-buffered snapshot exchange; the game thread writes `RenderState`, the render thread reads it without blocking either side.

### Key Interfaces (in `include/`)

- `IRenderer` — GPU rendering backend (OpenGL or Metal)
- `IScenePresenter` — Game logic that produces renderable frames each tick
- `ProtocolHandler` — Network protocol implementation (AO2 or future protocols)
- `ITcpSocket` — TCP transport (real `KissnetTcpSocket` or `MockTcpSocket` for tests)
- `Mount` — Virtual filesystem backend (directory, archive, HTTP, embedded)

### Asset Pipeline

1. **MountManager** — Virtual filesystem. Searches mounts in priority order: `MountDirectory` (disk) → `MountArchive` (7z/zip via bit7z) → `MountEmbedded` (compiled into binary) → `MountHttp` (remote HTTP).
2. **AssetLibrary** — Format probing: tries extensions in preference order (webp → apng → gif → png). Upgrades are transparent to callers.
3. **AssetCache** — LRU cache with soft memory limit. Assets held by `shared_ptr` callers are never evicted even under memory pressure.

### Rendering Pipeline

- Game thread produces a `RenderState` (ordered `LayerGroup` map with `Layer` objects).
- Each `Layer` holds: `ImageAsset` (texture + frame index), `Transform`, optional `ShaderAsset`/`MeshAsset`, opacity.
- Both renderers draw to an offscreen framebuffer; the texture is composited into the ImGui scene via `ImGui::Image()`.
- `GLRenderer` — OpenGL 4.5, caches textures/meshes/shaders keyed by asset pointer + generation counter. Supports wireframe debug mode.
- `MetalRenderer` — Apple Metal via Pimpl pattern.

### Text Rendering

1. `TextRenderer` rasterizes glyphs via FreeType.
2. `GlyphCache` packs glyphs into a 2048px texture atlas (row-based bin packing), pre-caches printable ASCII.
3. `TextMeshBuilder` converts text layout into a `MeshAsset` with per-glyph quads and atlas UVs.

### UI Architecture (Screen → Controller → Widget)

- **Screen** (in plugins) — Represents a game state. Managed as a stack by `UIManager`.
- **IScreenController** (in app) — Bridges a Screen to ImGui rendering. Created by factory in `ImGuiUIRenderer`.
- **IWidget** (in app) — Individual ImGui component. Each implements `handle_events()` and `render()`.
- Controllers own their widgets and orchestrate layout. Widgets publish outgoing events for user actions. Navigation via `NavAction` return values (e.g., `POP_SCREEN`, `POP_TO_ROOT`).

---

## Coding Style

Full details in `STYLE.md`. Key rules:

### Formatting

- 4 spaces, no tabs. Column limit: 120.
- Enforced by `.clang-format` (applied automatically via pre-commit hook).
- `#pragma once` — never `#ifndef` guards.

### Naming

| Category | Convention | Example |
|---|---|---|
| Classes, structs, enums, type aliases | PascalCase | `AssetCache`, `LayerGroup` |
| Functions and methods | snake_case | `fetch_data()`, `get_z_index()` |
| Local variables and parameters | snake_case | `delta_ms`, `relative_path` |
| Private/protected member variables | trailing underscore | `playing_`, `elapsed_ms_` |
| Public member variables | no suffix | `valid`, `conn_state` |
| Static constexpr constants | UPPER_CASE | `DELIMITER`, `MIN_FIELDS` |
| Getters (return value) | `get_` prefix | `get_opacity()` |
| Getters (return reference to internal object) | no prefix | `transform()`, `asset()` |
| Setters | `set_` prefix | `set_opacity()` |
| Boolean queries | `is_` or `has_` prefix | `is_playing()`, `has_frame()` |
| Header files | PascalCase matching class | `AssetCache.h`, `GLRenderer.h` |

### Ownership

- `std::shared_ptr<T>` for reference-counted assets (cache + callers + render snapshots).
- `std::unique_ptr<T>` for exclusive ownership (mounts, subsystem instances).
- `const T&` for non-owning parameters. `T&&` for sink parameters.
- Raw pointers are non-owning observers — never `delete` a raw pointer.

### Error Handling

- `std::optional<T>` for expected "not found" — `get_event()`, `fetch_data()`, `probe()`.
- `nullptr` for pointer-typed lookups that may miss.
- Exceptions for protocol violations and broken invariants (`PacketFormatException`, `ProtocolStateException`).
- Do not throw for routine missing-asset cases (these happen regularly during HTTP streaming).

### Other Rules

- No `using namespace std;` — always qualify.
- Mark methods `const` whenever they don't mutate logical state.
- Use `mutable` only for synchronization primitives.
- `struct` for plain data aggregates; `class` for types with invariants or non-trivial methods.
- Mark single-argument constructors `explicit`.
- Use member initializer lists, not body assignment.
- Delete copy constructor/assignment when the type must not be copied.

---

## Testing

Tests use Google Test 1.14.0 (fetched automatically via CMake `FetchContent`).

The test binary is `aosdl_tests`. A `StubTexture.cpp` no-op implementation of `ITexture` is linked so tests run without the GL renderer. `MockTcpSocket` (in `tests/net/`) enables WebSocket and networking tests without real network access.

### Test Coverage Areas

| Area | Files |
|---|---|
| AO2 protocol (packets, wire format) | `tests/ao/net/` |
| AO2 game logic (emote player, textbox, background, effects, message queue) | `tests/ao/game/` |
| AO2 assets (asset library, character sheet) | `tests/ao/asset/` |
| AO2 UI screens | `tests/ao/ui/` |
| AO2 events (ICMessageEvent) | `tests/ao/event/` |
| Engine assets (cache, APNG, image decoders, mount system) | `tests/asset/` |
| Engine events (channel, manager, types) | `tests/event/` |
| Rendering (animation player, layers, state buffer, transform, text mesh) | `tests/render/` |
| Networking (WebSocket, HttpPool, NetworkThread) | `tests/net/` |
| Game (ServerList, TickProfiler) | `tests/game/` |
| UI (UIManager) | `tests/ui/` |
| Audio (AudioStream) | `tests/audio/` |
| Utilities (Base64, UTF-8, JSON validation, Log, BlendOps) | `tests/utils/` |

---

## Third-Party Dependencies

All dependencies are vendored as git submodules in `third-party/` and built from source (except GLEW on Windows, which is pre-built).

| Library | Purpose | Notes |
|---|---|---|
| SDL2 | Windowing, input, platform abstraction | Static build; joystick, haptic, sensor disabled |
| Dear ImGui | UI toolkit | SDL2 + OpenGL3/Metal backends; `IMGUI_USE_WCHAR32` for emoji |
| FreeType | Font rasterization | HarfBuzz, bzip2, brotli, PNG disabled |
| libwebp | WebP image codec | Decode only; tools and encoding disabled |
| bit7z | 7z/zip archive extraction | Auto-format detection |
| Ogg + Opus + Opusfile | Opus audio codec | Opusfile built manually (no find_package) |
| kissnet | TCP sockets | Header-only |
| stb_image | Static image loading | Header-only (stb) |
| GLEW | OpenGL extension loading | Bundled on Windows; system on Linux |
| cpp-httplib | HTTP client | Compiled into engine; OpenSSL optional |
| nlohmann/json | JSON parsing | Header-only |
| Google Test | Unit testing | Fetched via FetchContent at configure time |
| miniaudio | Audio playback | Header-only |

---

## Coverage

Codecov ignores: `third-party/**`, `tests/**`, `build/**`, `out/**` (see `codecov.yml`).

---

## Version String

Automatically generated from git at CMake configure time:
- Clean commit: `<short-hash>` (e.g., `a1b2c3d`)
- Dirty working tree: `<short-hash>-dirty`
- No git: `unknown`

Output header: `<build-dir>/generated/utils/Version.h` (generated from `include/utils/Version.h.in`).

---

## LTO

Link-time optimization is enabled automatically for `Release` and `MinSizeRel` builds if the compiler supports it (checked via `CheckIPOSupported`). This significantly reduces binary size.

---

## Platform Notes

- **Windows:** `WIN32_LEAN_AND_MEAN` and `NOMINMAX` are defined globally. VCPKG is used for dependency management (`x64-windows-static` triplet). MSVC debug info (`/Zi`) enabled in Debug builds.
- **macOS:** Metal renderer (`aorender_metal`) is built instead of GL. Platform files are `.mm` (Objective-C++). Frameworks linked: `Metal`, `MetalKit`, `QuartzCore`, `CoreText`, `Foundation`, `IOKit`.
- **Linux:** GLEW and OpenSSL sourced from system packages. Build output in `out/build/linux-debug/`.
