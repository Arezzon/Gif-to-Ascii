#include "GifDecoder.hpp"
#include <iostream>

using namespace std;

GifDecoder::GifDecoder() {
    packet = av_packet_alloc();
    frame = av_frame_alloc();
}

GifDecoder::~GifDecoder() {
    if (frame) av_frame_free(&frame);
    if (packet) av_packet_free(&packet);
    if (codec_ctx) avcodec_free_context(&codec_ctx);
    if (fmt_ctx) avformat_close_input(&fmt_ctx);
}

bool GifDecoder::open(const char* filename) {
    if (avformat_open_input(&fmt_ctx, filename, nullptr, nullptr) < 0) {
        cerr << "Can't open file: " << filename << endl;
        return false;
    }
    else {
        cout << "File '" << filename << "' opened!" << endl;
    }

    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        cerr << "Can't find stream information" << endl;
        return false;
    }

    for(unsigned int i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index == -1) {
        cerr << "Can't find video stream in file" << endl;
        return false;
    }

    AVCodecParameters* codec_params = fmt_ctx->streams[video_stream_index]->codecpar;

    cout << "Found video stream! Index: " << video_stream_index << endl;
    cout << "Gif resolution: " << codec_params->width << "x" << codec_params->height << endl;

    const AVCodec* codec = avcodec_find_decoder(codec_params->codec_id);

    if (!codec) {
        cerr << "Can't find decoder" << endl;
        return false;
    }
    cout << "Codec found: " << codec->long_name << endl;

    codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx){
        cerr << "Can't allocate private data and initialize defaults for the given codec" << endl;
        return false;
    }

    if (avcodec_parameters_to_context(codec_ctx, codec_params) < 0) {
        cerr << "Can't fill the codec context based on the values from the supplied codec parameters." << endl;
        return false;
    }

    if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
        cerr << "Can't open codec" << endl;
        return false;
    } 

    return true;
}

bool GifDecoder::read_next_frame() {

    if (!fmt_ctx || !codec_ctx || !frame || !packet) return false;

    while (av_read_frame(fmt_ctx, packet) >= 0) {
        if (packet->stream_index == video_stream_index) {

            int response = avcodec_send_packet(codec_ctx, packet);

            if (response < 0) {
                cerr << "Can't send packet to decoder" << endl;
                av_packet_unref(packet);
                return false;
            }

            response = avcodec_receive_frame(codec_ctx, frame);

            if (response == 0) {
                av_packet_unref(packet);
                return true;
            }
        }
        av_packet_unref(packet);
    }
    return false;    
}

int GifDecoder::get_width() const { return codec_ctx ? codec_ctx->width : 0; }
int GifDecoder::get_height() const { return codec_ctx ? codec_ctx->height : 0; }
AVFrame* GifDecoder::get_current_frame() const { return frame; }
int GifDecoder::get_fps() const { 
    if(fmt_ctx && video_stream_index >=0 ) {
        AVRational rate = fmt_ctx->streams[video_stream_index]->avg_frame_rate;
        if (rate.den > 0 && rate.num > 0) {
            return rate.num/rate.den;
        }
    } 
    return 15;
}