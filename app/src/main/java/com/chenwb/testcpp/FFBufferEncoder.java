package com.chenwb.testcpp;

/**
 * Created by chenweibin on 16/9/18.
 */

public class FFBufferEncoder {
    static {
        System.loadLibrary("mp3lame");
        System.loadLibrary("avutil");
        System.loadLibrary("postproc");
        System.loadLibrary("swresample");
        System.loadLibrary("swscale");
        System.loadLibrary("avfilter");
        System.loadLibrary("avcodec");
        System.loadLibrary("avformat");
        System.loadLibrary("native-lib");
    }



    public native int startEncode(String outFilePath, String outFileTyp, int outBitRate);

    public native int appendData(byte[] data, int len);

    public native int endInput();
}
