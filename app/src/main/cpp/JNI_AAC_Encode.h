//
//  JNI_AAC_Encode.h
//  FFAudioMixing
//
//  Created by chen weibin on 16/9/2.
//  Copyright (c) 2016年 bbo. All rights reserved.
//

#ifndef FFAudioMixing_JNI_AAC_Encode_h
#define FFAudioMixing_JNI_AAC_Encode_h

extern "C" {
#include <libavutil/avstring.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <math.h>
    
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
}

int flush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index);
int start(const char *pcm_file, const char *out_file);
void audio_decode_example(const char *pcm_file, const char *out_file);
#endif
