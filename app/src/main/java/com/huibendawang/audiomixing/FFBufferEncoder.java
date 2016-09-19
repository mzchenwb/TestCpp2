package com.huibendawang.audiomixing;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.util.Log;

public class FFBufferEncoder {
    static {
        System.loadLibrary("mp3lame");
        System.loadLibrary("avutil");
        System.loadLibrary("postproc");
        System.loadLibrary("swresample");
        System.loadLibrary("swscale");
        System.loadLibrary("avcodec");
        System.loadLibrary("avformat");
        System.loadLibrary("avfilter");
        System.loadLibrary("native-lib");
    }

    private native int startEncode(String outFilePath, String outFileTyp, int outBitRate);

    private native int appendData(byte[] data, int len);

    private native int endInput();

    private boolean isRecording = false;

    private int amplitude = 0;

    public void record(String outputPath) {
        int frequency = 44100;
        int channelConfiguration = AudioFormat.CHANNEL_IN_MONO;
        int audioEncoding = AudioFormat.ENCODING_PCM_16BIT;

        int error = startEncode(outputPath, ".mp4", 128000);
        if (error > 0) {
            Log.e("tag", "startEncode");
            return;
        }
        Log.d("tag", "start ++++++++");

        try {
            // Create a new AudioRecord object to record the audio.
            int bufferSize = AudioRecord.getMinBufferSize(frequency, channelConfiguration, audioEncoding);
            //实例化AudioRecord
            AudioRecord record = new AudioRecord(MediaRecorder.AudioSource.MIC, frequency,
                    channelConfiguration, audioEncoding, bufferSize);

            //定义缓冲
            byte[] buffer = new byte[bufferSize];

            //开始录制
            record.startRecording();
            isRecording = true;

            //定义循环，根据isRecording的值来判断是否继续录制
            while (isRecording) {
                int bufferReadResult = record.read(buffer, 0, buffer.length);
                error = appendData(buffer, bufferReadResult);
                if (error != 0) {
                    Log.e("AudioRecord", "appendData Failed");
                    break;
                }
                amplitude = buffer[buffer.length / 2];
                Log.d("tag", "buff result = " + bufferReadResult + " buffer size = " + bufferSize + " amplitude = " + amplitude);
            }

            record.stop();
        } catch (Throwable t) {
            Log.e("AudioRecord", "Recording Failed");
        } finally {
            endInput();
        }
    }

    public int getCurAmplitude() {
        return amplitude;
    }

    public void stopRecord() {
        isRecording = false;
    }

    public boolean isRecording() {
        return isRecording;
    }
}
