/* 
 * File:   main.c
 * Author: vovka
 *
 * Created on 6 серпня 2010, 11:15
 */

//
// The full "Square Detector" program.
// It loads several images subsequentally and tries to find squares in
// each image
//
#ifdef _CH_
#pragma package <opencv>
#endif

#define CV_NO_BACKWARD_COMPATIBILITY

#include "helpers.h"

const char* wndname = "Square Detection Demo";
IplImage* img = 0;
IplImage* img0 = 0;
CvMemStorage* storage = 0;

/*
const char* names[] = {
    "tests_for_recognize_square_checkboxes_origin5_min.jpg",
    0 };
*/

float rotationAngle( /*CvSeq* outerRectangle*/ /*CvPoint* topLeft, CvPoint* bottomLeft*/ CvPoint** pointers )
{
    CvPoint* topLeft = pointers[0];
    CvPoint* bottomLeft = pointers[3];
    int height = bottomLeft->y - topLeft->y;
    int width = topLeft->x - bottomLeft->x;
    double arctgRad = atan((double)width / (double)height);
    return (float)(arctgRad / 3.14159265358979323846 * 180.);
}

int main(int argc, char** argv)
{
    const char* filename;
    const char* outputResultsAs = "human";
    int showDialog = 0;
    int minSquaresArea = 10000;
    int thresholdLevelToAllocateCheckedCheckboxes = 150;
    int debug = 0;
    int thresh = 50;
    
    if (!parseCmdParameters(
            argc,
            argv,
            &filename,
            &showDialog,
            &outputResultsAs,
            &minSquaresArea,
            &thresholdLevelToAllocateCheckedCheckboxes,
            &debug,
            &thresh))
        return -1;

    int i, c;
    // create memory storage that will contain all the dynamic data
    storage = cvCreateMemStorage(0);

    img0 = cvLoadImage( filename, 1 );
    if( !img0 )
    {
        printf("Couldn't load %s\n", filename );
        return 1;
    }
    img = cvCloneImage( img0 );

    CvSeq* squares = findSquares4( img, storage, 0, minSquaresArea, thresh );
    squares = filterImageBorderSquare( filterSimilarSquares(squares, SIMILAR_SQUARES_DISTANCE, storage), storage );
/*
window!
*/
    if (debug)
    {
        drawSquares( &img, squares, wndname);
        c = cvWaitKey(0);
    }

    CvPoint** outerRectangle = getOuterRectangle( squares );

    float ang = rotationAngle( outerRectangle );
    rotateImage( &img, &ang );
    squares = findSquares4( img, storage, 0, minSquaresArea, thresh ); // find squares again after rotation
    squares =  filterSimilarSquares(squares, SIMILAR_SQUARES_DISTANCE, storage);
    squares =  filterImageBorderSquare(squares, storage);
/*
window!
*/
    if (debug)
    {
        drawSquares( &img, squares, wndname);
        c = cvWaitKey(0);
    }

    outerRectangle = getOuterRectangle( squares );  // find outer rectangle again after rotation

    img = cropImage( &img, &outerRectangle );
/*
window!
*/
    if (debug)
    {
        cvShowImage( wndname, img );
        c = cvWaitKey(0);
    }

    squares = findSquares4( img, storage, 0, minSquaresArea, thresh ); // find squares again after crop
    squares = filterImageBorderSquare( filterSimilarSquares(squares, SIMILAR_SQUARES_DISTANCE, storage), storage );

    int* results;
    int totalQuestions;

    recognize( img, squares, storage, thresh, thresholdLevelToAllocateCheckedCheckboxes, debug, wndname, &results, &totalQuestions );
    outputResults(filename, &results, totalQuestions, outputResultsAs);

/*
window!
*/
    if (debug)
    {
        // find and draw the squares
        drawSquares( &img, squares, wndname );
        // wait for key.
        // Also the function cvWaitKey takes care of event processing
        c = cvWaitKey(0);
    }

    stopTimeProfiling();
    outputProfileInfo();

/*
window!
*/
    if (showDialog)
    {
        cvNamedWindow( wndname, 1 );
        cvShowImage( wndname, img0 );
        c = cvWaitKey(0);
    }

    // release both images
    cvReleaseImage( &img );
    cvReleaseImage( &img0 );
    // clear memory storage - reset free space position
    cvClearMemStorage( storage );

    if (showDialog)
    {
        cvDestroyWindow( wndname );
    }

    return 0;
}
