#!/usr/bin/env python3

import sys
import math

if len(sys.argv) != 2:
    print("Usage: ", sys.argv[0], " <quadrant entry count>")
    sys.exit(1);

entries = int(sys.argv[1]);
    
print("static const float sine_LUT[", entries, "] = { ");
