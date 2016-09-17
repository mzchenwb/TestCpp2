/*
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * simple audio converter
 *
 * @example transcode_aac.c
 * Convert an input audio file to AAC in an MP4 container using FFmpeg.
 * @author Andreas Unterweger (dustsigns@gmail.com)
 */
#include "transcode_aac.h"

/** The output bit rate in kbit/s */
#define OUTPUT_BIT_RATE 12800
/** The number of output channels */
#define OUTPUT_CHANNELS 1

BuffQueue queue;
bool isEnded = false;

void write_data(uint8_t* buf, int buf_size) {
    queue.write_data(buf, (size_t) buf_size);
}

void destroy() {
    isEnded = true;
}

/**
 * Convert an error code into a text message.
 * @param error Error code to be converted
 * @return Corresponding error text (not thread-safe)
 */
static const char *get_error_text(const int error) {
    static char error_buffer[255];
    av_strerror(error, error_buffer, sizeof(error_buffer));
    return error_buffer;
}

static int read_packet(void *opaque, uint8_t *buf, int buf_size) {
    int read_size = 0;
    do {
        read_size = queue.read_packet_callback(buf, buf_size);
        if (read_size > 0)
            return read_size;
        av_usleep(300);
    } while (read_size <= 0 && !isEnded);
    return read_size;
}

/** Open an input file and the required decoder. */
static int open_input_file(AVFormatContext **input_format_context,
                           AVCodecContext **input_codec_context) {
    AVCodec *input_codec;
    int error;
    AVIOContext *avioContext;

    uint8_t *buffer = NULL, *avio_ctx_buffer = NULL;
    size_t buffer_size, avio_ctx_buffer_size = 4096;

    avio_ctx_buffer = (uint8_t *) av_malloc(avio_ctx_buffer_size);
    if (!avio_ctx_buffer) {
        return AVERROR(ENOMEM);
    }
    avioContext = avio_alloc_context(avio_ctx_buffer, avio_ctx_buffer_size,
                                     0, NULL, &read_packet, NULL, NULL);
    if (!avioContext) {
        return AVERROR(ENOMEM);
    }
    if (!(*input_format_context = avformat_alloc_context())) {
        return AVERROR(ENOMEM);
    }
    (*input_format_context)->pb = avioContext;

    /** Open the input file to read from it. */
    //打开输入文件
    AVInputFormat *fmt = av_find_input_format("s16le");
    if ((error = avformat_open_input(input_format_context, NULL, fmt, NULL)) < 0) {
        fprintf(stderr, "Could not open (error '%s')\n", get_error_text(error));
        *input_format_context = NULL;
        return error;
    }
//    if ((error = avformat_open_input(input_format_context, filename, fmt,
//                                     NULL)) < 0) {
//        fprintf(stderr, "Could not open input file '%s' (error '%s')\n",
//                filename, get_error_text(error));
//        *input_format_context = NULL;
//        return error;
//    }

//    AVInputFormat *pInputFormat = (*input_format_context)->iformat;
//    pInputFormat->name = "s16be";
//    puts("FFMPEG支持的所有的输入文件格式：");
//    while (pInputFormat)
//    {
//        printf("%s ", pInputFormat->name);
//        pInputFormat = pInputFormat->next;
//    }
    /** Get information on the input file (number of streams etc.). */
    //查找流信息
//    if ((error = avformat_find_stream_info(*input_format_context, NULL)) < 0) {
//        fprintf(stderr, "Could not open find stream info (error '%s')\n",
//                get_error_text(error));
//        avformat_close_input(input_format_context);
//        return error;
//    }
//    //这个地方只检测了流的数量，而且并没有确定流的类型
//    //(*input_format_context)->streams[0]->codec->codec_type AVMEDIA_TYPE_AUDIO AVMEDIA_TYPE_VIDEO
//    //所以做测试的时候输入文件必须是一个纯的音频文件，没有的话可以使用ffmpeg转换一个
//    /** Make sure that there is only one stream in the input file. */
//    if ((*input_format_context)->nb_streams != 1) {
//        fprintf(stderr, "Expected one audio input stream, but found %d\n",
//                (*input_format_context)->nb_streams);
//        avformat_close_input(input_format_context);
//        return AVERROR_EXIT;
//    }
//
//    /** Find a decoder for the audio stream. */
//    //查找解码器 参数是解码器ID 解码时解码器的ID保存在 格式上下文->流->编解码器上下文->codec->codec_id
//    if (!(input_codec = avcodec_find_decoder((*input_format_context)->streams[0]->codec->codec_id))) {
//        fprintf(stderr, "Could not find input codec\n");
//        avformat_close_input(input_format_context);
//        return AVERROR_EXIT;
//    }
//
//    /** Open the decoder for the audio stream to use it later. */
//    //打开解码器 参数是编解码器上下文，编解码器，NULL=参数
//    if ((error = avcodec_open2((*input_format_context)->streams[0]->codec,
//                               input_codec, NULL)) < 0) {
//        fprintf(stderr, "Could not open input codec (error '%s')\n",
//                get_error_text(error));
//        avformat_close_input(input_format_context);
//        return error;
//    }
//
//    /** Save the decoder context for easier access later. */
//    *input_codec_context = (*input_format_context)->streams[0]->codec;


    //查找解码器 参数是解码器ID 解码时解码器的ID保存在 格式上下文->流->编解码器上下文->codec->codec_id
    AVCodecContext *codec = (*input_format_context)->streams[0]->codec;
    if (!(input_codec = avcodec_find_decoder(codec->codec_id))) {
        fprintf(stderr, "Could not find input codec\n");
        avformat_close_input(input_format_context);
        return AVERROR_EXIT;
    }

    if ((error = avcodec_open2(codec, input_codec, NULL)) < 0) {
        fprintf(stderr, "Could not open input codec (error '%s')\n",
                get_error_text(error));
        avformat_close_input(input_format_context);
        return error;
    }
    *input_codec_context = codec;
    return 0;
}

