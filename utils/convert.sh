#!/bin/bash
rm ../firmware/Core/Src/bitmaps/img_*
python3 LVGLImage.py --ofmt C --cf RGB565 -o ../firmware/Core/Src/bitmaps images/
