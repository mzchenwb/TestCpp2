#include <jni.h>
#include <string>
#include "transcode_aac.h"

extern "C" {

JNIEXPORT void JNICALL
Java_com_chenwb_testcpp_Recorder_init(JNIEnv *env, jobject instance, jint sampleRateInHz,
                                      jint channelConfig, jint audioEncoding, jstring outFile_) {
    const char *outFile = env->GetStringUTFChars(outFile_, 0);

//    main2(outFile);

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

