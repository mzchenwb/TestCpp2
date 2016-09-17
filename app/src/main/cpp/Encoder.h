//
// Created by chenwb on 16-9-17.
//

#ifndef TESTCPP2_ENCODER_H
#define TESTCPP2_ENCODER_H

extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/audio_fifo.h"
#include "libavutil/avassert.h"
#include "libavutil/avstring.h"
#include "libswresample/swresample.h"
};

#include "BuffQueue.h"

class Encoder {
    BuffQueue queue;
};

#endif //TESTCPP2_ENCODER_H
