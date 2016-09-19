/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.huibendawang.audiomixing;

import android.app.Activity;
import android.media.MediaPlayer;
import android.os.Environment;
import android.widget.ScrollView;
import android.widget.TextView;
import android.os.Bundle;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class Demo extends Activity {
    private FFAudioMixing mixing;
    private TextView messageText;

    private long lastTime;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        messageText = new TextView(this);
        ScrollView scrollView = new ScrollView(this);
        scrollView.addView(messageText);
        setContentView(scrollView);

        mixing = new FFAudioMixing() {
            @Override
            protected void printMessage(final String message) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        messageText.append(message);
                        messageText.append("\n");
                        System.out.println(message);
                    }
                });
            }
        };

        new Thread(new Runnable() {
            @Override
            public void run() {
                List<String> files = new ArrayList<String>();
                String path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/test2/";
                for (int i = 0; i <= 7; i++) {
                    files.add(path + i);
                }
                lastTime = System.currentTimeMillis();
                String outPath = path + "combine.mp3";
                FFAudioMixing.RecordAudio audio = new FFAudioMixing.RecordAudio(files.toArray(new String[files.size()]), outPath);
                audio.bkgMusicFile = path + "bgm.mp3";
                audio.haveIntroPage = true;
                audio.haveEndingPage = true;
                audio.timeSpanSec = 0.6;
                audio.bkgVolume = 0.2;

                String result = mixing.startAudioMixing(audio);
                result +=  " Use Time = " + (System.currentTimeMillis() - lastTime);

                File file = new File(outPath);
                if (file.exists()) {
                    MediaPlayer mediaPlayer = new MediaPlayer();
                    try {
                        mediaPlayer.reset();
                        mediaPlayer.setDataSource(outPath);
                        mediaPlayer.prepare();
                        result += " audio duration = " + mediaPlayer.getDuration();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }

                final String message = result;
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        messageText.append(message);
                        messageText.append("\n");
                        System.out.println(message);
                    }
                });
            }
        }).start();
    }
}
