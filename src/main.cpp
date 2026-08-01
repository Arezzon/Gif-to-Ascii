#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <cstdlib>
#include "GifDecoder.hpp"
#include "FrameScaler.hpp"
#include "GifEncoder.hpp"
#include "AsciiRenderer.hpp"
using namespace std;

extern "C" {
#include <libavutil/ffversion.h>
}

struct AppConfig {
    string gif_path      = "../giphy.gif";
    string font_path     = "../Mx437_IBM_BIOS.ttf";
    string output_path   = "output_test.gif";
    int    ascii_width   = 100;
    int    ascii_height  = 100;
    int    font_size     = 12;
};

namespace col {
    const char* reset  = "\033[0m";
    const char* green  = "\033[1;32m";
    const char* dim     = "\033[2;32m";
    const char* white  = "\033[1;37m";
    const char* amber  = "\033[1;33m";
    const char* red    = "\033[1;31m";
}

static void clearScreen() {
#if defined(_WIN32)
    system("cls");
#else
    system("clear");
#endif
}

static void printLogo() {
    cout << col::white;
    cout << "       _  __   _                         _ _ \n";
    cout << "  __ _(_)/ _| | |_ ___     __ _ ___  ___(_|_)\n";
    cout << " / _` | | |_  | __/ _ \\   / _` / __|/ __| | |\n";
    cout << "| (_| | |  _| | || (_) | | (_| \\__ \\ (__| | |\n";
    cout << " \\__, |_|_|    \\__\\___/   \\__,_|___/\\___|_|_|\n";
    cout << " |___/\n";
    cout << col::reset;
}

static void printBanner() {
    cout << "\n";
    printLogo();
    cout << "\n";

    cout << col::green << "  powered by FFmpeg + SFML" << col::reset << "\n\n";
    cout << col::amber << "  FFmpeg version: " << col::white << av_version_info() << col::reset << "\n\n";
}

static void printFooterLine() {
    cout << col::green
         << "+----------------------------------------------------------+"
         << col::reset << "\n";
}

static void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

static string askString(const string& prompt, const string& current) {
    cout << col::amber << "  " << prompt << col::dim << " [" << current << "]: " << col::reset;
    string value;
    getline(cin, value);
    return value.empty() ? current : value;
}

static int askInt(const string& prompt, int current) {
    cout << col::amber << "  " << prompt << col::dim << " [" << current << "]: " << col::reset;
    string line;
    getline(cin, line);
    if (line.empty()) return current;
    try {
        return stoi(line);
    } catch (...) {
        cout << col::red << "  Invalid number, keeping the previous value.\n" << col::reset;
        return current;
    }
}

static void configScreen(AppConfig& cfg) {
    cout << "\n";
    printFooterLine();
    cout << col::white << "  CONVERSION SETTINGS\n" << col::reset;
    printFooterLine();

    cfg.gif_path = askString("Path to GIF file", cfg.gif_path);
    cfg.font_path = askString("Path to font (.ttf)", cfg.font_path);
    cfg.output_path = askString("Path to output file", cfg.output_path);
    cfg.ascii_width = askInt("ASCII frame width (chars)", cfg.ascii_width);
    cfg.ascii_height = askInt("ASCII frame height (chars)", cfg.ascii_height);
    cfg.font_size = askInt("Font size (px)", cfg.font_size);

    printFooterLine();
    cout << "\n";
}

static void printSummary(const AppConfig& cfg) {
    printFooterLine();
    cout << col::white << "  CURRENT SETTINGS\n" << col::reset;
    printFooterLine();
    cout << col::green << "  GIF:      " << col::reset << cfg.gif_path << "\n";
    cout << col::green << "  Font:     " << col::reset << cfg.font_path << "\n";
    cout << col::green << "  Output:   " << col::reset << cfg.output_path << "\n";
    cout << col::green << "  Size:     " << col::reset << cfg.ascii_width << "x" << cfg.ascii_height << " chars\n";
    cout << col::green << "  Font px:  " << col::reset << cfg.font_size << "\n";
    printFooterLine();
    cout << "\n";
}

