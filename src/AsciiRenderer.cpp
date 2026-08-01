#include "AsciiRenderer.hpp"
#include <iostream>

using namespace std;

AsciiRenderer::AsciiRenderer(int cols, int rows) : cols_count(cols), rows_count(rows) {
    ascii_chars = ".:-=+*#%@";

    out_frame = av_frame_alloc();
}

AsciiRenderer::~AsciiRenderer() {
    if (out_frame) {
        av_frame_free(&out_frame);
    }
}

bool AsciiRenderer::init(const std::string& font_path, int font_size) {
    m_font_size = font_size;
    if (!font.loadFromFile(font_path)) {
        cerr << "Failed loaf font form: " << font_path << endl;
        return false;
    }

    const sf::Glyph& glyph = font.getGlyph('A', font_size, false);

    float char_width = glyph.advance;
    float char_height = font.getLineSpacing(font_size);

    pixel_width = cols_count * char_width;
    pixel_height = rows_count * char_height;

    if (!render_texture.create(pixel_width, pixel_height)) {
        cerr << "Failed to create render texture!" << endl;
        return false;
    }

    // OUT FRAME
    out_frame->format = AV_PIX_FMT_GRAY8;
    out_frame->width = pixel_width;
    out_frame->height = pixel_height;

    // 32 bytes architecture
    if (av_frame_get_buffer(out_frame, 32) < 0) {
        cerr << "Failed to allocate frame buffer for ASCII render" << endl;
        return false;
    }

    cout << "AsciiRenderer initialized. Output resolution: " << pixel_width << "x" << pixel_height << endl;
    return true;
}

AVFrame* AsciiRenderer::renderer_frame(AVFrame* gray_frame) {
    if (!gray_frame) return nullptr;

    // Black Background
    render_texture.clear(sf::Color::Black);

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(m_font_size);
    text.setFillColor(sf::Color::White);

    float char_width = pixel_width / (float) cols_count;
    float char_height = pixel_height/ (float) rows_count;

    for (int y = 0; y < rows_count; ++y) {
        for (int x = 0; x < cols_count; ++x) {
            uint8_t pixel_value = gray_frame->data[0][y * gray_frame->linesize[0] + x]; // data[0] - brightness channel [[y * gray_frame->linesize[0] * x]] - pixel

            int char_idx = (pixel_value * (ascii_chars.length() - 1)) / 255; // map

            text.setString(std::string(1, ascii_chars[char_idx]));
            text.setPosition(x * char_width, y * char_height);

            render_texture.draw(text);
        }
    }

    render_texture.display();

    sf::Image sfml_image = render_texture.getTexture().copyToImage();
    const uint8_t* sfml_pixels = sfml_image.getPixelsPtr();

    //SFML to FFMPEG
    av_frame_make_writable(out_frame);
    for (int y = 0; y < pixel_height; ++y){
        for (int x = 0; x < pixel_width; ++x) {
            int sfml_idx = (y * pixel_width + x) * 4; // R G B A = 4
            out_frame->data[0][y * out_frame->linesize[0] + x] = sfml_pixels[sfml_idx];
        }
    }

    out_frame->pts = gray_frame->pts;

    return out_frame;
}