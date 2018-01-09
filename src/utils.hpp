// utils.hpp
#include <cv.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

using namespace cv;
using namespace std;

typedef struct {
    Point mPt1;
    Point mPt2;
} line_t;

line_t processHoughLine( Vec2f aLine, Point aOffset );

// TODO: Should really be a class but I'm too lazy
typedef struct {
    float mRhoSum;
    float mThetaSum;
    int mNumLines;
} houghLineGroup_t;

vector<Vec2f> groupLines(
    const vector<Vec2f> aLines,
    const float aRhoEps,
    const float aThetaEps
    );

vector<line_t> processHoughLines(
    vector<Vec2f> aLine,
    Rect aRoiBound
    ); 

line_t extractCurrentLaneLine(
    const vector<Vec2f> aLines
  );

void drawHoughLines( Mat& aMat, const vector<Vec2f> aLines, Point aOffset );

void detectSingleLaneLine(
    Mat& aMat,
    Mat aMatEdge,
    Rect aRoiRect
    );

Mat detectLanes( Mat aMat );

int processStaticImage( const char* apFileName );

int processVideo( const char* apFileName );
