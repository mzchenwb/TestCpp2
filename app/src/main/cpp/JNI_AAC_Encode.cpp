#include "JNI_AAC_Encode.h"
#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096

int flush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index) {
    int ret;
    int got_frame;
    AVPacket enc_pkt;
    if (!(fmt_ctx->streams[stream_index]->codec->codec->capabilities &
          CODEC_CAP_DELAY))
        return 0;
    while (1) {
        enc_pkt.data = NULL;
        enc_pkt.size = 0;
        av_init_packet(&enc_pkt);
        ret = avcodec_encode_audio2(fmt_ctx->streams[stream_index]->codec, &enc_pkt,
                                    NULL, &got_frame);
        av_frame_free(NULL);
        if (ret < 0)
            break;
        if (!got_frame) {
            ret = 0;
            break;
        }
        printf("Flush Encoder: Succeed to encode 1 frame!\tsize:%5d\n", enc_pkt.size);
        /* mux encoded frame */
        ret = av_write_frame(fmt_ctx, &enc_pkt);
        if (ret < 0)
            break;
    }
    return ret;
}

int start(const char *pcm_file, const char *out_file) {
    AVFormatContext *pOutFormatCtx;
    AVOutputFormat *fmt;
    AVStream *audio_st;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;

    uint8_t *frame_buf;
    AVFrame *pFrame;
    AVPacket pkt;

    int got_frame = 0;
    int ret = 0;
    int size = 0;

    av_register_all();

    //Method 1.
    pOutFormatCtx = avformat_alloc_context();
    fmt = av_guess_format(NULL, out_file, NULL);
    pOutFormatCtx->oformat = fmt;


    //Method 2.
//    avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, out_file);
//    fmt = pFormatCtx->oformat;

    //Open output URL
    if (avio_open(&pOutFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE) < 0) {
        printf("Failed to open output file!\n");
        return -1;
    }

    audio_st = avformat_new_stream(pOutFormatCtx, 0);
    if (audio_st == NULL) {
        return -1;
    }
    
    AVCodec* codec = avcodec_find_encoder(pOutFormatCtx->oformat->audio_codec);
    
    pCodecCtx = audio_st->codec;
    pCodecCtx->codec_id = fmt->audio_codec;
    pCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
    pCodecCtx->sample_fmt = codec->sample_fmts[0];
    pCodecCtx->sample_rate = 44100;
    pCodecCtx->channel_layout = AV_CH_LAYOUT_MONO;
    pCodecCtx->channels = 1;
    pCodecCtx->bit_rate = 128000;

    //Show some information
    av_dump_format(pOutFormatCtx, 0, out_file, 1);

    pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
    if (!pCodec) {
        printf("Can not find encoder!\n");
        return -1;
    }
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        printf("Failed to open encoder!\n");
        return -1;
    }
    pFrame = av_frame_alloc();
    pFrame->nb_samples = pCodecCtx->frame_size;
    pFrame->format = pCodecCtx->sample_fmt;
//    av_frame_get_buffer(pFrame, 0);

    size = av_samples_get_buffer_size(NULL, pCodecCtx->channels, pCodecCtx->frame_size,
                                      pCodecCtx->sample_fmt, 0);
    frame_buf = (uint8_t *) av_malloc(size);
    ret = avcodec_fill_audio_frame(pFrame, pCodecCtx->channels, pCodecCtx->sample_fmt,
                                      frame_buf, size, 0);

    if (ret < 0) {
        printf("Failed avcodec_fill_audio_frame");
        return -1;
    }

    //Write Header
    ret = avformat_write_header(pOutFormatCtx, NULL);
    if (ret < 0) {
        printf("Failed avformat_write_header");
        return -1;
    }

    av_new_packet(&pkt, size);
    
    
    int frameNum = 1000;                   //Audio frame number
    FILE *in_file = fopen(pcm_file, "rb");//Raw PCM data
    
    /* find the mpeg audio decoder */
    AVCodec *input_codec;
    AVCodecContext *input_decode_ctx= NULL;
    codec = avcodec_find_decoder(AV_CODEC_ID_MP2);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }
    
    input_decode_ctx = avcodec_alloc_context3(input_codec);
    if (!input_decode_ctx) {
        fprintf(stderr, "Could not allocate audio codec context\n");
        exit(1);
    }
    
    /* open it */
    if (avcodec_open2(input_decode_ctx, input_codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }
    
    for (int i = 0; i < frameNum; i++) {
        //Read PCM
        if (fread(frame_buf, 1, size, in_file) <= 0) {
            printf("Failed to read raw data! \n");
            return -1;
        } else if (feof(in_file)) {
            break;
        }
//        pFrame->data[0] = frame_buf;  //PCM Data
//
        ret = avcodec_fill_audio_frame(pFrame, pCodecCtx->channels, pCodecCtx->sample_fmt,
                                       (const uint8_t *) frame_buf, size, 1);
//        pFrame->pts = i * 100;

        got_frame = 0;
        //Encode
        ret = avcodec_encode_audio2(pCodecCtx, &pkt, pFrame, &got_frame);
        
        if (ret < 0) {
            char ch;
            av_strerror(ret, &ch, 256);
            printf("Failed to encode char = %c!\n", ch);
            return -1;
        }
        if (got_frame == 1) {
            printf("Succeed to encode 1 frame! \tsize:%5d\n", pkt.size);
            pkt.stream_index = audio_st->index;
            ret = av_write_frame(pOutFormatCtx, &pkt);
            av_packet_unref(&pkt);
        }
    }
    

    //Flush Encoder
    ret = flush_encoder(pOutFormatCtx, 0);
    if (ret < 0) {
        printf("Flushing encoder failed\n");
        return -1;
    }

    //Write Trailer
    av_write_trailer(pOutFormatCtx);

    //Clean
    if (audio_st) {
        avcodec_close(audio_st->codec);
        av_free(pFrame);
        av_free(frame_buf);
    }
    avio_close(pOutFormatCtx->pb);
    avformat_free_context(pOutFormatCtx);

    fclose(in_file);

    return 0;
}
