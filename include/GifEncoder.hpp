#pragma once

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/opt.h>
    #include <libavutil/imgutils.h>
}

class GifEncoder {
private:
    AVFormatContext *fmt_ctx = nullptr;
    AVCodecContext* codec_ctx = nullptr;
    AVStream* video_stream = nullptr;
    AVPacket* packet = nullptr;

    int width;
    int height;
    int fps;

public:
    GifEncoder();
    ~GifEncoder();

    GifEncoder(const GifEncoder&) = delete;
    GifEncoder& operator = (const GifEncoder&) = delete;

    bool init(const char* filename, int out_width, int out_height, int out_fps, AVPixelFormat pix_fmt);

    bool encode_frame(AVFrame* frame);

    bool finish();
};