/**
 * Open an output file and the required encoder.
 * Also set some basic encoder parameters.
 * Some of these parameters are based on the input file's parameters.
 */
static int open_output_file(const char *filename,
                            AVCodecContext *input_codec_context,
                            AVFormatContext **output_format_context,
                            AVCodecContext **output_codec_context) {
    AVIOContext *output_io_context = NULL;
    AVStream *stream = NULL;
    AVCodec *output_codec = NULL;
    int error;

    /** Open the output file to write to it. */
    //对比例1中的avio_alloc_context，本次是打开文件，并且填充io上下文，而在例1中是直接指定io上下文的读回调函数
    //如果此时不想把通过ffmpeg的方式把数据写入到文件中的话，也可以使用avio_alloc_context去指定写回调函数，就可以随心所欲的处理音频数据了
    //对于输出格式上下文，其成员 io上下文和 输出格式oformat都有写文件函数，他们有什么关系呢？
    //当调用av_write_frame的时候，会先调用oformat里面的写函数，oformat里面的写函数在去调用io上下文的写函数，
    //总结来看io上下文的写 是和文件url有关的（网络，或者本地文件）
    //oformat则是和媒体容器相关的，比如是mp4，flv，或者aac，mp3
    //此处是我自己的理解，不一定正确
    if ((error = avio_open(&output_io_context, filename,
                           AVIO_FLAG_WRITE)) < 0) {
        fprintf(stderr, "Could not open output file '%s' (error '%s')\n",
                filename, get_error_text(error));
        return error;
    }

    /** Create a new format context for the output container format. */
    //创建输出格式上下文
    if (!(*output_format_context = avformat_alloc_context())) {
        fprintf(stderr, "Could not allocate output format context\n");
        return AVERROR(ENOMEM);
    }

    /** Associate the output file (pointer) with the container format context. */
    //指定格式上下文的io上下文
    (*output_format_context)->pb = output_io_context;

    /** Guess the desired container format based on the file extension. */
    //猜测输出格式的上下文，作用，我猜测是在写入文件的时候，写入相应的文件格式的头或者类似的信息，
    //比如mp4的话，就需要写一个个的box，flv则是tag，其他的容器也有自己的数据格式
    if (!((*output_format_context)->oformat = av_guess_format(NULL, filename,
                                                              NULL))) {
        fprintf(stderr, "Could not find output file format\n");
        goto cleanup;
    }

    av_strlcpy((*output_format_context)->filename, filename,
               sizeof((*output_format_context)->filename));

    /** Find the encoder to be used by its name. */
    //查找编码器
    if (!(output_codec = avcodec_find_encoder(AV_CODEC_ID_AAC))) {
        fprintf(stderr, "Could not find an AAC encoder.\n");
        goto cleanup;
    }

    /** Create a new audio stream in the output file container. */
    //新建一个流
    if (!(stream = avformat_new_stream(*output_format_context, output_codec))) {
        fprintf(stderr, "Could not create new stream\n");
        error = AVERROR(ENOMEM);
        goto cleanup;
    }

    /** Save the encoder context for easier access later. */
    *output_codec_context = stream->codec;

    /**
     * Set the basic encoder parameters.
     * The input file's sample rate is used to avoid a sample rate conversion.
     */
    //编码器初始化
    (*output_codec_context)->channels = OUTPUT_CHANNELS;//声道数
    (*output_codec_context)->channel_layout = av_get_default_channel_layout(OUTPUT_CHANNELS);//声道格式
    (*output_codec_context)->sample_rate = input_codec_context->sample_rate;//采样率
    (*output_codec_context)->sample_fmt = output_codec->sample_fmts[0];//采样格式
    (*output_codec_context)->bit_rate = OUTPUT_BIT_RATE;//码率

    /** Allow the use of the experimental AAC encoder */
    (*output_codec_context)->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;

    /** Set the sample rate for the container. */
    stream->time_base.den = input_codec_context->sample_rate;
    stream->time_base.num = 1;

    /**
     * Some container formats (like MP4) require global headers to be present
     * Mark the encoder so that it behaves accordingly.
     */
    if ((*output_format_context)->oformat->flags & AVFMT_GLOBALHEADER)
        (*output_codec_context)->flags |= CODEC_FLAG_GLOBAL_HEADER;

    /** Open the encoder for the audio stream to use it later. */
    //打开编码器
    if ((error = avcodec_open2(*output_codec_context, output_codec, NULL)) < 0) {
        fprintf(stderr, "Could not open output codec (error '%s')\n",
                get_error_text(error));
        goto cleanup;
    }

    return 0;

    cleanup:
    avio_closep(&(*output_format_context)->pb);
    avformat_free_context(*output_format_context);
    *output_format_context = NULL;
    return error < 0 ? error : AVERROR_EXIT;
}

