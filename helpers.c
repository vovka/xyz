#include "helpers.h"

/*
const int THRESHOLD_LEVEL_TO_ALLOCATE_CHECKED_CHECKBOX = 100;//162;
//const int THRESHOLD_LEVEL_TO_ALLOCATE_ALL_CHECKBOXES = 300;//231;
const int MIN_CHECKBOX_AREA = 100;
const int SIMILAR_CHECKBOXES_DISTANCE = 10;
*/

// the function draws all the squares in the image
void drawSquares( IplImage** tmpImg, CvSeq* squares, char* wndname)
{
    CvSeqReader reader;
    IplImage* cpy = cvCloneImage( *tmpImg );
    int i;

    // initialize reader of the sequence
    cvStartReadSeq( squares, &reader, 0 );

    srand(time(0));

    // read 4 sequence elements at a time (all vertices of a square)
    for( i = 0; i < squares->total; i += 4 )
    {
        CvPoint pt[4], *rect = pt;

        // read 4 vertices
        CV_READ_SEQ_ELEM( pt[0], reader );
        CV_READ_SEQ_ELEM( pt[1], reader );
        CV_READ_SEQ_ELEM( pt[2], reader );
        CV_READ_SEQ_ELEM( pt[3], reader );

        // draw the square as a closed polyline
      cvLine(cpy, cvPoint(rect[0].x, rect[0].y), cvPoint(rect[1].x, rect[1].y), CV_RGB(200, 0, 0), 1, 8, 0);
      cvLine(cpy, cvPoint(rect[1].x, rect[1].y), cvPoint(rect[2].x, rect[2].y), CV_RGB(0, 200, 0), 1, 8, 0);
      cvLine(cpy, cvPoint(rect[2].x, rect[2].y), cvPoint(rect[3].x, rect[3].y), CV_RGB(0, 0, 200), 1, 8, 0);
      cvLine(cpy, cvPoint(rect[3].x, rect[3].y), cvPoint(rect[0].x, rect[0].y), CV_RGB(200, 200, 0), 1, 8, 0);
    }

    int dx = 15;
    cvLine(cpy, cvPoint(10, 10), cvPoint(10 + dx, 10), CV_RGB(200, 0, 0), 1, 8, 0);

    // show the resultant image
/*
window!
    cvShowImage( wndname, cpy );
*/
    cvReleaseImage( &cpy );
}

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double angle( CvPoint* pt1, CvPoint* pt2, CvPoint* pt0 )
{
    double dx1 = pt1->x - pt0->x;
    double dy1 = pt1->y - pt0->y;
    double dx2 = pt2->x - pt0->x;
    double dy2 = pt2->y - pt0->y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
CvSeq* findSquares4( IplImage* tmpImg, CvMemStorage* tmpStorage, char* wndName, int minArea, int thresh )
{
    CvSeq* contours;
    int i, c, l, N = 15;   //default was 11
    CvSize sz = cvSize( tmpImg->width & -2, tmpImg->height & -2 );
    IplImage* timg = cvCloneImage( tmpImg ); // make a copy of input image
    IplImage* gray = cvCreateImage( sz, 8, 1 );
    IplImage* pyr = cvCreateImage( cvSize(sz.width/2, sz.height/2), 8, 3 );
    IplImage* tgray;
    CvSeq* result;
    double s, t;
    // create empty sequence that will contain points -
    // 4 points per square (the square's vertices)
    CvSeq* squares = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint), tmpStorage );

    // select the maximum ROI in the image
    // with the width and height divisible by 2
    cvSetImageROI( timg, cvRect( 0, 0, sz.width, sz.height ));

    // down-scale and upscale the image to filter out the noise
    cvPyrDown( timg, pyr, 7 );
    cvPyrUp( pyr, timg, 7 );
    tgray = cvCreateImage( sz, 8, 1 );

    // find squares in every color plane of the image
    for( c = 0; c < 3; c++ )
    {
        // extract the c-th color plane
        cvSetImageCOI( timg, c+1 );
        cvCopy( timg, tgray, 0 );

        // try several threshold levels
        for( l = 0; l < N; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                cvCanny( tgray, gray, 0, thresh, 5 );
                // dilate canny output to remove potential
                // holes between edge segments
                cvDilate( gray, gray, 0, 1 );
            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                cvThreshold( tgray, gray, (l+1)*255/N, 255, CV_THRESH_BINARY );
            }

            // find contours and store them all as a list
            cvFindContours( gray, tmpStorage, &contours, sizeof(CvContour),
                CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );

            // test each contour
            while( contours )
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                result = cvApproxPoly( contours, sizeof(CvContour), tmpStorage,
                    CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0 );

                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                double area = fabs(cvContourArea(result,CV_WHOLE_SEQ));
                if( result->total == 4 &&
                    area > minArea &&
                    cvCheckContourConvexity(result) )
                {
                    s = 0;

                    for( i = 0; i < 5; i++ )
                    {
                        // find minimum angle between joint
                        // edges (maximum of cosine)
                        if( i >= 2 )
                        {
                            t = fabs(angle(
                            (CvPoint*)cvGetSeqElem( result, i ),
                            (CvPoint*)cvGetSeqElem( result, i-2 ),
                            (CvPoint*)cvGetSeqElem( result, i-1 )));
                            s = s > t ? s : t;
                        }
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if( s < 0.1 )
                    {
                        CvPoint* points[4];
                        for( i = 0; i < 4; i++ )
                        {
                            points[i] = (CvPoint*)cvGetSeqElem( result, i );
                        }
                        normalizeRectanglePoints(points);
                        for( i = 0; i < 4; i++ )
                        {
                            cvSeqPush( squares, points[i]);
                        }
                    }
                }

                // take the next contour
                contours = contours->h_next;
            }
        }
    }

    // release all the temporary images
    cvReleaseImage( &gray );
    cvReleaseImage( &pyr );
    cvReleaseImage( &tgray );
    cvReleaseImage( &timg );

    return squares;
}

