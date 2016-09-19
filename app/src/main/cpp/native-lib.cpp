#include "transcode_aac.h"
#include <jni.h>
#include <string>
#include "FFAudioBufferEncoder.hpp"
#include <android/log.h>

#define LOG(...) __android_log_print(ANDROID_LOG_ERROR,"FFAudioBufferEncoder",__VA_ARGS__)

extern "C" {
static FFAudioBufferEncoder *glf_encoder = NULL;

JNIEXPORT jint JNICALL
Java_com_chenwb_testcpp_FFBufferEncoder_startEncode(JNIEnv *env, jobject instance,
                                                    jstring outFilePath_,
                                                    jstring outFileTyp_,
                                                    jint outBitRate) {
    const char *outFilePath = env->GetStringUTFChars(outFilePath_, 0);
    const char *outFileTyp = env->GetStringUTFChars(outFileTyp_, 0);

    FFAudioBufferEncoder *encoder = new FFAudioBufferEncoder(outFilePath, outFileTyp, outBitRate);
    glf_encoder = encoder;
    AVOutputFormat *format = av_guess_format("aac", NULL, NULL);
    if (!format)
        format = av_guess_format(NULL,outFilePath, NULL);
    if (!format)
        format = av_guess_format(NULL,"mp4",  NULL);
    if (!format)
        format = av_guess_format(NULL, "aac", NULL);
    if (!format) {
        LOG("format err");
        return -1;
    }

    int error = glf_encoder->beginInput();
    if (error != 0) {
        LOG("beginInput err %s", &FFAudioHelper::getErrorText(error).front());
        return error;
    }

    env->ReleaseStringUTFChars(outFilePath_, outFilePath);
    env->ReleaseStringUTFChars(outFileTyp_, outFileTyp);
    return 0;
}

JNIEXPORT jint JNICALL
Java_com_chenwb_testcpp_FFBufferEncoder_appendData(JNIEnv *env, jobject instance,
                                                   jbyteArray data_, jint len) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);

//    std::vector<uint8_t> buffer;
//    buffer.resize(4 * 1024);

    size_t readed = 0;
    int error;
//    do {

//        readed = std::min(buffer.size(), (const unsigned int &) len);

//        memcpy(&buffer.front(), data, readed);

//        int samples = readed / 2;
//        int16_t *begin = (int16_t *) &buffer.front();
//        int16_t *end = begin + samples;
//
//        while (begin != end) {
//            int16_t &oneSample = *begin;
//            oneSample = (oneSample << 8) | ((oneSample >> 8) & 0xFF);
//
//            ++begin;
//        }

    error = glf_encoder->appendData((const uint8_t *) data, len);
    if (error != 0) {
        LOG("appendData err %s", &FFAudioHelper::getErrorText(error).front());
        return error;
    }
//    }
//    while (readed == buffer.size());

    env->ReleaseByteArrayElements(data_, data, 0);
    return 0;
}

JNIEXPORT jint JNICALL
Java_com_chenwb_testcpp_FFBufferEncoder_endInput(JNIEnv *env, jobject instance) {
    int error = glf_encoder->endInput();
    if (error != 0) {
        LOG("endInput err %s", &FFAudioHelper::getErrorText(error).front());
        return error;
    }
    delete glf_encoder;
    glf_encoder = NULL;
    return 0;
}

JNIEXPORT void JNICALL
Java_com_chenwb_testcpp_Recorder_init(JNIEnv *env, jobject instance, jint sampleRateInHz,
                                      jint channelConfig, jint audioEncoding, jstring outFile_) {
    const char *outFile = env->GetStringUTFChars(outFile_, 0);

    main2(outFile);

    env->ReleaseStringUTFChars(outFile_, outFile);
}

JNIEXPORT void JNICALL
Java_com_chenwb_testcpp_Recorder_encoding(JNIEnv *env, jobject instance, jbyteArray buf_,
                                          jint bufSize) {
    jbyte *buf = env->GetByteArrayElements(buf_, NULL);

    write_data((uint8_t *) buf, bufSize);

    env->ReleaseByteArrayElements(buf_, buf, 0);
}

JNIEXPORT void JNICALL
Java_com_chenwb_testcpp_Recorder_destroyRecord(JNIEnv *env, jobject instance) {
    destroy();
}

jstring Java_com_chenwb_testcpp_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
}

