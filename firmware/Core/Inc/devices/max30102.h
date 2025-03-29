#ifndef MAX30102_H
#define MAX30102_H
#include <stdint.h>

typedef struct {
    uint32_t red[32];
    uint32_t IR[32];
    uint32_t green[32];
    uint8_t head;
    uint8_t tail;
} max_struct_t;

void max30102_init();

int max30102_read_data();

void max30102_clear_int();

#endif //MAX30102_H
