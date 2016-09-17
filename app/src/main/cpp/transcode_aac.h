//
// Created by chenwb on 16-9-17.
//

#ifndef TESTCPP2_TRANSCODE_AAC_H
#define TESTCPP2_TRANSCODE_AAC_H

#include <cstdint>
extern "C" {
#include <stdio.h>
#include "libavformat/avformat.h"
#include "libavutil/audio_fifo.h"
#include "libavutil/avassert.h"
#include "libavutil/avstring.h"
#include "libswresample/swresample.h"
#include <libavutil/time.h>
}

#include "BuffQueue.h"

void write_data(uint8_t* buf, int buf_size);
int main2(const char *output_file_path);

void destroy();

#endif //TESTCPP2_TRANSCODE_AAC_H
