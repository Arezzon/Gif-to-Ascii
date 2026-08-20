# GIF to ASCII Converter

A high-performance C++ tool that converts standard animated GIFs into fully rendered ASCII art animations. 

Instead of just printing ASCII characters to the console, this program processes the video frame-by-frame, maps pixel brightness to ASCII characters, and renders a completely new GIF file using a custom TrueType font (`.ttf`).

## Demonstration

| Original GIF | ASCII Art Result |
| :---: | :---: |
| <img src="giphy.gif" width="400"/> | <img src="output.gif" width="400"/> |

*(Font used in demonstration: `Mx437_IBM_BIOS.ttf`)*

## Features

* **Native FFmpeg Integration:** Uses `libavcodec` and `libavformat` for fast, low-level video decoding and encoding.
* **Smart Scaling:** Utilizes `libswscale` to downsample frames into a grid (e.g., 100x100 characters) while preserving grayscale brightness data.
* **Custom Font Rendering:** Uses **SFML** to draw ASCII characters onto a virtual canvas using any custom `.ttf` monospace font.
* **Dynamic Frame Rate:** Automatically detects the original GIF's FPS and applies the exact same framerate to the output file.
* **RAII Architecture:** Clean, memory-safe OOP design in C++ preventing memory leaks during complex FFmpeg buffer operations.
* **Portable Builds:** Standalone portable binaries available for both Windows and Linux.

---

## Quick Start (Portable Artifacts)

Pre-built portable packages are automatically generated for each commit and release via GitHub Actions.

### Windows
1. Download the `gif2ascii-windows` artifact (or release zip).
2. Extract the archive into a folder of your choice.
3. Run `gif2ascii.exe` (all required FFmpeg & SFML runtime `.dll` files as well as the default font are bundled).

### Linux (AppImage)
1. Download the `gif2ascii-linux` artifact (`Gif2Ascii-x86_64.AppImage`).
2. Make it executable:
   ```bash
   chmod +x Gif2Ascii*.AppImage
   ```
3. Run it:
   ```bash
   ./Gif2Ascii*.AppImage
   ```

---

## First Launch & Configuration

> [!IMPORTANT]
> **Check file paths on first launch!**
> When running the portable version for the first time, make sure to enter menu option **`[1] Configure paths and parameters`** to adjust the paths to your GIF and font files.

### Interactive Menu Overview

When the application starts, you will be greeted with the interactive terminal menu:

```text
  MAIN MENU
+----------------------------------------------------------+
  [1] Configure paths and parameters
  [2] Show current settings
  [3] Start conversion
  [0] Exit
+----------------------------------------------------------+
```

### Configuration Options (`[1]`)

| Parameter | Default | Description |
| :--- | :--- | :--- |
| **Path to GIF file** | `../giphy.gif` | Path to the source `.gif` animation you want to convert. |
| **Path to font (.ttf)** | `../Mx437_IBM_BIOS.ttf` | Path to any monospace TrueType font file. |
| **Path to output file** | `output_test.gif` | Name/path for the newly rendered ASCII `.gif`. |
| **ASCII frame width** | `100` | Grid width in characters. |
| **ASCII frame height** | `100` | Grid height in characters. |
| **Font size (px)** | `12` | Size of the font in pixels per character. |

> [!TIP]
> * Press **Enter** on any prompt to keep the default or current value shown in brackets `[...]`.
> * If your GIF and font are in the same folder as the executable, you can specify just their filenames (e.g., `my_animation.gif` and `Mx437_IBM_BIOS.ttf`).
> * After setting your paths, choose **`[3] Start conversion`** to render your animation!

---

## Building from Source

### Linux (Ubuntu / Debian)

1. **Install dependencies:**
   ```bash
   sudo apt update
   sudo apt install -y build-essential cmake pkg-config \
                       libavformat-dev libavcodec-dev libavutil-dev libswscale-dev \
                       libsfml-dev
   ```

2. **Configure & Build:**
   ```bash
   cmake -S . -B build
   cmake --build build -j4
   ```

3. **Run:**
   ```bash
   ./build/gif2ascii
   ```

---

### Windows (via MSYS2)

1. Download and install [MSYS2](https://www.msys2.org/).
2. Open the **MSYS2 UCRT64** terminal (`ucrt64.exe`).
3. **Install toolchain & libraries:**
   ```bash
   pacman -S mingw-w64-ucrt-x86_64-gcc \
            mingw-w64-ucrt-x86_64-cmake \
            mingw-w64-ucrt-x86_64-pkgconf \
            mingw-w64-ucrt-x86_64-ffmpeg \
            mingw-w64-ucrt-x86_64-sfml \
            mingw-w64-ucrt-x86_64-make
   ```
4. **Configure & Build:**
   ```bash
   cmake -S . -B build -G "MinGW Makefiles"
   cmake --build build -j4
   ```
5. **Run:**
   ```bash
   ./build/gif2ascii.exe
   ```
