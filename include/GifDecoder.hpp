#pragma once

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
}

class GifDecoder{
private:
    AVFormatContext *fmt_ctx = nullptr;
    AVCodecContext* codec_ctx = nullptr;
    int video_stream_index = -1;

    AVPacket* packet = nullptr;
    AVFrame* frame = nullptr;

public:
    GifDecoder();

    ~GifDecoder();

    bool open(const char* filename);

    bool read_next_frame();

    int get_width() const;
    int get_height() const;
    AVFrame* get_current_frame() const;
    int get_fps() const;
};