int isExistsSimilarPoint(CvSeq* filteredSquares, CvSeq* square, int index, int similarityDistance)
{
    int result = 0;
    if (filteredSquares){
        for(int i = 0; i < filteredSquares->total; i += 4)
        {
            CvPoint* p1 = (CvPoint*)cvGetSeqElem( filteredSquares, i );
            for (int j = 0; j < 4; j++)
            {
                CvPoint* p2 = (CvPoint*)cvGetSeqElem( square, index + j );

                int distance = sqrt(
                    pow(
                        (double)(p1->x - p2->x),
                        2
                    ) +
                    pow(
                        (double)(p1->y - p2->y),
                        2
                    )
                );
                if (distance < similarityDistance)
                {
                    result = 1;
                    break;
                }
            }
        }
    }
    return result;
}

int isBorderSquare(CvPoint* square)
{
    int result = 0;
    int counter = 0;
    for(int i = 0; i < 4; i++)
    {
        CvPoint p = *(square + i);
        if (1 == p.x || 1 == p.y)
        {
            counter++;
        }
    }
    if(counter >= 3)
    {
        result = 1;
    }
    return result;
}

CvSeq* filterSimilarSquares(CvSeq* squares, int similarityDistance, CvMemStorage* storage)
{
    CvSeq* filteredSquares = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint), storage );
    for (int i = 0; i < squares->total; i += 4)
    {
        if (!isExistsSimilarPoint(filteredSquares, squares, i, similarityDistance ))
        {
            cvSeqPush( filteredSquares, (CvPoint*)cvGetSeqElem( squares, i ));
            cvSeqPush( filteredSquares, (CvPoint*)cvGetSeqElem( squares, i + 1 ));
            cvSeqPush( filteredSquares, (CvPoint*)cvGetSeqElem( squares, i + 2 ));
            cvSeqPush( filteredSquares, (CvPoint*)cvGetSeqElem( squares, i + 3 ));
        }
    }
    return filteredSquares;
}

