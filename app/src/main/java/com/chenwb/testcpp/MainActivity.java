package com.chenwb.testcpp;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

public class MainActivity extends AppCompatActivity {

    Recorder recorder;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        recorder = new Recorder();
        findViewById(R.id.record).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                recorder.startRecord("/sdcard/record.m4a");
            }
        });

        findViewById(R.id.stop).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                recorder.stopRecord();
            }
        });
    }


}
