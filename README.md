# slack
Open-source DAW/Tracker/Music Editor.

## Initial implementation

Implementation starts in the planned pure-C core with a minimal engine boundary under
`src/core/engine/slack_engine.[ch]`.

The first core module currently provides:

- default project metadata
- transport start/stop state
- beat-position tracking from sample rate and tempo

This keeps the first executable logic inside the core layer so later JUCE UI and
platform-specific code can build on a stable C API.
