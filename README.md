# **Finding Lane Lines on the Road** 

### System Requirements
MacOS High Sierra 10.13.1
OpenCV 3.2.0

### Directory Structure
build: compilation script and output/executible files
demo: demo videos and screenshots of the program
imgs: test images
src: source code and CMake file
vids: test videos

### Build and Run Instructions
1.  If you've never run a C++ OpenCV Application before do the following, otherwise skip:
    Download the appropriate OpenCV library zip file
    cd to the root directory of the OpenCV library folder and run: cmake .
2.  cd to build directory of the package I sent you and run: sh compile.sh
    All this shell script does is call cmake and then make.
3.  Run the executable, specify a .jpg or .mp4 file as the command line argument.
    For example: ./LaneDetection ../vids/SolidWhiteRight.mp4
If you have any questions, just send me a message (h397wang@uwaterloo.ca)

### Pipeline Description

1. Convert to grayscale.
2. Perform histogram equalization to improve contrast and range of pixel values.
3. Gaussian blur is applied to remove noise for the next stage.
4. Use Canny edge detection.
5. For the bottom left and right corners. Apply Hough Transform to detect straight lines. 
6. Group "close" lines together "close", and use average rho and theta values to represent a line group.

### Issues and Improvements
No color heuristics.
Everything is done on a grayscale image. Working with color images is non-trivial, but could be helpful if done right. For example, extracting the color of the road and lane line paint would allow easier identification of such features.

Hough Line detection only detects straight lines.
It's harder to find a curved line. But we can join together smaller segments returned from Hough Transform to better describe a curved line.

Canny and Hough algorithms use thresholding.
The thresholds are currently constant, and quite arbituary. They should be determined through rigorous experimental analysis and/or adjusted dynamically based on other input.

Curvature of road not accounted for.
The lane line are currently represented using 2 straight lines. The curvature of the road needs to be described using some radius value. One way to do this is to do an affine transform so we can see the road from a bird's eye view, and calculate as required.