CvSeq* filterImageBorderSquare(CvSeq* squares, CvMemStorage* storage)
{
    CvSeq* filteredSquares = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint), storage );
    for (int i = 0; i < squares->total; i += 4)
    {
        if (!isBorderSquare((CvPoint*)cvGetSeqElem( squares, i )))
        {
            cvSeqPush( filteredSquares, (CvPoint*)cvGetSeqElem( squares, i ));
            cvSeqPush( filteredSquares, (CvPoint*)cvGetSeqElem( squares, i + 1 ));
            cvSeqPush( filteredSquares, (CvPoint*)cvGetSeqElem( squares, i + 2 ));
            cvSeqPush( filteredSquares, (CvPoint*)cvGetSeqElem( squares, i + 3 ));
        }
    }
    return filteredSquares;
}

void rotateImage(IplImage** image, float* ang)
{
    IplImage* rotatedImage = cvCreateImage(cvSize((*image)->width, (*image)->height), IPL_DEPTH_8U, (*image)->nChannels);

    CvPoint2D32f center;
    center.x = (float)((*image)->width) / 2.;
    center.y = (float)((*image)->height) / 2.;
    CvMat* mapMatrix = cvCreateMat( 2, 3, CV_32FC1 );

    cv2DRotationMatrix(center, *ang, 1.0, mapMatrix);
    cvWarpAffine(*image, rotatedImage, mapMatrix, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll(255));

    cvReleaseImage(image);
    *image = rotatedImage;
    cvReleaseMat(&mapMatrix);
}

/*
float rotationAngle(  CvPoint** pointers )
{
    CvPoint* topLeft = pointers[0];
    CvPoint* bottomLeft = pointers[3];    
    int height = bottomLeft->y - topLeft->y;
    int width = topLeft->x - bottomLeft->x;
    double arctgRad = atan((double)width / (double)height);
    return (float)(arctgRad / 3.14159265358979323846 * 180.);
}

*/
CvPoint** getOuterRectangle(CvSeq* squares)
{
    CvSeq* rect = squares;
    CvPoint* points[4];
    points[0] = (CvPoint*)cvGetSeqElem(squares, 0);
    points[1] = (CvPoint*)cvGetSeqElem(squares, 1);
    points[2] = (CvPoint*)cvGetSeqElem(squares, 2);
    points[3] = (CvPoint*)cvGetSeqElem(squares, 3);

    double vectorLength = sqrt(points[0]->x * points[0]->x + points[0]->y * points[0]->y);
    for (int i = 1; i< 4; i++)
    {
        double tmpVectorLength = sqrt(points[i]->x * points[i]->x + points[i]->y * points[i]->y);
        if (tmpVectorLength < vectorLength)
        {
            vectorLength = tmpVectorLength;
        }
    }

    double minVectorLength = vectorLength;
    CvSeq* outerRectangle = rect;

    for (int i = 4; i < squares->total; i += 4)
    {
        rect = (CvSeq*)cvGetSeqElem(squares, i);

        CvPoint* tmpPoints[4];

        tmpPoints[0] = (CvPoint*)cvGetSeqElem(squares, i + 0);
        tmpPoints[1] = (CvPoint*)cvGetSeqElem(squares, i + 1);
        tmpPoints[2] = (CvPoint*)cvGetSeqElem(squares, i + 2);
        tmpPoints[3] = (CvPoint*)cvGetSeqElem(squares, i + 3);

        double vectorLength = sqrt(tmpPoints[0]->x * tmpPoints[0]->x + tmpPoints[0]->y * tmpPoints[0]->y);
        for (int i = 1; i< 4; i++)
        {
            double tmpVectorLength = sqrt(tmpPoints[i]->x * tmpPoints[i]->x + tmpPoints[i]->y * tmpPoints[i]->y);
            if (tmpVectorLength < vectorLength)
            {
                vectorLength = tmpVectorLength;
            }
        }

        if(vectorLength < minVectorLength)
        {
            minVectorLength = vectorLength;
            outerRectangle = rect;
            for (int j = 0; j < 4; j++)
            {
                points[j] = tmpPoints[j];
            }
        }
    }

    normalizeRectanglePoints(points);

    return points;
}

