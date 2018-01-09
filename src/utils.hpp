// utils.hpp

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

using namespace cv;
using namespace std;

Mat detectLanes( Mat aMat );

int processStaticImage( const char* apFileName );

int processVideo( const char* apFileName );
