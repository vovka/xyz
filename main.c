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

/*
    // All command line params example: 
    --min-squares-area 25000
    --threshold-level-to-allocate-checked-checkboxes 150
    --debug
    --threshold 50
    --min-checkbox-area 50
    --question-to-outer-rect-width-ratio 1.099426386
    --checkboxes-area-width-to-question-width-ratio 0.1
    --similar-checkboxes-distance 8
    --similar-squares-distance 20
    --min-vector-length-for-similarity 40
    --min-y-length-for-similarity 10
*/
int main(int argc, char** argv)
{
    const char* filename;
    const char* outputResultsAs = "human";
    int showDialog = 0;
    int debug = 0;
/*
    // Default:
    int minSquaresArea = 50000;//10000;
    int thresholdLevelToAllocateCheckedCheckboxes = 150;
    int debug = 0;
    int thresh = 50;
    int minCheckboxArea = 50;
    double questionToOuterRectWidthRatio = 1014. / 909.;
    float checkboxesAreaWidthToQuestionWidthRatio = 0.1;
    int similarCheckboxesDistance = 8;
    int similarSquaresDistance = 20;
    int minVectorLengthForSimilarity = 40;
    int minYLengthForSimilarity = 10;
*/
/*
 * tests_for_recognize_square_checkboxes_qr_origin_rotated.jpg --debug --min-squares-area 25000 --threshold-level-to-allocate-checked-checkboxes 160 --threshold 50 --min-checkbox-area 50 --question-to-outer-rect-width-ratio 1.11551155115512 --checkboxes-area-width-to-question-width-ratio 0.1 --similar-checkboxes-distance 10 --similar-squares-distance 20 --min-vector-length-for-similarity 20 --min-y-length-for-similarity 10
 *
 * scans/скан5.jpg --debug --min-squares-area 25000 --threshold-level-to-allocate-checked-checkboxes 160 --threshold 50 --min-checkbox-area 100 --question-to-outer-rect-width-ratio 1.11551155115512 --checkboxes-area-width-to-question-width-ratio 0.1 --similar-checkboxes-distance 10 --similar-squares-distance 20 --min-vector-length-for-similarity 50 --min-y-length-for-similarity 10
 */
    int minSquaresArea = 25000;
    int thresholdLevelToAllocateCheckedCheckboxes = 160;
    int thresh = 50;
    int minCheckboxArea = 50;
    double questionToOuterRectWidthRatio = 1014. / 909.;
    double checkboxesAreaWidthToQuestionWidthRatio = 0.1;
    int similarCheckboxesDistance = 20;
    int similarSquaresDistance = 20;
    int minVectorLengthForSimilarity = 10;
    int minYLengthForSimilarity = 10;

    
    if (!parseCmdParameters(
            argc,
            argv,
            &filename,
            &showDialog,
            &outputResultsAs,
            &minSquaresArea,
            &thresholdLevelToAllocateCheckedCheckboxes,
            &debug,
            &thresh,
            &minCheckboxArea,
            &questionToOuterRectWidthRatio,
            &checkboxesAreaWidthToQuestionWidthRatio,
            &similarCheckboxesDistance,
            &similarSquaresDistance,
            &minVectorLengthForSimilarity,
            &minYLengthForSimilarity
            ))
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
    squares = filterImageBorderSquare( filterSimilarSquares(squares, similarSquaresDistance, storage), storage );
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
    squares =  filterSimilarSquares(squares, similarSquaresDistance, storage);
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
    squares = filterImageBorderSquare( filterSimilarSquares(squares, similarSquaresDistance, storage), storage );

    int* results;
    int totalQuestions;

    //IplImage* qr = getSubimage(img, cvRect(440, 0, 110, 110));
    //printf("questionToOuterRectWidthRatio: %f; checkboxesAreaWidthToQuestionWidthRatio: %f\n", questionToOuterRectWidthRatio, checkboxesAreaWidthToQuestionWidthRatio);
    cvSaveImage("/tmp/rotated_image.png", (CvArr*)img, 0);
    recognize(
        img,
        squares,
        storage,
        thresh,
        thresholdLevelToAllocateCheckedCheckboxes,
        debug,
        wndname,
        &results,
        &totalQuestions,
        minCheckboxArea,
        &questionToOuterRectWidthRatio,
        &checkboxesAreaWidthToQuestionWidthRatio,
        similarCheckboxesDistance,
        minVectorLengthForSimilarity,
        minYLengthForSimilarity
    );
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
    //outputProfileInfo();

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