/** Initialize one data packet for reading or writing. */
static void init_packet(AVPacket *packet) {
    av_init_packet(packet);
    /** Set the packet data and size so that it is recognized as being empty. */
    packet->data = NULL;
    packet->size = 0;
}

/** Initialize one audio frame for reading from the input file */
static int init_input_frame(AVFrame **frame) {
    if (!(*frame = av_frame_alloc())) {
        fprintf(stderr, "Could not allocate input frame\n");
        return AVERROR(ENOMEM);
    }
    return 0;
}

/**
 * Initialize the audio resampler based on the input and output codec settings.
 * If the input and output sample formats differ, a conversion is required
 * libswresample takes care of this, but requires initialization.
 */
static int init_resampler(AVCodecContext *input_codec_context,
                          AVCodecContext *output_codec_context,
                          SwrContext **resample_context) {
    int error;

    /**
     * Create a resampler context for the conversion.
     * Set the conversion parameters.
     * Default channel layouts based on the number of channels
     * are assumed for simplicity (they are sometimes not detected
     * properly by the demuxer and/or decoder).
     */
    //区别于例三，一个函数搞定重采样上下文的初始化
    *resample_context = swr_alloc_set_opts(NULL,
                                           av_get_default_channel_layout(
                                                   output_codec_context->channels),
                                           output_codec_context->sample_fmt,
                                           output_codec_context->sample_rate,
                                           av_get_default_channel_layout(
                                                   input_codec_context->channels),
                                           input_codec_context->sample_fmt,
                                           input_codec_context->sample_rate,
                                           0, NULL);
    if (!*resample_context) {
        fprintf(stderr, "Could not allocate resample context\n");
        return AVERROR(ENOMEM);
    }
    /**
    * Perform a sanity check so that the number of converted samples is
    * not greater than the number of samples to be converted.
    * If the sample rates differ, this case has to be handled differently
    */
    //确保输入输出采样率相等，猜测：如果采样率不相等的话，采样数就需要重新计算，但是此段代码中并没有重新计算采样数
    av_assert0(output_codec_context->sample_rate == input_codec_context->sample_rate);

    /** Open the resampler with the specified parameters. */
    if ((error = swr_init(*resample_context)) < 0) {
        fprintf(stderr, "Could not open resample context\n");
        swr_free(resample_context);
        return error;
    }
    return 0;
}

