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

## Prerequisites

To build and run this project, you need **CMake**, a C++17 compiler, **FFmpeg** libraries, and **SFML**. 

On Ubuntu/Debian, you can install all dependencies via:

```bash
sudo apt update
sudo apt install build-essential cmake pkg-config \
                 libavformat-dev libavcodec-dev libavutil-dev libswscale-dev \
                 libsfml-dev