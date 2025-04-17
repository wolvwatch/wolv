#ifndef BIOMETRICS_H
#define BIOMETRICS_H
#include "filter.h"

void update_biometrics(BWBandPass *filter, PeakDetector *detector);

#endif //BIOMETRICS_H