static int mainMenu() {
    cout << col::white << "  MAIN MENU\n" << col::reset;
    printFooterLine();
    cout << col::green << "  [1] " << col::reset << "Configure paths and parameters\n";
    cout << col::green << "  [2] " << col::reset << "Show current settings\n";
    cout << col::green << "  [3] " << col::reset << "Start conversion\n";
    cout << col::green << "  [0] " << col::reset << "Exit\n";
    printFooterLine();
    cout << col::amber << "  Your choice: " << col::reset;

    int choice;
    if (!(cin >> choice)) {
        clearInputBuffer();
        return -1;
    }
    clearInputBuffer();
    return choice;
}

static int runConversion(const AppConfig& cfg) {
    GifDecoder decoder;
    FrameScaler scaler(cfg.ascii_width, cfg.ascii_height);
    AsciiRenderer renderer(cfg.ascii_width, cfg.ascii_height);
    GifEncoder encoder;

    if (!decoder.open(cfg.gif_path.c_str())) {
        cerr << col::red << "  [ERROR] Failed to open GIF: " << cfg.gif_path << col::reset << "\n";
        return 1;
    }

    if (!renderer.init(cfg.font_path, cfg.font_size)) {
        cerr << col::red << "  [ERROR] Failed to initialize font: " << cfg.font_path << col::reset << "\n";
        return 1;
    }

    bool scaler_inited = false;
    bool encoder_inited = false;
    int frame_count = 0;

    printFooterLine();
    cout << col::white << "  PROCESSING FRAMES...\n" << col::reset;
    printFooterLine();

    while (decoder.read_next_frame()) {
        AVFrame* frame = decoder.get_current_frame();

        if (!scaler_inited) {
            if (!scaler.init(frame->width, frame->height, (AVPixelFormat)frame->format)) return 1;
            scaler_inited = true;
        }

        if (scaler.scale(frame)) {
            AVFrame* scaled_frame = scaler.get_scaled_frame();
            scaled_frame->pts = frame_count;

            AVFrame* ascii_frame = renderer.renderer_frame(scaled_frame);

            if (!encoder_inited) {
                int original_fps = decoder.get_fps();
                cout << col::green << "  Original FPS: " << col::white << original_fps << col::reset << "\n";

                if (!encoder.init(cfg.output_path.c_str(), renderer.get_pixel_width(),
                                   renderer.get_pixel_height(), original_fps,
                                   (AVPixelFormat)ascii_frame->format)) return 1;
                encoder_inited = true;
            }

            encoder.encode_frame(ascii_frame);
            frame_count++;

            if (frame_count % 10 == 0) {
                cout << col::dim << "  ...processed " << frame_count << " frames" << col::reset << "\n";
            }
        }
    }

    printFooterLine();
    if (encoder_inited) {
        encoder.finish();
        cout << col::amber << "  SUCCESS! Saved " << col::white << frame_count
             << col::amber << " frames to " << col::white << cfg.output_path << col::reset << "\n";
    } else {
        cout << col::red << "  No frames were encoded.\n" << col::reset;
    }
    printFooterLine();

    return 0;
}


int main() {
    AppConfig cfg;

    printBanner();

    bool running = true;
    while (running) {
        int choice = mainMenu();

        clearScreen();
        printBanner();

        switch (choice) {
            case 1:
                configScreen(cfg);
                break;
            case 2:
                printSummary(cfg);
                break;
            case 3:
                printSummary(cfg);
                runConversion(cfg);
                cout << "\n";
                break;
            case 0:
                cout << col::green << "  See you next time!\n" << col::reset;
                running = false;
                break;
            default:
                cout << col::red << "  Invalid choice, please try again.\n\n" << col::reset;
                break;
        }
    }

    return 0;
}