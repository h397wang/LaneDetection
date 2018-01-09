// utils.cpp

#include "utils.hpp"

Mat detectLanes( Mat aMat )
{
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
