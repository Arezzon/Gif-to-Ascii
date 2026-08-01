#include "FrameScaler.hpp"
#include <iostream>

using namespace std;

FrameScaler::FrameScaler(int width, int height) : target_width(width), target_height(height) {
    scaled_frame = av_frame_alloc();
}

FrameScaler::~FrameScaler() {
    if (buffer) av_freep(&buffer);
    if (scaled_frame) av_frame_free(&scaled_frame);
    if (sws_ctx) sws_freeContext(sws_ctx);
}


bool FrameScaler::init(int src_width, int src_height, AVPixelFormat src_format) {
    sws_ctx = sws_getContext(src_width, src_height, src_format, target_width, target_height, AV_PIX_FMT_GRAY8, SWS_BILINEAR, nullptr, nullptr, nullptr);

    if (!sws_ctx) {
        cerr << "Can't initialize SwsContext!" << endl;
    }

    // Frame setup
    scaled_frame->width = target_width;
    scaled_frame->height = target_height;
    scaled_frame->format = AV_PIX_FMT_GRAY8;

    int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_GRAY8, target_width, target_height, 1);
    buffer = (uint8_t*)av_malloc(num_bytes * sizeof(uint8_t));

    if (!buffer) {
        cerr << "Can't allocate buffer for scaled frame!" << endl;
        return false;
    }

    av_image_fill_arrays(scaled_frame->data, scaled_frame->linesize, buffer, AV_PIX_FMT_GRAY8, target_width, target_height, 1);

    return true;
}

bool FrameScaler::scale(AVFrame* src_frame) {

    if(!sws_ctx || !scaled_frame || !src_frame) {
        cerr << "FrameScaler is not properly initialized!" << endl;
        return false;
    }

    int result_height = sws_scale(sws_ctx, src_frame->data, src_frame->linesize, 0, src_frame->height, scaled_frame->data, scaled_frame->linesize);

    if (result_height <= 0) {
        cerr << "Failed to scale frame!" << endl;
        return false;
    }

    return true;
}

AVFrame* FrameScaler::get_scaled_frame() const { return scaled_frame; }