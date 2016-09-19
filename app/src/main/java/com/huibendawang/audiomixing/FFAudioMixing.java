package com.huibendawang.audiomixing;

public class FFAudioMixing {
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

    public String startAudioMixing(RecordAudio recordAudio) {
        return startAudioMixing(recordAudio.beginEffect,
                recordAudio.endEffect,
                recordAudio.haveIntroPage,
                recordAudio.haveEndingPage,
                recordAudio.voicePages,
                recordAudio.timeSpanSec,
                recordAudio.bkgMusicFile,
                recordAudio.bkgVolume,
                recordAudio.outputFile,
                recordAudio.isM4a);
    }

    public native String startAudioMixing(String beginEffect,
                                          String endEffect,
                                          boolean haveIntroPage,
                                          boolean haveEndingPage,
                                          String[] voicePages,
                                          double timeSpanSec,
                                          String bkgMusicFile,
                                          double bkgVolume,
                                          String outputFile,
                                          boolean isM4a);

    protected void printMessage(String message) {
    }

    public static class RecordAudio {
        public String beginEffect = "";
        public String endEffect = "";
        public boolean haveIntroPage = false;
        public boolean haveEndingPage = false;
        public String[] voicePages;
        public double timeSpanSec = 2;
        public String bkgMusicFile = "";
        public double bkgVolume = 0.2;
        public String outputFile;
        public boolean isM4a;

        public RecordAudio(String[] voicePages, String outputFile) {
            this.voicePages = voicePages;
            this.outputFile = outputFile;
        }
    }
}


