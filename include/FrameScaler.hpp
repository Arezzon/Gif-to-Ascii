#pragma once

extern "C" {
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
}

class FrameScaler {
private:
    SwsContext* sws_ctx = nullptr; // SWS - Software Width Scale (Software Scale)
    AVFrame* scaled_frame = nullptr;
    uint8_t* buffer = nullptr;

    int target_width = 0;
    int target_height = 0;

public:
    FrameScaler(int width, int height);

    ~FrameScaler();

    // RAII - no copy / no reference for two objects to one
    FrameScaler(const FrameScaler&) = delete;
    FrameScaler& operator = (const FrameScaler&) = delete; // Can't scaler2 = scaler1

    bool init(int src_width, int src_height, AVPixelFormat src_format);

    bool scale(AVFrame* src_frame);

    AVFrame* get_scaled_frame() const;
};
