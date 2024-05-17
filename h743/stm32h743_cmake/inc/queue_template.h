#ifndef QUEUE_TEMPLATE_H
#define QUEUE_TEMPLATE_H


//////////      set it in src file  ////////////////
//template <typename T, uint16_t len>
//QueueAny<T, len> *QueueAny<T, len>::pThis = nullptr;
//QueueAny<uint16_t, 255> queueAny;

//  +---------------------------------------------+
//  |       QUEUE FOR ANY DATA                    |
//  +---------------------------------------------+

template <typename T, uint16_t len> class QueueAny {
  public:
    QueueAny() { pThis = this; }
    static QueueAny *pThis;
    T size() {
        if (first == nullptr) {
            return 0;
        } else if (first == last) {
            return 1;
        } else if (first < last) {
            return (last - first) + 1;
        } else {
            return (t_size + 1 - (first - last));
        }
    }
    T pop() {
        if (first == nullptr) {
            return 0;
        } else if (first == last) {
            // one value in queue
            uint8_t temp = buffer[last - buffer];
            first = nullptr;
            last = nullptr;
            return temp;
        } else if (first < last) {
            uint8_t temp = *first;
            first++;
            return temp;
        } else {
            uint8_t temp = *first;
            if (first < &buffer[t_size] - 1) {
                first++;
                return temp;
            } else {
                first = &buffer[0];
                return temp;
            }
        }
    }
    void push(T val) {
        if (first == nullptr) {
            buffer[0] = val;
            first = buffer;
            last = buffer;
        } else if (first == last) {
            // one element
            if (last == &buffer[t_size]) {
                last = buffer;
                buffer[0] = val;
            } else {
                last++;
                buffer[last - buffer] = val;
            }
        } else if (first < last) {
            if (last < &(buffer[t_size]) - 1) {
                last++;
                buffer[last - buffer] = val;
            } else {
                if (first != &buffer[0]) {
                    last = buffer;
                    buffer[0] = val;
                } else {
                    return;
                }
            }
        } else {
            if (last < (first)-1) {
                last++;
                buffer[last - buffer] = val;
            } else {
                return;
            }
        }
    }

  private:
    uint16_t t_size = len;
    uint8_t *last = nullptr;
    uint8_t *first = nullptr;
    uint8_t buffer[len];
};

#endif // QUEUE_TEMPLATE_H