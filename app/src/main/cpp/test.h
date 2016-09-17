//
// Created by chenwb on 16-9-10.
//

#ifndef TESTCPP2_TEST_H
#define TESTCPP2_TEST_H

extern "C" {
#include <libavutil/avstring.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
};

int start(const char *out_file_path, const char *pcm_file_path);

#endif //TESTCPP2_TEST_H
