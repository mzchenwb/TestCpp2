package com.huibendawang.audiomixing;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaPlayer;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import com.chenwb.testcpp.R;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class RecordActivity extends Activity {
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
                        String outPath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/out_encode.mp4";
                        mEncoder.record(outPath);
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
                mEncoder.stopRecord();
                findViewById(R.id.start_bt).setEnabled(true);
                findViewById(R.id.end_bt).setEnabled(false);
            }

        });

    }

    public void play() {
        MediaPlayer player = new MediaPlayer();
        try {
            player.setDataSource(Environment.getExternalStorageDirectory().getAbsolutePath() + "/out_encode.mp4");
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
//        // Get the file we want to playback.
//        File file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/out_encode.m4a");
//        // Get the length of the audio stored in the file (16 bit so 2 bytes per short)
//        // and create a short array to store the recorded audio.
//        int musicLength = (int) (file.length() / 2);
//        short[] music = new short[musicLength];
//
//
//        try {
//            // Create a DataInputStream to read the audio data back from the saved file.
//            InputStream is = new FileInputStream(file);
//            BufferedInputStream bis = new BufferedInputStream(is);
//            DataInputStream dis = new DataInputStream(bis);
//
//            // Read the file into the music array.
//            int i = 0;
//            while (dis.available() > 0) {
//                music[i] = dis.readShort();
//                i++;
//            }
//
//
//            // Close the input streams.
//            dis.close();
//
//
//            // Create a new AudioTrack object using the same parameters as the AudioRecord
//            // object used to create the file.
//            AudioTrack audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC,
//                    44100,
//                    AudioFormat.CHANNEL_OUT_MONO,
//                    AudioFormat.ENCODING_PCM_16BIT,
//                    musicLength * 2,
//                    AudioTrack.MODE_STREAM);
//            // Start playback
//            audioTrack.play();
//
//            // Write the music buffer to the AudioTrack object
//            audioTrack.write(music, 0, musicLength);
//
//            audioTrack.stop();
//
//        } catch (Throwable t) {
//            Log.e("AudioTrack", "Playback Failed");
//        }
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
