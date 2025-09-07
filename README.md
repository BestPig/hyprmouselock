# hyprmouselock

Confines the cursor to the active window in Hyprland.

When enabled, the plugin prevents the cursor from leaving the main surface of the currently active window. On activation, the cursor is re-centered inside the window.

## Installation

Install with hyprpm/hyprnpm (recommended):

```
hyprpm update
hyprpm add https://github.com/BestPig/hyprmouselock
hyprpm enable hyprmouselock
hyprpm reload
```

Notes:
- You can use `BestPig/hyprmouselock` instead of the full URL on newer hyprpm.
- After enabling, hyprpm takes care of building and loading the plugin; a Hyprland restart or `hyprpm reload` may be required.

Build with `make` (recommended):

```
make
```

Or with CMake:

```
cmake -B build -S .
cmake --build build -j
```

Hot-load the plugin:

```
make load   # load the .so into Hyprland
make unload # unload the plugin
```

To load at startup, add to your Hyprland config:

```
plugin = /path/to/hyprmouselock.so
```

## Usage

The plugin exposes one dispatcher:

```
hyprmouselock:toggle
```

Example bind in `hyprland.conf`:

```
bind = SUPER, F1, hyprmouselock:toggle
```

Behavior:
- On activation: the cursor is centered on the active window.
- While enabled: every cursor move is clamped inside the active window’s main surface. If the cursor leaves the bounds, it is immediately warped back to the nearest edge and the move event is cancelled.

## Technical Details

- Confinement area: the window's main surface box (`getWindowMainSurfaceBox()`), excluding borders/decorations.
- Event hook: registers to the public `mouseMove` hook and cancels the event if a clamp/warp is applied.

## Compatibility

- Hyprland evolves quickly; build the plugin against the same version/commit as your Hyprland binary to avoid ABI issues.

## Troubleshooting

- Verify the plugin is loaded: `hyprctl plugin list`.
- Rebuild after Hyprland updates: `make && make load`.
- If the cursor is not confined, ensure a window is active and the toggle is ON.
