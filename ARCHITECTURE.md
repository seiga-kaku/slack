# Slack Architecture

Slack is planned as a DAW/Tracker/Music Editor with clear boundaries between the real-time audio core, the application/UI layer, and hardware-optimized routines.

## Technology Split

- **Core engine:** Pure C
- **UI and application shell:** JUCE with C++
- **Hardware-specific acceleration:** Assembly and compiler intrinsics

This split keeps the audio engine portable and deterministic, while allowing the UI layer to move quickly with JUCE and leaving room for optimized low-level routines where profiling proves they are needed.

## Design Goals

1. Keep the audio engine independent from the UI toolkit.
2. Separate real-time safe code from non-real-time/editor code.
3. Make tracker and DAW workflows share the same song/project model.
4. Allow hardware-specific optimizations without coupling them to the rest of the engine.
5. Keep file formats, rendering, and playback logic testable outside the GUI.

## Proposed Repository Structure

```text
slack/
├── ARCHITECTURE.md
├── README.md
├── docs/
│   ├── decisions/
│   └── formats/
├── src/
│   ├── core/
│   │   ├── audio/
│   │   ├── engine/
│   │   ├── project/
│   │   ├── tracker/
│   │   ├── timeline/
│   │   └── dsp/
│   ├── platform/
│   │   ├── intrinsics/
│   │   └── asm/
│   └── ui/
│       ├── app/
│       ├── components/
│       ├── editors/
│       └── controllers/
├── tests/
│   ├── core/
│   └── integration/
└── third_party/
```

## Layer Responsibilities

### 1. Core Engine (`src/core/`)

Written in pure C and intended to build without JUCE.

Primary responsibilities:

- audio graph execution
- transport and playback state
- tracker pattern sequencing
- timeline/song arrangement
- DSP primitives and effects
- sample/instrument data management
- project serialization model
- undo-friendly command targets for editor actions

Rules:

- No JUCE types or C++ exceptions in the core.
- Real-time audio paths must avoid allocations, locks, and blocking I/O.
- The core should expose a stable API that the UI layer can call through thin wrappers.

### 2. UI/Application Layer (`src/ui/`)

Written in C++ using JUCE.

Primary responsibilities:

- main windowing and docking
- tracker editor, piano roll, mixer, and arrangement views
- command routing and shortcuts
- file dialogs, settings, and platform integration
- binding user interactions to core-engine commands
- non-real-time background tasks such as waveform generation and project browsing

Rules:

- UI state may cache views of core data, but the source of truth stays in the core/project model.
- Audio-thread work must never depend on JUCE UI objects.

### 3. Hardware Acceleration Layer (`src/platform/`)

Contains optional implementations for performance-critical code paths.

Primary responsibilities:

- SIMD versions of mixers, resamplers, filters, and buffer utilities
- CPU feature detection
- architecture-specific assembly where intrinsics are insufficient

Rules:

- Every optimized routine must have a portable C fallback.
- Selection of optimized code paths should happen behind a narrow dispatch interface.
- Optimized code should be introduced only after profiling identifies a bottleneck.

## Major Subsystems

### Project Model

Shared data model for:

- songs
- patterns
- instruments
- samples
- automation
- mixer routing
- timeline arrangements

The model should support both tracker-style step sequencing and DAW-style arrangement editing.

### Playback/Render Engine

Responsible for:

- sample-accurate timing
- pattern playback
- automation playback
- offline rendering/export
- transport synchronization

### Editing Systems

Editing should be expressed as commands that can be used by:

- tracker grid editing
- arrangement editing
- automation editing
- mixer actions
- undo/redo

### Plugin/Extension Boundary

If plugin support is added later, keep it outside the core scheduling model behind a dedicated host boundary so external processing cannot leak UI or platform assumptions into the engine.

## Threading Model

At minimum, separate:

- **Audio thread:** real-time playback and mixing
- **Message/UI thread:** rendering the JUCE interface and handling user input
- **Worker threads:** disk streaming, waveform analysis, indexing, export, and other background tasks

Communication between these areas should use lock-free queues, snapshots, or other real-time safe messaging patterns.

## Suggested Build Boundaries

- Build the core engine as a standalone C library.
- Build the JUCE application as a separate C++ target linking against the core library.
- Build hardware-optimized modules as optional objects selected by platform/capability.

This makes it easier to test the engine independently and potentially reuse it for command-line tools, exporters, or future headless workflows.

## Early Milestones

1. Define the core project/song data structures in C.
2. Establish the engine API boundary consumed by the JUCE layer.
3. Implement a minimal transport, pattern sequencer, and audio mixer.
4. Build a basic JUCE shell with project navigation and tracker editing.
5. Add profiling and then optimize the hottest DSP paths with intrinsics.

## Non-Goals for the First Iteration

- Premature assembly optimization before profiling
- Mixing UI framework concerns into core-engine data types
- Supporting every DAW workflow before tracker playback/editing is stable
