#ifndef QUEUE_H
#define QUEUE_H

#include "stm32g070xx.h"
#include "stdint.h"
#include "stddef.h"
#define SIZE 20

typedef struct QUEUE {
    uint8_t* last;
    uint8_t* first;
    uint8_t arr[SIZE];
}Queue;

void queue_init(Queue* queue);

void queue_clear(Queue* queue);

uint8_t queue_size(Queue* queue);

void queue_push(Queue* queue, uint8_t val);

uint8_t queue_pop(Queue* queue);

#endif //QUEUE_H