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
int thresh = 50;
IplImage* img = 0;
IplImage* img0 = 0;
CvMemStorage* storage = 0;

const char* names[] = {
    //"tests_for_recognize_square_checkboxes_origin3.jpg",
    //"tests_for_recognize_square_checkboxes_origin3_rotated1.jpg",
    "tests_for_recognize_square_checkboxes_origin3_rotated2.jpg",
    "tests_for_recognize_square_checkboxes_origin3_rotated3.jpg",
    0 };

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
    int i, c;
    // create memory storage that will contain all the dynamic data
    storage = cvCreateMemStorage(0);

    for( i = 0; names[i] != 0; i++ )
    {
        // load i-th image
        img0 = cvLoadImage( names[i], 1 );
        if( !img0 )
        {
            printf("Couldn't load %s\n", names[i] );
            continue;
        }
        img = cvCloneImage( img0 );

/*
window!
*/
        cvNamedWindow( wndname, 1 );
        cvShowImage( wndname, img );
        c = cvWaitKey(0);

        CvSeq* squares = findSquares4( img, storage, 0, 1000, thresh );
        squares = filterImageBorderSquare( filterSimilarSquares(squares, 15, storage), storage );
/*
window!
*/
        drawSquares( &img, squares, wndname);
        c = cvWaitKey(0);

        CvPoint** outerRectangle = getOuterRectangle( squares );

        float ang = rotationAngle( outerRectangle );
        rotateImage( &img, &ang );
        squares = findSquares4( img, storage, 0, 1000, thresh ); // find squares again after rotation
        squares =  filterSimilarSquares(squares, 15, storage);
        squares =  filterImageBorderSquare(squares, storage);
/*
window!
*/
        drawSquares( &img, squares, wndname);
        c = cvWaitKey(0);

        outerRectangle = getOuterRectangle( squares );  // find outer rectangle again after rotation

        img = cropImage( &img, &outerRectangle );
/*
window!
*/
        cvShowImage( wndname, img );
        c = cvWaitKey(0);
        
        squares = findSquares4( img, storage, 0, 1000, thresh ); // find squares again after crop
        squares = filterImageBorderSquare( filterSimilarSquares(squares, 15, storage), storage );

        int* results;
        int totalQuestions;

        recognize( img, squares, storage, thresh, wndname, &results, &totalQuestions );
        outputResults(names[i], &results, totalQuestions, "json");

/*
window!
*/
        // find and draw the squares
        drawSquares( &img, squares, wndname );
        // wait for key.
        // Also the function cvWaitKey takes care of event processing
        c = cvWaitKey(0);
        
        // release both images
        cvReleaseImage( &img );
        cvReleaseImage( &img0 );
        // clear memory storage - reset free space position
        cvClearMemStorage( storage );
        if( (char)c == 27 )
            break;
    }

/*
window!
*/
    cvDestroyWindow( wndname );

    return 0;
}