/** Initialize a FIFO buffer for the audio samples to be encoded. */
static int init_fifo(AVAudioFifo **fifo, AVCodecContext *output_codec_context) {
    /** Create the FIFO buffer based on the specified output sample format. */
    if (!(*fifo = av_audio_fifo_alloc(output_codec_context->sample_fmt,
                                      output_codec_context->channels, 1))) {
        fprintf(stderr, "Could not allocate FIFO\n");
        return AVERROR(ENOMEM);
    }
    return 0;
}

/** Write the header of the output file container. */
static int write_output_file_header(AVFormatContext *output_format_context) {
    int error;
    //写文件头
    if ((error = avformat_write_header(output_format_context, NULL)) < 0) {
        fprintf(stderr, "Could not write output file header (error '%s')\n",
                get_error_text(error));
        return error;
    }
    return 0;
}

/** Decode one audio frame from the input file. */
static int decode_audio_frame(AVFrame *frame,
                              AVFormatContext *input_format_context,
                              AVCodecContext *input_codec_context,
                              int *data_present, int *finished) {
    /** Packet used for temporary storage. */
    AVPacket input_packet;
    int error;
    init_packet(&input_packet);

    /** Read one audio frame from the input file into a temporary packet. */
    if ((error = av_read_frame(input_format_context, &input_packet)) < 0) {
        /** If we are at the end of the file, flush the decoder below. */
        if (error == AVERROR_EOF)
            *finished = 1;
        else {
            fprintf(stderr, "Could not read frame (error '%s')\n",
                    get_error_text(error));
            return error;
        }
    }

    /**
     * Decode the audio frame stored in the temporary packet.
     * The input audio stream decoder is used to do this.
     * If we are at the end of the file, pass an empty packet to the decoder
     * to flush it.
     */
    if ((error = avcodec_decode_audio4(input_codec_context, frame,
                                       data_present, &input_packet)) < 0) {
        fprintf(stderr, "Could not decode frame (error '%s')\n",
                get_error_text(error));
        av_free_packet(&input_packet);
        return error;
    }

    /**
     * If the decoder has not been flushed completely, we are not finished,
     * so that this function has to be called again.
     */
    if (*finished && *data_present)
        *finished = 0;
    av_free_packet(&input_packet);
    return 0;
}

/**
 * Initialize a temporary storage for the specified number of audio samples.
 * The conversion requires temporary storage due to the different format.
 * The number of audio samples to be allocated is specified in frame_size.
 */
static int init_converted_samples(uint8_t ***converted_input_samples,
                                  AVCodecContext *output_codec_context,
                                  int frame_size) {
    int error;

    /**
     * Allocate as many pointers as there are audio channels.
     * Each pointer will later point to the audio samples of the corresponding
     * channels (although it may be NULL for interleaved formats).
     */
    if (!(*converted_input_samples = (uint8_t **) calloc(output_codec_context->channels,
                                                         sizeof(**converted_input_samples)))) {
        fprintf(stderr, "Could not allocate converted input sample pointers\n");
        return AVERROR(ENOMEM);
    }

    /**
     * Allocate memory for the samples of all channels in one consecutive
     * block for convenience.
     */
    if ((error = av_samples_alloc(*converted_input_samples, NULL,
                                  output_codec_context->channels,
                                  frame_size,
                                  output_codec_context->sample_fmt, 0)) < 0) {
        fprintf(stderr,
                "Could not allocate converted input samples (error '%s')\n",
                get_error_text(error));
        av_freep(&(*converted_input_samples)[0]);
        free(*converted_input_samples);
        return error;
    }
    return 0;
}

/**
 * Convert the input audio samples into the output sample format.
 * The conversion happens on a per-frame basis, the size of which is specified
 * by frame_size.
 */
