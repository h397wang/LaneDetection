// utils.cpp

#include "utils.hpp"

line_t processHoughLine( Vec2f aLine, Rect aRoiBound )
{
    const float rho = aLine[0];
    const float theta = aLine[1];
    Point pt1, pt2;
    const double a = cos(theta);
    const double b = sin(theta);
    const double x0 = a * rho;
    const double y0 = b * rho;
    pt1.x = cvRound(x0 + 1000 * (-b));
    pt1.y = cvRound(y0 + 1000 * (a));
    pt2.x = cvRound(x0 - 1000 * (-b));
    pt2.y = cvRound(y0 - 1000 * (a));

    pt1.x += aRoiBound.x;
    pt1.y += aRoiBound.y;

    pt2.x += aRoiBound.x;
    pt2.y += aRoiBound.y;

    clipLine( aRoiBound, pt1, pt2 );
    
    line_t vLineClipped;
    vLineClipped.mPt1 = pt1;
    vLineClipped.mPt2 = pt2;
    return vLineClipped;
}

vector<line_t> processHoughLines(
    vector<Vec2f> aLines,
    Rect aRoiBound
    ) 
{   
    vector<line_t> vProcessedLines = vector<line_t>();
    for( size_t i = 0; i < aLines.size(); i++ )
    {
        line_t vProcessedLine = processHoughLine( aLines[i], aRoiBound );
        vProcessedLines.push_back( vProcessedLine );
    }
    return vProcessedLines;
}

vector<Vec2f> groupLines(
    const vector<Vec2f> aLines,
    float aRhoEps,
    float aThetaEps
    )
{   
    vector<Vec2f> vMergedLines = vector<Vec2f>();
    if ( aLines.size() != 0 ) {
        vector<houghLineGroup_t> vLineGroups = vector<houghLineGroup_t>();
        houghLineGroup_t vFirstGroup = { aLines[0][0], aLines[0][1], 1 };
        vLineGroups.push_back( vFirstGroup );

        for( size_t i = 1; i < aLines.size(); i++ )
        {
            const float vRho = aLines[i][0];
            const float vTheta = aLines[i][1];
            
            bool vAddedToLineGroup = false;
            for( size_t j = 1; j < vLineGroups.size(); j++ )
            {
                const float vRhoMean = vLineGroups[j].mRhoSum / vLineGroups[j].mNumLines;
                const float vThetaMean = vLineGroups[j].mThetaSum / vLineGroups[j].mNumLines;
                
                if ( 1 
                    && ( abs( vRhoMean - vRho ) < aRhoEps )
                    && ( abs( vThetaMean - vTheta ) < aThetaEps )
                    )
                { // Add to the current line group
                    vLineGroups[j].mRhoSum += vRho;
                    vLineGroups[j].mThetaSum += vTheta;
                    vLineGroups[j].mNumLines++;
                    vAddedToLineGroup = true;
                    break;
                }
            }
            if ( ! vAddedToLineGroup )
            {
                const houghLineGroup_t vNewGroup = { vRho, vTheta, 1 };
                vLineGroups.push_back( vNewGroup );
            }     
        }

        for( size_t j = 0; j < vLineGroups.size(); j++ )
        {
            const float vRhoMean = vLineGroups[j].mRhoSum / vLineGroups[j].mNumLines;
            const float vThetaMean = vLineGroups[j].mThetaSum / vLineGroups[j].mNumLines;
            const Vec2f vLine( vRhoMean, vThetaMean );
            vMergedLines.push_back( vLine );
        }
    }

    return vMergedLines;
}


// largest abs slope
line_t extractCurrentLaneLine(
    const vector<line_t> aLines
  )
{   
    int vIndx = 0;
    float vMaxAbsSlope = 0;
    for( size_t i = 0; i < aLines.size(); i++ )
    {
        const float vDeltaX = aLines[i].mPt1.x - aLines[i].mPt2.x;
        const float vDeltaY = aLines[i].mPt1.y - aLines[i].mPt2.y;
        const float vSlope =  vDeltaY / vDeltaX;

        if ( abs(vSlope) > vMaxAbsSlope )
        {
            vMaxAbsSlope = abs(vSlope);
            vIndx = i;
        }
    }
    return aLines[vIndx];
}

