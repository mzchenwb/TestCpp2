#ifndef TESTCPP_BUF_QUEUE_H
#define TESTCPP_BUF_QUEUE_H

#include <memory>
#include <queue>
#include <mutex>
#include <cstring>

typedef std::vector<uint8_t> XBuffer;
typedef std::deque<std::shared_ptr<XBuffer>> XBufferQueue;

class BuffQueue {
public:
    int read_packet_callback(uint8_t *buf_out, int buf_size);
    void write_data(uint8_t *data, size_t size);

private:
    XBufferQueue queue;
    std::mutex mtx;
    void read_one_front(uint8_t *buf_out, size_t buf_size, std::shared_ptr<XBuffer> &q_buffer);
};

#endif //TESTCPP_BUF_QUEUE_H