static int convert_samples(const uint8_t **input_data,
                           uint8_t **converted_data, const int frame_size,
                           SwrContext *resample_context) {
    int error;

    /** Convert the samples using the resampler. */
    if ((error = swr_convert(resample_context,
                             converted_data, frame_size,
                             input_data, frame_size)) < 0) {
        fprintf(stderr, "Could not convert input samples (error '%s')\n",
                get_error_text(error));
        return error;
    }

    return 0;
}

/** Add converted input audio samples to the FIFO buffer for later processing. */
static int add_samples_to_fifo(AVAudioFifo *fifo,
                               uint8_t **converted_input_samples,
                               const int frame_size) {
    int error;

    /**
     * Make the FIFO as large as it needs to be to hold both,
     * the old and the new samples.
     */
    if ((error = av_audio_fifo_realloc(fifo, av_audio_fifo_size(fifo) + frame_size)) < 0) {
        fprintf(stderr, "Could not reallocate FIFO\n");
        return error;
    }

    /** Store the new samples in the FIFO buffer. */
    if (av_audio_fifo_write(fifo, (void **) converted_input_samples,
                            frame_size) < frame_size) {
        fprintf(stderr, "Could not write data to FIFO\n");
        return AVERROR_EXIT;
    }
    return 0;
}

/**
 * Read one audio frame from the input file, decodes, converts and stores
 * it in the FIFO buffer.
 */
static int read_decode_convert_and_store(AVAudioFifo *fifo,
                                         AVFormatContext *input_format_context,
                                         AVCodecContext *input_codec_context,
                                         AVCodecContext *output_codec_context,
                                         SwrContext *resampler_context,
                                         int *finished) {
    /** Temporary storage of the input samples of the frame read from the file. */
    AVFrame *input_frame = NULL;
    /** Temporary storage for the converted input samples. */
    uint8_t **converted_input_samples = NULL;
    int data_present;
    int ret = AVERROR_EXIT;

    /** Initialize temporary storage for one input frame. */
    if (init_input_frame(&input_frame))
        goto cleanup;
    /** Decode one frame worth of audio samples. */
    if (decode_audio_frame(input_frame, input_format_context,
                           input_codec_context, &data_present, finished))
        goto cleanup;
    /**
     * If we are at the end of the file and there are no more samples
     * in the decoder which are delayed, we are actually finished.
     * This must not be treated as an error.
     */
    if (*finished && !data_present) {
        ret = 0;
        goto cleanup;
    }
    /** If there is decoded data, convert and store it */
    //input_frame->nb_samples表示当前帧内数据的长度 MP3一般是1152 aac=1024
    //区别于编解码器上下文中的frame_size是帧最大长度
    if (data_present) {
        /** Initialize the temporary storage for the converted input samples. */
        if (init_converted_samples(&converted_input_samples, output_codec_context,
                                   input_frame->nb_samples))
            goto cleanup;

        /**
         * Convert the input samples to the desired output sample format.
         * This requires a temporary storage provided by converted_input_samples.
         */
        if (convert_samples((const uint8_t **) input_frame->extended_data, converted_input_samples,
                            input_frame->nb_samples, resampler_context))
            goto cleanup;

        /** Add the converted input samples to the FIFO buffer for later processing. */
        if (add_samples_to_fifo(fifo, converted_input_samples,
                                input_frame->nb_samples))
            goto cleanup;
        ret = 0;
    }
    ret = 0;

    cleanup:
    if (converted_input_samples) {
        av_freep(&converted_input_samples[0]);
        free(converted_input_samples);
    }
    av_frame_free(&input_frame);

    return ret;
}

/**
 * Initialize one input frame for writing to the output file.
 * The frame will be exactly frame_size samples large.
 */