void normalizeRectanglePoints(CvPoint** points)
{
    // Точки могут располагаться в любом порядке, т.е. начинаться, например, с
    // правой нижней точки и идти против часовой, или по часовой. Т.е. с любой
    // позиции в любом направлении. Чтобы упорядочить их (с левой верхней по
    // часовой) нахожу центр фигуры, провожу через центр дополнительную систему
    // кординат (представляемо). Таким образом точки будут расположены в разных
    // четвертях плоскости.
    int minX = points[0]->x;
    int maxX = points[0]->x;
    int minY = points[0]->y;
    int maxY = points[0]->y;
    for (int i = 1; i < 4; i++)
    {
        if (points[i]->x < minX)
        {
            minX = points[i]->x;
        }
        if (points[i]->x > maxX)
        {
            maxX = points[i]->x;
        }
        if (points[i]->y < minY)
        {
            minY = points[i]->y;
        }
        if (points[i]->y > maxY)
        {
            maxY = points[i]->y;
        }
    }
    int abscissa = minX + (int)( (double)(maxX - minX) / 2. );
    int ordinate = minY + (int)( (double)(maxY - minY) / 2. );
    for (int i = 0; i < 4; i++)
    {
        CvPoint* buf;
        // Top left point
        if (points[i]->x < abscissa && points[i]->y < ordinate)
        {
            buf = points[0];
            points[0] = points[i];
            points[i] = buf;
        }
        // Top right point
        if (points[i]->x > abscissa && points[i]->y < ordinate)
        {
            buf = points[1];
            points[1] = points[i];
            points[i] = buf;
        }
        // Bottom right point
        if (points[i]->x > abscissa && points[i]->y > ordinate)
        {
            buf = points[2];
            points[2] = points[i];
            points[i] = buf;
        }
        // Bottom left point
        if (points[i]->x < abscissa && points[i]->y > ordinate)
        {
            buf = points[3];
            points[3] = points[i];
            points[i] = buf;
        }
    }
}

IplImage* cropImage( IplImage** tmpImg, CvPoint*** outerRectangle )
{
    /* sets the Region of Interest
       Note that the rectangle area has to be __INSIDE__ the image */
    cvSetImageROI(  *tmpImg,
                    cvRect(     (*outerRectangle)[0]->x,
                                (*outerRectangle)[0]->y,
                                (*outerRectangle)[2]->x - (*outerRectangle)[0]->x,
                                (*outerRectangle)[2]->y - (*outerRectangle)[0]->y));
    /* create destination image
       Note that cvGetSize will return the width and the height of ROI */
    IplImage* img2 = cvCreateImage(    cvGetSize(*tmpImg),
                                       (*tmpImg)->depth,
                                       (*tmpImg)->nChannels);
    /* copy subimage */
    cvCopy(*tmpImg, img2, NULL);
    /* always reset the Region of Interest */
    cvResetImageROI(*tmpImg);
    return img2;
}

IplImage* getSubimage(IplImage* tmpImg, CvRect roi)
{
    IplImage *result;
    // set ROI, you may use following two funs:
    //cvSetImageROI( image, cvRect( 0, 0, image->width, image->height ));
    cvSetImageROI(tmpImg, roi);
    // sub-image
    result = cvCreateImage( cvGetSize(tmpImg), tmpImg->depth, tmpImg->nChannels );
    cvCopy(tmpImg, result, NULL);
    cvResetImageROI(tmpImg); // release image ROI
    return result;
}

IplImage* copyImage(IplImage* image)
{
    return cvClone(image);
}

