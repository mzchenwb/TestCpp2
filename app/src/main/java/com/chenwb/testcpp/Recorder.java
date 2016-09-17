package com.chenwb.testcpp;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.text.TextUtils;

import java.io.BufferedOutputStream;
import java.io.DataOutputStream;
import java.io.FileOutputStream;

public class Recorder implements Runnable {
    private static int sampleRateInHz = 44100;
    private static int channelConfig = AudioFormat.CHANNEL_IN_MONO;
    private static int audioEncoding = AudioFormat.ENCODING_PCM_16BIT;

    static {
        System.loadLibrary("avutil");
        System.loadLibrary("postproc");
        System.loadLibrary("swresample");
        System.loadLibrary("swscale");
        System.loadLibrary("avfilter");
        System.loadLibrary("avcodec");
        System.loadLibrary("avformat");
        System.loadLibrary("native-lib");
    }

    public native void init(int sampleRateInHz, int channelConfig, int audioEncoding, String outFile);

    public native void encoding(byte[] buf, int bufSize);

    public native void destroyRecord();

    private boolean isRecording = true;

//    private String audioFile;


    @Override
    public void run() {
        isRecording = true;
        try {
            //开通输出流到指定的文件
//            DataOutputStream dos = new DataOutputStream(new BufferedOutputStream(new FileOutputStream(audioFile)));
            //根据定义好的几个配置，来获取合适的缓冲大小
            int bufferSize = AudioRecord.getMinBufferSize(sampleRateInHz, channelConfig, audioEncoding);
            //实例化AudioRecord
            AudioRecord record = new AudioRecord(MediaRecorder.AudioSource.MIC, sampleRateInHz,
                    channelConfig, audioEncoding, bufferSize);

            //定义缓冲
            byte[] buffer = new byte[bufferSize];

            //开始录制
            record.startRecording();

            //定义循环，根据isRecording的值来判断是否继续录制
            while (isRecording) {
                int bufferReadResult = record.read(buffer, 0, buffer.length);
//                dos.write(buffer, 0, bufferReadResult);
                System.out.println("bufferReadResult = " + bufferReadResult);
                encoding(buffer, bufferReadResult);
            }
            destroyRecord();
            //录制结束
            record.stop();
//            dos.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void startRecord(final String outFilePath) {
        if (TextUtils.isEmpty(outFilePath)) {
            throw new NullPointerException("out file path is empty!");
        }
        new Thread(this).start();
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    Thread.sleep(100);
                    init(sampleRateInHz, channelConfig,audioEncoding, outFilePath);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();

    }

    public void stopRecord() {
        isRecording = false;
    }
}
