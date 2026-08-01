#include "GifEncoder.hpp"
#include <iostream>

using namespace std;

GifEncoder::GifEncoder() {
    packet = av_packet_alloc();
}

GifEncoder::~GifEncoder() {
    if (packet) av_packet_free(&packet);
    if (codec_ctx) avcodec_free_context(&codec_ctx);
    if (fmt_ctx){
        if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
            avio_closep(&fmt_ctx->pb);
        }
        avformat_free_context(fmt_ctx);
    }
}

bool GifEncoder::init(const char* filename, int out_width, int out_height, int out_fps, AVPixelFormat pix_fmt) {
    width = out_width;
    height = out_height;
    fps = out_fps;

    if (avformat_alloc_output_context2(&fmt_ctx, nullptr, "gif", filename) < 0) {
        cerr << "Can't allocate context for output format!" << endl;
        return false;
    }

    const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_GIF);
    if (!codec) {
        cerr << "GIF encoder not found!" << endl;
        return false;
    }

    video_stream = avformat_new_stream(fmt_ctx, codec);
    if (!video_stream) {
        cerr << "Can't allocate stream!" << endl;
        return false;
    }

    video_stream->id = fmt_ctx->nb_streams - 1;

    codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        cerr << "Cant'allocate video codec context!" << endl;
        return false;
    }

    codec_ctx->codec_id = fmt_ctx->oformat->video_codec;
    codec_ctx->width = width;
    codec_ctx->height = height;
    codec_ctx->time_base = {1, fps};
    video_stream->time_base = codec_ctx->time_base;

    codec_ctx->pix_fmt = pix_fmt;

    if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
        cerr << "Can't open codec!" << endl;
        return false;
    }

    if (avcodec_parameters_from_context(video_stream->codecpar, codec_ctx) < 0) {
        cerr << "Can't copy the stream parameters" << endl;
        return false;
    }

    if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&fmt_ctx->pb, filename, AVIO_FLAG_WRITE) < 0) {
            cerr << "Can't open output file: " << filename << endl;
            return false;
        }
    }

    if (avformat_write_header(fmt_ctx, nullptr) < 0) {
        cerr << "Error occured when opening output file" << endl;
        return false;
    }

    cout << "GifEncoder succesfully initialized for: " << filename << endl;
    return true;

}

bool GifEncoder::encode_frame(AVFrame* frame) {
    if (!codec_ctx || !fmt_ctx) return false;

    int response = avcodec_send_frame(codec_ctx, frame);
    if (response < 0) {
        // cerr << "Error sending frame to encoder!" << endl;
        // return false;
        char errbuf[128];
        av_strerror(response, errbuf, sizeof(errbuf));
        cerr << "Error sending frame to encoder: " << errbuf << endl;
        return false;
    }

    while (response >= 0) {
        response = avcodec_receive_packet(codec_ctx, packet);

        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
            break;
        }
        else if (response < 0) {
            cerr << "Error encoding frame!" << endl;
            return false;
        }

        packet->stream_index = video_stream->index;
        av_interleaved_write_frame(fmt_ctx, packet);

        av_packet_unref(packet);
    }
    
    return true;
}

bool GifEncoder::finish() {
    if (!fmt_ctx) return false;

    encode_frame(nullptr);

    if (av_write_trailer(fmt_ctx) < 0) {
        cerr << "Error writing trailer" << endl;
        return false;
    }

    cout << "GIF successfully saved and closed!" << endl;
    return true;
}
