#include <string.h>
#include <jni.h>
#include <sstream>
#include "FFAudioMixing.hpp"

extern "C"
{
JNIEXPORT jstring JNICALL
Java_com_huibendawang_audiomixing_FFAudioMixing_startAudioMixing(JNIEnv *env, jobject instance,
                                                                 jstring jBeginEffect,
                                                                 jstring jEndEffect_,
                                                                 jboolean jHaveIntroPage,
                                                                 jboolean jHaveEndingPage,
                                                                 jobjectArray jVoicePages,
                                                                 jdouble jTimeSpanSec,
                                                                 jstring jBkgMusicFile,
                                                                 jdouble jBkgVolume,
                                                                 jstring jOutputFile,
                                                                 jboolean isM4A) {

    //找到java中的类
    jclass cls = env->FindClass("com/huibendawang/audiomixing/FFAudioMixing");
    //再找类中的方法
    jmethodID printMessage = env->GetMethodID(cls, "printMessage", "(Ljava/lang/String;)V");
    if (printMessage == NULL) {
        return env->NewStringUTF("init error");
    }

    jint size = env->GetArrayLength(jVoicePages);
    if (size <= 0) {
        return env->NewStringUTF("File size Empty!");
    }

    env->CallVoidMethod(instance, printMessage, env->NewStringUTF("Start ++++++"));

    const char *beginEffect = env->GetStringUTFChars(jBeginEffect, JNI_FALSE);
    const char *endEffect = env->GetStringUTFChars(jEndEffect_, JNI_FALSE);
    const char *bkgMusicFile = env->GetStringUTFChars(jBkgMusicFile, JNI_FALSE);
    const char *outputFile = env->GetStringUTFChars(jOutputFile, JNI_FALSE);

    std::vector<std::string> inputFiles;
    for (jint i = 0; i < size; i++) {
        jstring strObj = (jstring) env->GetObjectArrayElement(jVoicePages, i);
        const char *chr = env->GetStringUTFChars(strObj, JNI_FALSE);
        inputFiles.push_back(chr);
        env->ReleaseStringUTFChars(strObj, chr);
    }
    std::string beginEffectFile(beginEffect);
    std::string endEffectFile(endEffect);
    std::string backgroundFile(bkgMusicFile);
    std::string outputFileStr(outputFile);

    env->ReleaseStringUTFChars(jBeginEffect, beginEffect);
    env->ReleaseStringUTFChars(jEndEffect_, endEffect);
    env->ReleaseStringUTFChars(jBkgMusicFile, bkgMusicFile);
    env->ReleaseStringUTFChars(jOutputFile, outputFile);

    IFFAudioMixing* audioMixing = FFAudioMixingFactory::createInstance();

    if (isM4A) {
        audioMixing->init(".m4a", 128000);
    } else {
        audioMixing->init();
    }

    env->CallVoidMethod(instance, printMessage, env->NewStringUTF("combineAudios starting ++++++"));
    int err = audioMixing->combineAudios(beginEffectFile,
                                        endEffectFile,
                                        jHaveIntroPage,
                                        jHaveEndingPage,
                                        inputFiles,
                                        jTimeSpanSec,
                                        backgroundFile,
                                        jBkgVolume,
                                        outputFileStr);

    audioMixing->destory();

    env->CallVoidMethod(instance, printMessage, env->NewStringUTF("combineAudios end ------- "));

    char str[15];
    sprintf(str, "result = %d\n", err);
    return env->NewStringUTF(str);
}
}