static int init_output_frame(AVFrame **frame,
                             AVCodecContext *output_codec_context,
                             int frame_size) {
    int error;

    /** Create a new frame to store the audio samples. */
    if (!(*frame = av_frame_alloc())) {
        fprintf(stderr, "Could not allocate output frame\n");
        return AVERROR_EXIT;
    }

    /**
     * Set the frame's parameters, especially its size and format.
     * av_frame_get_buffer needs this to allocate memory for the
     * audio samples of the frame.
     * Default channel layouts based on the number of channels
     * are assumed for simplicity.
     */
    (*frame)->nb_samples = frame_size;
    (*frame)->channel_layout = output_codec_context->channel_layout;
    (*frame)->format = output_codec_context->sample_fmt;
    (*frame)->sample_rate = output_codec_context->sample_rate;

    /**
     * Allocate the samples of the created frame. This call will make
     * sure that the audio frame can hold as many samples as specified.
     */
    if ((error = av_frame_get_buffer(*frame, 0)) < 0) {
        fprintf(stderr, "Could allocate output frame samples (error '%s')\n",
                get_error_text(error));
        av_frame_free(frame);
        return error;
    }

    return 0;
}

/** Global timestamp for the audio frames */
static int64_t pts = 0;

/** Encode one frame worth of audio to the output file. */
static int encode_audio_frame(AVFrame *frame,
                              AVFormatContext *output_format_context,
                              AVCodecContext *output_codec_context,
                              int *data_present) {
    /** Packet used for temporary storage. */
    AVPacket output_packet;
    int error;
    init_packet(&output_packet);

    /** Set a timestamp based on the sample rate for the container. */
    if (frame) {
        frame->pts = pts;
        pts += frame->nb_samples;
    }

    /**
     * Encode the audio frame and store it in the temporary packet.
     * The output audio stream encoder is used to do this.
     */
    if ((error = avcodec_encode_audio2(output_codec_context, &output_packet,
                                       frame, data_present)) < 0) {
        fprintf(stderr, "Could not encode frame (error '%s')\n",
                get_error_text(error));
        av_free_packet(&output_packet);
        return error;
    }

    /** Write one audio frame from the temporary packet to the output file. */
    if (*data_present) {
        if ((error = av_write_frame(output_format_context, &output_packet)) < 0) {
            fprintf(stderr, "Could not write frame (error '%s')\n",
                    get_error_text(error));
            av_free_packet(&output_packet);
            return error;
        }

        av_free_packet(&output_packet);
    }

    return 0;
}

/**
 * Load one audio frame from the FIFO buffer, encode and write it to the
 * output file.
 */
static int load_encode_and_write(AVAudioFifo *fifo,
                                 AVFormatContext *output_format_context,
                                 AVCodecContext *output_codec_context) {
    /** Temporary storage of the output samples of the frame written to the file. */
    AVFrame *output_frame;
    /**
     * Use the maximum number of possible samples per frame.
     * If there is less than the maximum possible frame size in the FIFO
     * buffer use this number. Otherwise, use the maximum possible frame size
     */
    const int frame_size = FFMIN(av_audio_fifo_size(fifo),
                                 output_codec_context->frame_size);
    int data_written;

    /** Initialize temporary storage for one output frame. */
    if (init_output_frame(&output_frame, output_codec_context, frame_size))
        return AVERROR_EXIT;

    /**
     * Read as many samples from the FIFO buffer as required to fill the frame.
     * The samples are stored in the frame temporarily.
     */
    if (av_audio_fifo_read(fifo, (void **) output_frame->data, frame_size) < frame_size) {
        fprintf(stderr, "Could not read data from FIFO\n");
        av_frame_free(&output_frame);
        return AVERROR_EXIT;
    }

    /** Encode one frame worth of audio samples. */
    if (encode_audio_frame(output_frame, output_format_context,
                           output_codec_context, &data_written)) {
        av_frame_free(&output_frame);
        return AVERROR_EXIT;
    }
    av_frame_free(&output_frame);
    return 0;
}

/** Write the trailer of the output file container. */
static int write_output_file_trailer(AVFormatContext *output_format_context) {
    int error;
    if ((error = av_write_trailer(output_format_context)) < 0) {
        fprintf(stderr, "Could not write output file trailer (error '%s')\n",
                get_error_text(error));
        return error;
    }
    return 0;
}

