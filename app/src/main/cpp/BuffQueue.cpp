#include "BuffQueue.h"

void BuffQueue::read_one_front(uint8_t *buf_out, size_t buf_size, std::shared_ptr<XBuffer> &q_buffer) {
    memcpy(buf_out, &q_buffer->front(), buf_size);

    if (q_buffer->size() <= buf_size)
        return;

    XBuffer *newBuffer = new XBuffer();
    newBuffer->resize(q_buffer->size() - buf_size);
    memcpy(&newBuffer->front(), &q_buffer->front() + buf_size, newBuffer->size());

    mtx.lock();
    queue.pop_front();
    queue.push_front(std::shared_ptr<XBuffer>(newBuffer));
    mtx.unlock();
}

int BuffQueue::read_packet_callback(uint8_t *buf_out, int buf_size) {
    std::shared_ptr<XBuffer> &q_buffer = queue.front();

    if (q_buffer->size() >= buf_size) {
        read_one_front(buf_out, (size_t) buf_size, q_buffer);
        return buf_size;
    } else {
        size_t total_size = 0;
        size_t read_size = 0;
        do {
            total_size += q_buffer->size();
            if (total_size > buf_size) {
                size_t size = q_buffer->size() - (total_size - buf_size);
                read_one_front(buf_out + read_size, size, q_buffer);
                return buf_size;
            } else {
                memcpy(buf_out + read_size, &q_buffer->front(), q_buffer->size());
            }
            read_size = total_size;
            mtx.lock();
            queue.pop_front();
            mtx.unlock();
            q_buffer = queue.front();
            if (!q_buffer)
                break;
        } while (total_size < buf_size);

        return (int) read_size;
    }
}

void BuffQueue::write_data(uint8_t *data, size_t size) {
    XBuffer *buffer = new XBuffer();
    buffer->resize(size);

    uint8_t *head = &buffer->front();
    memcpy(head, data, size);

    queue.push_back(std::shared_ptr<XBuffer>(buffer));
}