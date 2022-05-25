#include "queue.h"

void queue_init(Queue* queue) {
    queue->last = NULL;
    queue->first = NULL;
    for(int i=0; i<SIZE;i++) {
        queue->arr[i] = 0;
    }
}

void queue_clear(Queue* queue) {
    queue->last = NULL;
    queue->first = NULL;
    for(int i=0; i<SIZE;i++) {
        queue->arr[i] = 0;
    }
}

uint8_t queue_size(Queue* queue) {
    if(queue->first == NULL) {
        return 0;
    } else if(queue->first == queue->last) {
        return 1;
    } else if(queue->first < queue->last) {
        return (queue->last - queue->first) + 1;
    } else {
        return (SIZE +1 - (queue->first - queue->last));
    }
}

void queue_push(Queue* queue, uint8_t val) {
    if(queue->first == NULL) {
        queue->arr[0] = val;
        queue->first = queue->arr;
        queue->last = queue->arr;
    } else if(queue->first == queue->last) {
        //one element
        if(queue->last == &queue->arr[SIZE]) {
            queue->last = queue->arr;
            queue->arr[0] = val;
        } else {
            queue->last ++;
            queue->arr[queue->last - queue->arr] = val;
        }
    } else if(queue->first < queue->last){
        if(queue->last < &(queue->arr[SIZE])-1) {
            queue->last ++;
            queue->arr[queue->last - queue->arr] = val;
        } else {
            if(queue->first != &queue->arr[0]) {
                queue->last = queue->arr;
                queue->arr[0] = val;
            } else {
                return;
            }
        }
    } else {
        if(queue->last < (queue->first)-1) {
            queue->last++;
            queue->arr[queue->last - queue->arr] = val;
        } else {
            return;
        }
    }
}

uint8_t queue_pop(Queue* queue) {
    if(queue->first == NULL) {
        return 0;
    } else if(queue->first == queue->last){
        //one value in queue
        uint8_t temp = queue->arr[queue->last - queue->arr];
        queue->first = NULL;
        queue->last = NULL;
        return temp;
    } else if(queue->first < queue->last) {
        uint8_t temp = *queue->first;
        queue->first ++;
        return temp;
    } else {
        uint8_t temp = *queue->first;
        if(queue->first < &queue->arr[SIZE]-1) {
            queue->first++;
            return temp;
        } else {
            queue->first = &queue->arr[0];
            return temp;
        }
    }
}