/** Convert an audio file to an AAC file in an MP4 container. */
int main2(const char *output_file_path) {
    //输入输出格式上下文
    AVFormatContext *input_format_context = NULL, *output_format_context = NULL;
    //解码器上下文、编码器上下文
    AVCodecContext *input_codec_context = NULL, *output_codec_context = NULL;
    //重采样上下文
    SwrContext *resample_context = NULL;
    //音频fifo
    AVAudioFifo *fifo = NULL;
    int ret = AVERROR_EXIT;

//    const char * input_file_path = "/home/chenwb/reverseme.pcm";
//    const char * output_file_path = "/home/chenwb/out.m4a";

    /** Register all codecs and formats so that they can be used. */
    av_register_all();
    /** Open the input file for reading. */
    //填充了输入格式上下文，并且打开了相对应的解码器
    if (open_input_file(&input_format_context, &input_codec_context))
        goto cleanup;
    /** Open the output file for writing. */
    //填充输出格式上下文，打开io上下文，流的初始化，编码器的初始化
    if (open_output_file(output_file_path, input_codec_context,
                         &output_format_context, &output_codec_context))
        goto cleanup;
    /** Initialize the resampler to be able to convert audio sample formats. */
    //初始化重采样上下文
    if (init_resampler(input_codec_context, output_codec_context,
                       &resample_context))
        goto cleanup;
    /** Initialize the FIFO buffer to store audio samples to be encoded. */
    //初始化音频fifo
    if (init_fifo(&fifo, output_codec_context))
        goto cleanup;
    /** Write the header of the output file container. */
    if (write_output_file_header(output_format_context))
        goto cleanup;

    /**
     * Loop as long as we have input samples to read or output samples
     * to write; abort as soon as we have neither.
     */
    while (1) {
        /** Use the encoder's desired frame size for processing. */
        const int output_frame_size = output_codec_context->frame_size;
        int finished = 0;

        /**
         * Make sure that there is one frame worth of samples in the FIFO
         * buffer so that the encoder can do its work.
         * Since the decoder's and the encoder's frame size may differ, we
         * need to FIFO buffer to store as many frames worth of input samples
         * that they make up at least one frame worth of output samples.
         */
        //检查fifo里面的数据是否大于要输出的帧的大小，如果不大于，那么将解码转换的数据存放的fifo中
        while (av_audio_fifo_size(fifo) < output_frame_size) {
            /**
             * Decode one frame worth of audio samples, convert it to the
             * output sample format and put it into the FIFO buffer.
             */
            if (read_decode_convert_and_store(fifo, input_format_context,
                                              input_codec_context,
                                              output_codec_context,
                                              resample_context, &finished))
                goto cleanup;

            /**
             * If we are at the end of the input file, we continue
             * encoding the remaining audio samples to the output file.
             */
            if (finished)
                break;
        }

        /**
         * If we have enough samples for the encoder, we encode them.
         * At the end of the file, we pass the remaining samples to
         * the encoder.
         */
        //检查fifo数据是否大于输出帧的大小，或者（到文件末尾并且fifo数据长度大于0）
        while (av_audio_fifo_size(fifo) >= output_frame_size ||
               (finished && av_audio_fifo_size(fifo) > 0))
            /**
             * Take one frame worth of audio samples from the FIFO buffer,
             * encode it and write it to the output file.
             */
            //读fifo，编码，写入文件
            if (load_encode_and_write(fifo, output_format_context,
                                      output_codec_context))
                goto cleanup;

        /**
         * If we are at the end of the input file and have encoded
         * all remaining samples, we can exit this loop and finish.
         */
        if (finished) {
            int data_written;
            /** Flush the encoder as it may have delayed frames. */
            //刷新编码器中可能存在的延时帧
            do {
                if (encode_audio_frame(NULL, output_format_context,
                                       output_codec_context, &data_written))
                    goto cleanup;
            } while (data_written);
            break;
        }
    }

    /** Write the trailer of the output file container. */
    //写文件尾
    if (write_output_file_trailer(output_format_context))
        goto cleanup;
    ret = 0;

    cleanup:
    if (fifo)
        av_audio_fifo_free(fifo);
    swr_free(&resample_context);
    if (output_codec_context)
        avcodec_close(output_codec_context);
    if (output_format_context) {
        avio_closep(&output_format_context->pb);
        avformat_free_context(output_format_context);
    }
    if (input_codec_context)
        avcodec_close(input_codec_context);
    if (input_format_context)
        avformat_close_input(&input_format_context);

    return ret;
}

