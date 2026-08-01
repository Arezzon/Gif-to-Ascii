#pragma once

#include <SFML/Graphics.hpp>
#include <string>

extern "C" {
    #include <libavutil/frame.h>
    #include <libavutil/imgutils.h>
}

class AsciiRenderer {
private:
    sf::Font font;
    sf::RenderTexture render_texture;

    int cols_count;
    int rows_count;

    int pixel_width;
    int pixel_height;

    int m_font_size;

    AVFrame* out_frame = nullptr;
    std::string ascii_chars;

public:
    AsciiRenderer(int cols, int rows);
    ~AsciiRenderer();

    //RAII
    AsciiRenderer(const AsciiRenderer&) = delete;
    AsciiRenderer& operator = (const AsciiRenderer&) = delete;

    bool init(const std::string& font_path, int font_size = 12);

    AVFrame* renderer_frame(AVFrame* gray_frame);

    int get_pixel_width() const { return pixel_width; }
    int get_pixel_height() const { return pixel_height; }

};