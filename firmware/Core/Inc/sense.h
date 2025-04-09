#ifndef SENSE_H
#define SENSE_H

#include "stdint.h"
#include "stdbool.h"

typedef struct {
  uint32_t hr;
  bool val_hr;

  uint32_t spo2;
  bool val_spo2;
} sense_t;

#endif //SENSE_H