IplImage* avoidThreshold(IplImage* src, int thresholdLevel)
{
    IplImage* res = cvCreateImage(cvSize(src->width, src->height), 8, 1);

    CvSize sz = cvSize( src->width , src->height );
    IplImage* timg = cvCloneImage( src ); // make a copy of input image
    cvSetImageROI( timg, cvRect( 0, 0, sz.width, sz.height ));
    IplImage* pyr = cvCreateImage( cvSize(sz.width/2, sz.height/2), 8, 3 );
    cvPyrDown( timg, pyr, 7 );
    cvPyrUp( pyr, timg, 7 );
    IplImage* tgray = cvCreateImage( sz, 8, 1 );
    cvSetImageCOI( timg, 1 );
    cvCopy( timg, tgray, 0 );
    cvThreshold( tgray, res, thresholdLevel, 255, CV_THRESH_BINARY );
    return res;
}

CvSeq* getCheckedSign(IplImage* image, CvMemStorage* storage)
{
    CvSeq* contours;
    cvFindContours( image, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
    return filterImageBorderSquare( contours, storage );
}

CvPoint getRectangleCenter(CvPoint point0, CvPoint point1, CvPoint point2, CvPoint point3 )
{
    //TODO: normalize it
    CvPoint figure[4];
    figure[0] = point0;
    figure[1] = point1;
    figure[2] = point2;
    figure[3] = point3;
    CvPoint center;
    int minX = figure[0].x;
    int maxX = figure[0].x;
    int minY = figure[0].y;
    int maxY = figure[0].y;
    for (int i = 1; i < 4; i++)
    {
        if (figure[i].x < minX)
        {
            minX = figure[i].x;
        }
        if (figure[i].x > maxX)
        {
            maxX = figure[i].x;
        }
        if (figure[i].y < minY)
        {
            minY = figure[i].y;
        }
        if (figure[i].y > maxY)
        {
            maxY = figure[i].y;
        }
    }
    center.x = minX + (maxX - minX) / 2;
    center.y = minY + (maxY - minY) / 2;
    return center;
}

CvPoint getSeqCenter(CvSeq* figure)
{
    CvPoint* point = (CvPoint*)cvGetSeqElem(figure, 0);
    int minX = point->x;
    int maxX = point->x;
    int minY = point->y;
    int maxY = point->y;
    for (int i = 1; i < figure->total; i++)
    {
        point = (CvPoint*)cvGetSeqElem(figure, i);
        if (point->x < minX)
        {
            minX = point->x;
        }
        if (point->x > maxX)
        {
            maxX = point->x;
        }
        if (point->y < minY)
        {
            minY = point->y;
        }
        if (point->y > maxY)
        {
            maxY = point->y;
        }
    }
    CvPoint center = cvPoint(minX + (maxX - minX) / 2, minY + (maxY - minY) / 2);
    return center;
}

CvPoint* getRectanglesCenters(CvSeq* figures)
{
    CvPoint centers[figures->total / 4];
    for (int i = 0; i < figures->total; i += 4)
    {
        CvPoint* points[4];
        points[0] = (CvPoint*)cvGetSeqElem(figures, i + 0);
        points[1] = (CvPoint*)cvGetSeqElem(figures, i + 1);
        points[2] = (CvPoint*)cvGetSeqElem(figures, i + 2);
        points[3] = (CvPoint*)cvGetSeqElem(figures, i + 3);
        centers[i / 4] = getRectangleCenter(*points[0], *points[1], *points[2], *points[3]);
    }
    return &(centers[0]);
}

int getCheckedPosition(CvSeq* checkboxes, CvSeq* checkedFigure)
{
    int result = checkboxes->total / 4 + 1;
    CvPoint* somePointOfCheckedFigure = (CvPoint*)cvGetSeqElem(checkedFigure, 0);

        CvPoint* sign[checkedFigure->total];
        for(int i = 0; i < checkedFigure->total; i++)
        {
            sign[i] = (CvPoint*)cvGetSeqElem(checkedFigure, i);
        }

    CvPoint* pointsOfCheckboxes[checkboxes->total / 4];
    for (int i = 0; i < checkboxes->total; i += 4)
    {
        pointsOfCheckboxes[i / 4] = (CvPoint*)cvGetSeqElem(checkboxes, i);
    }
    // Order by y axis
    CvPoint buf;
    for (int i = checkboxes->total / 4 - 1; i > 0; i--)
        for (int j = 0; j < i; j++)
        {
            if (pointsOfCheckboxes[j]->y > pointsOfCheckboxes[j + 1]->y)
            {
                buf = *(pointsOfCheckboxes[j]);
                *(pointsOfCheckboxes[j]) = *(pointsOfCheckboxes[j + 1]);
                *(pointsOfCheckboxes[j + 1]) = buf;
            }
        }

    //Similarity
    //HARDCODE: do it the right way
    int similar = -1;
    const int MIN_VECTOR_LENGTH_FOR_SIMILARITY = 25;
    for (int i = 0; i < checkboxes->total / 4; i++)
    {
        int x2 = pow(pointsOfCheckboxes[i]->x - somePointOfCheckedFigure->x, 2);
        int y2 = pow(pointsOfCheckboxes[i]->y - somePointOfCheckedFigure->y, 2);
        int vector = sqrt(x2 + y2);
        if (vector < MIN_VECTOR_LENGTH_FOR_SIMILARITY)
        {
            similar = i + 1;
            break;
        }
    }

    if (similar > 0)
        result = similar;
    else
        for (int i = 0; i < checkboxes->total / 4 - 1; i++)
        {
            CvPoint* somePointOfCheckbox = pointsOfCheckboxes[i];
            CvPoint* somePointOfNextCheckbox = pointsOfCheckboxes[i + 1];

            if ( abs(somePointOfCheckbox->y - somePointOfCheckedFigure->y) < 10 )    //TODO: remove magic number!
            {
                result = i + 1;
                break;
            }
            else
            {
                if (0 == i && somePointOfCheckedFigure->y < somePointOfCheckbox->y)
                {
                    result = 1;
                    break;
                }
                else
                {
                    if (somePointOfCheckedFigure->y > somePointOfCheckbox->y && somePointOfCheckedFigure->y < somePointOfNextCheckbox->y)
                    {
                        result = i + 2;
                        break;
                    }
                    else
                    {
                        if (i == checkboxes->total / 4 - 1)
                        {
                            result = checkboxes->total / 4 + 1;
                            break;
                        }
                    }
                }
            }
        }

    return result;
}

int getTotalConsideringSimilarityOfSequences(CvSeq* checkboxes, CvSeq* checkedFigure)
{
    int result = checkboxes->total / 4;
    CvPoint* checkboxesCenters = getRectanglesCenters(checkboxes);
    CvPoint checkedFigureCenter = getSeqCenter(checkedFigure);

    //HARDCODE: creating fixed sized copy of array, because var sized array
    // rewrites while next for-cycle interation
    CvPoint tmpArr[10];
    for (int i = 0; i < result; i++)
    {
        tmpArr[i] = checkboxesCenters[i];
    }

    //HARDCODE: do it the right way
    int similar = 0;
    const int MIN_VECTOR_LENGTH_FOR_SIMILARITY = 25;
    for (int i = 0; i < result; i++)
    {
        int x2 = pow(tmpArr[i].x - checkedFigureCenter.x, 2);
        int y2 = pow(tmpArr[i].y - checkedFigureCenter.y, 2);
        int vector = sqrt(x2 + y2);
        if (vector < MIN_VECTOR_LENGTH_FOR_SIMILARITY)
        {
            similar = 1;
            break;
        }
    }

    return similar ? result : (result + 1);
}

void getQuestionResults(IplImage* image, CvMemStorage* storage, int thresh, char* wndname, int* resultOut)
{
    //int result[2] = {-1, -1};
    //resultOut = malloc(2 * sizeof(int));

    IplImage* backup = copyImage(image);
    image = avoidThreshold(backup, THRESHOLD_LEVEL_TO_ALLOCATE_CHECKED_CHECKBOX);

/*
window!
    cvNamedWindow("test", 1);
    cvShowImage("test", image);
    cvWaitKey(0);
    cvDestroyWindow("test");
*/

    CvSeq* checkedFigure = getCheckedSign(image, storage);

    CvSeq* checkboxes = findSquares4(backup, storage, 0, MIN_CHECKBOX_AREA, thresh);
    checkboxes = filterSimilarSquares( checkboxes, SIMILAR_CHECKBOXES_DISTANCE, storage );
    checkboxes = filterImageBorderSquare( checkboxes, storage );

/*
window!
    drawSquares(&backup, checkboxes, wndname);
    cvWaitKey(0);
*/

    if (checkboxes && checkboxes->total > 0)
    {

        IplImage* testImg = cvCreateImage(cvSize(image->width, image->height), image->depth, image->nChannels);
/*
window!
        drawSquares(&testImg, checkboxes, wndname);
        cvWaitKey(0);
*/

        //printf("\n");
        if (checkedFigure && checkedFigure->total > 0)
        {
            int total = getTotalConsideringSimilarityOfSequences(checkboxes, checkedFigure);
            *(resultOut + 1) = total;
            int checkedPosition = getCheckedPosition(checkboxes, checkedFigure);
            *(resultOut + 0) = checkedPosition;
            //printf("%d / %d\n", result[0], result[1]);
        }
        else
        {
            *(resultOut + 0) = NO_ANSWER; // none
            *(resultOut + 1) = NO_ANSWER;
            //printf("none\n");
        }
    }
    else
    {
        *(resultOut + 0) = TEXT_ANSWER;
        *(resultOut + 1) = TEXT_ANSWER; //text question
        //printf("text question\n");
    }

    //return result;  // the first element is a checked checkbox, the second is total checkboxes
}

//
// result: 
//  [
//      [1, 5],
//      [3, 4],
//      ...
//      [{checked index}, {total vatiants}],
//      [-1, {any}], => text answers expected
//      ...
//  ]
//
void recognize( IplImage* tmpImg, CvSeq* squares, CvMemStorage* storage, int thresh, char* wndname, int** results, int* totalQuestions )
{
    //loadConfig();
    int mainBorderWidth = tmpImg->width;
    const double BASE_WIDTH_RATIO_QUESTION_TO_PLACEHOLDER = 760. / 720.;    //462. / 415.;
    const double ADMISSION_RATIO = 0.02;
    IplImage* subimage, * subimage2;

    *totalQuestions = squares->total / 4 - 1;
    //int* results[squares->total];
    *results = (int*)malloc(2 * squares->total / 4 * sizeof(int));
    for (int i = 0; i < squares->total; i += 4)
    {
        CvPoint* points[4];
        for (int j = 0; j < 4; j++)
        {
            points[j] = (CvPoint*)cvGetSeqElem(squares, i + j);
        }
        //normalizeRectanglePoints(points);
        int width = points[1]->x - points[0]->x;

        // If width like question's width
        double questionWidthRatio = (double)mainBorderWidth / (double)width;
        if ( questionWidthRatio > (BASE_WIDTH_RATIO_QUESTION_TO_PLACEHOLDER - ADMISSION_RATIO) &&
             questionWidthRatio < (BASE_WIDTH_RATIO_QUESTION_TO_PLACEHOLDER + ADMISSION_RATIO)) // origin 455 - 40 (px) = 415 (px)
        {
            subimage = getSubimage(tmpImg, cvRect( points[0]->x + 0.88 * width, points[0]->y, 0.12 * width, points[2]->y - points[1]->y ));
/*
window!
            cvNamedWindow( "Checkboxes", 1 );
            cvShowImage( "Checkboxes", subimage );
            cvWaitKey(0);
*/

            getQuestionResults(subimage, storage, thresh, wndname, *results + 2 * i / 4 );
/*
window!
            cvDestroyWindow("Checkboxes");
*/

            cvCircle(tmpImg, *(points[0]), 10, CV_RGB(255, 0, 0), 1, 8, 0);
            cvCircle(tmpImg, *(points[1]), 10, CV_RGB(0, 255, 0), 1, 8, 0);
            cvCircle(tmpImg, *(points[2]), 10, CV_RGB(0, 0, 255), 1, 8, 0);
            cvCircle(tmpImg, *(points[3]), 10, CV_RGB(0, 255, 255), 1, 8, 0);
        }
    }
    //return results;
}

int isTextAnswer(int* vals)
{
    return TEXT_ANSWER == *(vals + 0);
}

int isNoAnswer(int* vals)
{
    return NO_ANSWER == *(vals + 1);
}

void jsonOutputResults(const char* filename, int** results, int totalQuestions)
{
    char* answers;
    for (int i = totalQuestions - 1; i >= 0; i--)
    {
        if (isTextAnswer(*results + 2 * i))
        {
            answers = strcat(answers, "'text answer expected'");
        }
        else
        {
            if (isNoAnswer(*results + 2 * i))
            {
                answers = strcat(answers, "'no answer checked'");
            }
            else
            {
                answers = strcat(answers, " { ");
                //strcat(answers, itoa(*(*results + 2 * i + 0)));
                char b[2];
                sprintf(b, "d%", *(*results + 2 * i + 0));
                answers = strcat(answers, b);
                
                answers = strcat(answers, " , ");
                //strcat(answers, itoa(*(*results + 2 * i + 1)));
                sprintf(b, "d%", *(*results + 2 * i + 0));
                answers = strcat(answers, b);

                answers = strcat(answers, " } ");
                //printf("replied %d of %d", *(*results + 2 * i + 0), *(*results + 2 * i + 1));
            }
        }
        if (i != totalQuestions - 1)
        {
            strcat(answers, ", ");
        }
        //printf("\n");
    }

    char* resultAsJSON = " { '";
    resultAsJSON = strcat(resultAsJSON, "++");//filename
    resultAsJSON = strcat(resultAsJSON, "' : [");
    resultAsJSON = strcat(resultAsJSON, answers);
    resultAsJSON = strcat(resultAsJSON, "] } ");
    printf(resultAsJSON);
}

void humanOutputResults(char* filename, int** results, int totalQuestions)
{
    printf("'%s' file results:\n", filename);
    for (int i = totalQuestions - 1; i >= 0; i--)
    {
        printf("\tQuestion %d: ", totalQuestions - i);
        if (isTextAnswer(*results + 2 * i))
        {
            printf("text answer expected");
        }
        else
        {
            if (isNoAnswer(*results + 2 * i))
            {
                printf("no answer checked");
            }
            else
            {
                printf("replied %d of %d", *(*results + 2 * i + 0), *(*results + 2 * i + 1));
            }
        }
        printf("\n");
    }
    printf("\n");
}

void defaultOutputResults(char* filename, int** results, int totalQuestions)
{
    printf("%s:\n", filename);
    for (int i = totalQuestions - 1; i >= 0; i--)
    {
        printf("%d / %d\n", *(*results + 2 * i + 0), *(*results + 2 * i + 1));
    }
    printf("\n");
}

const int DEFAULT = 0;
const int JSON = 1;
const int HUMAN = 2;

int translateFormat(const char* format)
{
    if (0 == strcmp(format, "json"))
        return JSON;
    if (0 == strcmp(format, "human"))
        return HUMAN;
    return DEFAULT;
}

void outputResults(const char* filename, int** results, int totalQuestions, const char* format)
{
    int f = translateFormat(format);
    switch (f)
    {
        case 1:
            jsonOutputResults(filename, results, totalQuestions);
            break;
        case 0:
            defaultOutputResults(filename, results, totalQuestions);
            break;
        default:    //HUMAN
            humanOutputResults(filename, results, totalQuestions);
    }
}

/* 
 * Profiler
 */

time_t  profilerStartAt = -1,
        profilerStopAt = -1;

void startTimeProfiling()
{
    profilerStartAt = clock();  //time(NULL);
}

void stopTimeProfiling()
{
    profilerStopAt = clock();   //time(NULL);
}

void outputProfileInfo()
{
   printf("Recognition completed in %.3f seconds \n", (float)(profilerStopAt - profilerStartAt) / CLOCKS_PER_SEC);
}

