package com.chenwb.testcpp;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaPlayer;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import java.io.IOException;

public class RecordActivity extends Activity {
    private boolean isRecording = false;
    private FFBufferEncoder mEncoder;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.record);

        mEncoder = new FFBufferEncoder();

        Button start = (Button) findViewById(R.id.start_bt);
        start.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View arg0) {
                Thread thread = new Thread(new Runnable() {
                    public void run() {
                        record();
                    }
                });
                thread.start();
                findViewById(R.id.start_bt).setEnabled(false);
                findViewById(R.id.end_bt).setEnabled(true);
            }

        });

        Button play = (Button) findViewById(R.id.play_bt);
        play.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                play();
            }

        });

        Button stop = (Button) findViewById(R.id.end_bt);
        stop.setEnabled(false);
        stop.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                isRecording = false;
                findViewById(R.id.start_bt).setEnabled(true);
                findViewById(R.id.end_bt).setEnabled(false);
            }

        });

    }

    public void play() {
        MediaPlayer player = new MediaPlayer();
        try {
            player.setDataSource(Environment.getExternalStorageDirectory().getAbsolutePath() + "/out_encode.m4a");
            player.prepare();
            player.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
                @Override
                public void onCompletion(MediaPlayer mp) {
                    mp.release();
                }
            });
            player.start();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void record() {
        int frequency = 44100;
        int channelConfiguration = AudioFormat.CHANNEL_IN_MONO;
        int audioEncoding = AudioFormat.ENCODING_PCM_16BIT;

        int error = mEncoder.startEncode(Environment.getExternalStorageDirectory().getAbsolutePath() + "/out_encode.m4a", ".m4a", 128000);
        if (error > 0) {
            Log.d("tag", "startEncode");
            return;
        }
        Log.d("tag", "start ++++++++");

        try {
            // Create a new AudioRecord object to record the audio.
            int bufferSize = AudioRecord.getMinBufferSize(frequency, channelConfiguration, audioEncoding);

            AudioRecord audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC,
                    frequency, channelConfiguration,
                    audioEncoding, bufferSize);

            byte[] buffer = new byte[bufferSize];
            audioRecord.startRecording();

            isRecording = true;
            boolean isFirstReading = true;

            while (isRecording) {
                if (isFirstReading){
                    Log.d("tag", "before buff read bufferSize = " + bufferSize);
                    int bufferReadResult = audioRecord.read(buffer, 0, 1);
                    if (bufferReadResult > 0) {
                        isFirstReading = false;
                        Log.d("tag", "first buff result = " + bufferReadResult);
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                popTip();
                            }
                        });
                        mEncoder.appendData(buffer, 1);
                    }
                }
                int bufferReadResult = audioRecord.read(buffer, 0, bufferSize);
                Log.d("tag", "buff result = " + bufferReadResult + " buffer size = " + bufferSize);
                mEncoder.appendData(buffer, bufferReadResult);
            }


            audioRecord.stop();
            mEncoder.endInput();

        } catch (Throwable t) {
            Log.e("AudioRecord", "Recording Failed");
        }
    }

    private void popTip() {
        new AlertDialog.Builder(this).setMessage("tt").setPositiveButton("ok", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            }
        }).create().show();
    }
}