void drawHoughLines(
    Mat& aMat,
    const vector<Vec2f> aLines,
    Point aOffset
    )
{   
    Rect vRoiRect(
        aOffset.x,
        aOffset.y,
        aMat.cols - aOffset.x,
        aMat.rows - aOffset.y
        );
    vector<line_t> vProcessedLines = processHoughLines( aLines, vRoiRect );
        
    for( size_t i = 0; i < vProcessedLines.size(); i++ )
    {
        line(
            aMat,
            vProcessedLines[i].mPt1,
            vProcessedLines[i].mPt2,
            Scalar( 55, 155, 255 ),
            3,
            CV_AA
            );
    }
}

Mat detectLanes( Mat aMat )
{
    // Grayscale
    Mat vMatGray;
    cvtColor( aMat, vMatGray, CV_BGR2GRAY);

    // Equalize to improve contrast
    Mat vMatEqualized;
    equalizeHist( vMatGray, vMatEqualized );

    // Gaussian blur
    Mat vMatBlurred;
    GaussianBlur( vMatGray, vMatBlurred, Size( 5, 5 ), 0, 0 );

    // Thresholding
    // TODO: Thresholding based on color heuristics is non-trivial and noisy.

    // Canny
    Mat vMatEdges;
    Canny( vMatBlurred, vMatEdges, 35, 90);

    // Hough Transform
    const double vRho = 1;
    const double vTheta = 2 * M_PI / 180.0;
    // This value is critical and hard to determine
    const int vThreshold = vMatEdges.rows / 8;

    // Bottom right corner
    {
    Rect vRoiRect( vMatEdges.cols / 2, vMatEdges.rows / 2, vMatEdges.cols / 2, vMatEdges.rows / 2);
    Mat vMatRoi = vMatEdges( vRoiRect );
    vector<Vec2f> vLines;
    HoughLines( vMatRoi, vLines, vRho, vTheta, vThreshold );
    vLines = groupLines( vLines, 5 * vRho, 5 * vTheta);
    vector<line_t> vProcessedLines = processHoughLines( vLines, vRoiRect );
    line_t vLaneLine = extractCurrentLaneLine( vProcessedLines );
    line(
        aMat,
        vLaneLine.mPt1,
        vLaneLine.mPt2,
        Scalar( 255, 255, 255 ),
        3,
        CV_AA
        );
    }

    // Bottom left corner
    {
    Rect vRoiRect( 0, vMatEdges.rows / 2, vMatEdges.cols / 2, vMatEdges.rows / 2);
    Mat vMatRoi = vMatEdges( vRoiRect );
    vector<Vec2f> vLines;
    HoughLines( vMatRoi, vLines, vRho, vTheta, vThreshold );
    vector<line_t> vProcessedLines = processHoughLines( vLines, vRoiRect );
    line_t vLaneLine = extractCurrentLaneLine( vProcessedLines );
    line(
        aMat,
        vLaneLine.mPt1,
        vLaneLine.mPt2,
        Scalar( 255, 255, 255 ),
        3,
        CV_AA
        );    
    }

    return aMat;
}


int processStaticImage( const char* apFileName )
{
    const char* vpWindowNameOriginal = "Original";
    namedWindow(vpWindowNameOriginal);

    const char* vpWindowNameProcessed = "Processed";
    namedWindow(vpWindowNameProcessed);

    const Mat vImgOriginal = imread( apFileName );
    if( ! vImgOriginal.data )
    {
        cerr <<  "Could not open file: " << apFileName << endl;
        return -1;
    }

    Mat vImgProcessed = detectLanes( vImgOriginal );

    imshow( vpWindowNameOriginal, vImgOriginal );
    imshow( vpWindowNameProcessed, vImgProcessed );
    
    waitKey(0);
    return 0;
}

int processVideo( const char* apFileName )
{
    const char* vpWindowNameOriginal = "Original";
    namedWindow(vpWindowNameOriginal);

    const char* vpWindowNameProcessed = "Processed";
    namedWindow(vpWindowNameProcessed);

    VideoCapture vVidCap( apFileName );
    if( !vVidCap.isOpened() )
    {
        cerr << "Error opening video stream or file" << endl;
        return -1;
    }

    // Collect information about video file
    const int vFramePeriod = 33;

    while( 1 ){
        Mat vFrameOriginal;
        vVidCap >> vFrameOriginal; // API does some stupid overloading of right shift operator
        if ( vFrameOriginal.empty() )
        {
          break;
        }
        
        Mat vFrameProcessed = detectLanes( vFrameOriginal );

        imshow( vpWindowNameOriginal, vFrameOriginal );
        imshow( vpWindowNameProcessed, vFrameProcessed );
    
        // Exit with escape key
        const char vCharKey = waitKey( vFramePeriod );
        if( vCharKey == 27 )
        {
            break;
        }
    }

    vVidCap.release();
    
    return 0;
}
