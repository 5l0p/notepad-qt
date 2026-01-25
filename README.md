# Notepad Qt

A simple, lightweight text editor built with Qt, inspired by Windows Notepad.

## Features

- Dark UI theme with classic white text area
- Line numbers (toggleable)
- Monochrome syntax highlighting for common languages
- Find and Replace with case sensitivity option
- Go to line
- Word wrap (toggleable)
- Font selection
- Standard keyboard shortcuts

## Building

Requires Qt 5 development libraries.

```bash
qmake
make
```

## Usage

```bash
./notepad [file]
```

## Keyboard Shortcuts

| Action      | Shortcut       |
|-------------|----------------|
| New         | Ctrl+N         |
| Open        | Ctrl+O         |
| Save        | Ctrl+S         |
| Save As     | Ctrl+Shift+S   |
| Quit        | Ctrl+Q         |
| Undo        | Ctrl+Z         |
| Redo        | Ctrl+Shift+Z   |
| Cut         | Ctrl+X         |
| Copy        | Ctrl+C         |
| Paste       | Ctrl+V         |
| Select All  | Ctrl+A         |
| Find        | Ctrl+F         |
| Replace     | Ctrl+H         |
| Go to Line  | Ctrl+G         |

## License

Public domain
