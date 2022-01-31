# tennis-court-detection
[![Build
Status](https://travis-ci.com/gchlebus/tennis-court-detection.svg?branch=master)](https://travis-ci.com/gchlebus/tennis-court-detection)
[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

Fully automatic algorithm for tennis court line detection. Implementation based on Farin
D. et al. "Robust Camera Calibration for Sports Videos using Court Models", *Storage and
Retrieval Methods and Applications for Multimedia 2004*, 2003.

![AlgorithmResult](tennis-court-detection.png)

# Installation instructions

```
git clone https://github.com/gchlebus/tennis-court-detection.git
cd tennis-court-detection
mkdir build && cd build
conan install .. --build missing
cmake ..
cmake --build .
```

# Usage

The detection algorithm is started for the middle frame of the input video file (in avi
format).

```
Usage: ./detect directory_path
       directory_path:  path to an input image directory.
```

## Output format

For each image `<imagename>` in the input directory, two output files are produced.
`<imagename>.with_lines.png` contains a visualization of the detected lines.
`<imagename>.txt` contains one point per line. The XY coordinates are separated by a
semicolon ";".  The points are in the following order:

```
1.  Intersection of the upper base line with the left side line
2.  Intersection of the lower base line with the left side line
3.  Intersection of the lower base line with the right side line
4.  Intersection of the upper base line with the right side line
5.  Intersection of the upper base line with the left singles line
6.  Intersection of the lower base line with the left singles line
7.  Intersection of the lower base line with the right singles line
8.  Intersection of the upper base line with the right singles line
9.  Intersection of the left singles line with the upper service line
10. Intersection of the right singles line with the upper service line
11. Intersection of the left singles line with the lower service line
12. Intersection of the right singles line with the lower service line
13. Intersection of the upper service line with the center service line
14. Intersection of the lower service line with the center service line
15. Intersection of the left side line with the net line
16. Intersection of the right side line with the net line
```

