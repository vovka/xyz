#include "helpers.h"

/*
const int THRESHOLD_LEVEL_TO_ALLOCATE_CHECKED_CHECKBOX = 100;//162;
//const int THRESHOLD_LEVEL_TO_ALLOCATE_ALL_CHECKBOXES = 300;//231;
const int MIN_CHECKBOX_AREA = 100;
const int SIMILAR_CHECKBOXES_DISTANCE = 10;
*/

// the function draws all the squares in the image
void drawSquares( IplImage** iplimgCanvas, CvSeq* squares, char* chpWndname)
{
    CvSeqReader reader;
    IplImage* cpy = cvCloneImage( *iplimgCanvas );
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
*/
    cvShowImage( chpWndname, cpy );
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
CvSeq* findSquares4( IplImage* iplimgSrc, CvMemStorage* tmpStorage, char* wndName, int minArea, int thresh )
{
    CvSeq* contours;
    int i, c, l, N = 15;   //default was 11
    CvSize sz = cvSize( iplimgSrc->width & -2, iplimgSrc->height & -2 );
    IplImage* timg = cvCloneImage( iplimgSrc ); // make a copy of input image
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
                        CvPoint* arrcvpntpPoints[4];
                        for( i = 0; i < 4; i++ )
                        {
                            arrcvpntpPoints[i] = (CvPoint*)cvGetSeqElem( result, i );
                        }
                        normalizeRectanglePoints(arrcvpntpPoints);
                        for( i = 0; i < 4; i++ )
                        {
                            cvSeqPush( squares, arrcvpntpPoints[i]);
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

CvSeq* filterSimilarSquares(CvSeq* squares, int similarityDistance, CvMemStorage* cvmemStorage)
{
    CvSeq* filteredSquares = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint), cvmemStorage );
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

CvSeq* filterImageBorderSquare(CvSeq* squares, CvMemStorage* cvmemStorage)
{
    CvSeq* filteredSquares = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint), cvmemStorage );
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
    CvPoint* arrcvpntpPoints[4];
    arrcvpntpPoints[0] = (CvPoint*)cvGetSeqElem(squares, 0);
    arrcvpntpPoints[1] = (CvPoint*)cvGetSeqElem(squares, 1);
    arrcvpntpPoints[2] = (CvPoint*)cvGetSeqElem(squares, 2);
    arrcvpntpPoints[3] = (CvPoint*)cvGetSeqElem(squares, 3);

    double vectorLength = sqrt(arrcvpntpPoints[0]->x * arrcvpntpPoints[0]->x + arrcvpntpPoints[0]->y * arrcvpntpPoints[0]->y);
    for (int i = 1; i< 4; i++)
    {
        double tmpVectorLength = sqrt(arrcvpntpPoints[i]->x * arrcvpntpPoints[i]->x + arrcvpntpPoints[i]->y * arrcvpntpPoints[i]->y);
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
                arrcvpntpPoints[j] = tmpPoints[j];
            }
        }
    }

    normalizeRectanglePoints(arrcvpntpPoints);

    return arrcvpntpPoints;
}

void normalizeRectanglePoints(CvPoint** cvpntppPoints)
{
    // Точки могут располагаться в любом порядке, т.е. начинаться, например, с
    // правой нижней точки и идти против часовой, или по часовой. Т.е. с любой
    // позиции в любом направлении. Чтобы упорядочить их (с левой верхней по
    // часовой) нахожу центр фигуры, провожу через центр дополнительную систему
    // кординат (представляемо). Таким образом точки будут расположены в разных
    // четвертях плоскости.
    int minX = cvpntppPoints[0]->x;
    int maxX = cvpntppPoints[0]->x;
    int minY = cvpntppPoints[0]->y;
    int maxY = cvpntppPoints[0]->y;
    for (int i = 1; i < 4; i++)
    {
        if (cvpntppPoints[i]->x < minX)
        {
            minX = cvpntppPoints[i]->x;
        }
        if (cvpntppPoints[i]->x > maxX)
        {
            maxX = cvpntppPoints[i]->x;
        }
        if (cvpntppPoints[i]->y < minY)
        {
            minY = cvpntppPoints[i]->y;
        }
        if (cvpntppPoints[i]->y > maxY)
        {
            maxY = cvpntppPoints[i]->y;
        }
    }
    int abscissa = minX + (int)( (double)(maxX - minX) / 2. );
    int ordinate = minY + (int)( (double)(maxY - minY) / 2. );
    for (int i = 0; i < 4; i++)
    {
        CvPoint* buf;
        // Top left point
        if (cvpntppPoints[i]->x < abscissa && cvpntppPoints[i]->y < ordinate)
        {
            buf = cvpntppPoints[0];
            cvpntppPoints[0] = cvpntppPoints[i];
            cvpntppPoints[i] = buf;
        }
        // Top right point
        if (cvpntppPoints[i]->x > abscissa && cvpntppPoints[i]->y < ordinate)
        {
            buf = cvpntppPoints[1];
            cvpntppPoints[1] = cvpntppPoints[i];
            cvpntppPoints[i] = buf;
        }
        // Bottom right point
        if (cvpntppPoints[i]->x > abscissa && cvpntppPoints[i]->y > ordinate)
        {
            buf = cvpntppPoints[2];
            cvpntppPoints[2] = cvpntppPoints[i];
            cvpntppPoints[i] = buf;
        }
        // Bottom left point
        if (cvpntppPoints[i]->x < abscissa && cvpntppPoints[i]->y > ordinate)
        {
            buf = cvpntppPoints[3];
            cvpntppPoints[3] = cvpntppPoints[i];
            cvpntppPoints[i] = buf;
        }
    }
}

IplImage* cropImage( IplImage** iplimgSrc, CvPoint*** outerRectangle )
{
    /* sets the Region of Interest
       Note that the rectangle area has to be __INSIDE__ the image */
    cvSetImageROI(  *iplimgSrc,
                    cvRect(     (*outerRectangle)[0]->x,
                                (*outerRectangle)[0]->y,
                                (*outerRectangle)[2]->x - (*outerRectangle)[0]->x,
                                (*outerRectangle)[2]->y - (*outerRectangle)[0]->y));
    /* create destination image
       Note that cvGetSize will return the width and the height of ROI */
    IplImage* img2 = cvCreateImage(    cvGetSize(*iplimgSrc),
                                       (*iplimgSrc)->depth,
                                       (*iplimgSrc)->nChannels);
    /* copy subimage */
    cvCopy(*iplimgSrc, img2, NULL);
    /* always reset the Region of Interest */
    cvResetImageROI(*iplimgSrc);
    return img2;
}

IplImage* getSubimage(IplImage* iplimgSrc, CvRect roi)
{
    IplImage *result;
    // set ROI, you may use following two funs:
    //cvSetImageROI( image, cvRect( 0, 0, image->width, image->height ));
    cvSetImageROI(iplimgSrc, roi);
    // sub-image
    result = cvCreateImage( cvGetSize(iplimgSrc), iplimgSrc->depth, iplimgSrc->nChannels );
    cvCopy(iplimgSrc, result, NULL);
    cvResetImageROI(iplimgSrc); // release image ROI
    return result;
}

IplImage* copyImage(IplImage* iplimgSrc)
{
    return cvClone(iplimgSrc);
}

IplImage* avoidThreshold(IplImage* iplimgSrc, int iThresholdLevel)
{
    IplImage* iplimgResult = cvCreateImage(cvSize(iplimgSrc->width, iplimgSrc->height), 8, 1);

    CvSize szSrc = cvSize( iplimgSrc->width , iplimgSrc->height );
    IplImage* iplimgTimg = cvCloneImage( iplimgSrc ); // make a copy of input image
    cvSetImageROI( iplimgTimg, cvRect( 0, 0, szSrc.width, szSrc.height ));
    IplImage* iplimgPyr = cvCreateImage( cvSize(szSrc.width/2, szSrc.height/2), 8, 3 );
    cvPyrDown( iplimgTimg, iplimgPyr, 7 );
    cvPyrUp( iplimgPyr, iplimgTimg, 7 );
    IplImage* iplimgTgray = cvCreateImage( szSrc, 8, 1 );
    cvSetImageCOI( iplimgTimg, 1 );
    cvCopy( iplimgTimg, iplimgTgray, 0 );
    cvThreshold( iplimgTgray, iplimgResult, iThresholdLevel, 255, CV_THRESH_BINARY );
    return iplimgResult;
}

CvSeq* getCheckedSign(IplImage* iplimgSrc, CvMemStorage* cvmempStorage)
{
    CvSeq* seqpContours;
    cvFindContours( iplimgSrc, cvmempStorage, &seqpContours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
    return filterImageBorderSquare( seqpContours, cvmempStorage );
}

CvPoint getRectangleCenter(CvPoint pntPoint0, CvPoint pntPoint1, CvPoint pntPoint2, CvPoint pntPoint3 )
{
    //TODO: normalize it
    CvPoint arrpntFigure[4];
    arrpntFigure[0] = pntPoint0;
    arrpntFigure[1] = pntPoint1;
    arrpntFigure[2] = pntPoint2;
    arrpntFigure[3] = pntPoint3;
    CvPoint pntCenter;
    int iMinX = arrpntFigure[0].x;
    int iMaxX = arrpntFigure[0].x;
    int iMinY = arrpntFigure[0].y;
    int iMaxY = arrpntFigure[0].y;
    for (int i = 1; i < 4; i++)
    {
        if (arrpntFigure[i].x < iMinX)
        {
            iMinX = arrpntFigure[i].x;
        }
        if (arrpntFigure[i].x > iMaxX)
        {
            iMaxX = arrpntFigure[i].x;
        }
        if (arrpntFigure[i].y < iMinY)
        {
            iMinY = arrpntFigure[i].y;
        }
        if (arrpntFigure[i].y > iMaxY)
        {
            iMaxY = arrpntFigure[i].y;
        }
    }
    pntCenter.x = iMinX + (iMaxX - iMinX) / 2;
    pntCenter.y = iMinY + (iMaxY - iMinY) / 2;
    return pntCenter;
}

CvPoint getSeqCenter(CvSeq* seqpFigure)
{
    CvPoint* pntpPoint = (CvPoint*)cvGetSeqElem(seqpFigure, 0);
    int iMinX = pntpPoint->x;
    int iMaxX = pntpPoint->x;
    int iMinY = pntpPoint->y;
    int iMaxY = pntpPoint->y;
    for (int i = 1; i < seqpFigure->total; i++)
    {
        pntpPoint = (CvPoint*)cvGetSeqElem(seqpFigure, i);
        if (pntpPoint->x < iMinX)
        {
            iMinX = pntpPoint->x;
        }
        if (pntpPoint->x > iMaxX)
        {
            iMaxX = pntpPoint->x;
        }
        if (pntpPoint->y < iMinY)
        {
            iMinY = pntpPoint->y;
        }
        if (pntpPoint->y > iMaxY)
        {
            iMaxY = pntpPoint->y;
        }
    }
    CvPoint pntCenter = cvPoint(iMinX + (iMaxX - iMinX) / 2, iMinY + (iMaxY - iMinY) / 2);
    return pntCenter;
}

CvPoint* getRectanglesCenters(CvSeq* seqpFigures)
{
    CvPoint arrpntCenters[seqpFigures->total / 4];
    for (int i = 0; i < seqpFigures->total; i += 4)
    {
        CvPoint* arrcvpntpPoints[4];
        arrcvpntpPoints[0] = (CvPoint*)cvGetSeqElem(seqpFigures, i + 0);
        arrcvpntpPoints[1] = (CvPoint*)cvGetSeqElem(seqpFigures, i + 1);
        arrcvpntpPoints[2] = (CvPoint*)cvGetSeqElem(seqpFigures, i + 2);
        arrcvpntpPoints[3] = (CvPoint*)cvGetSeqElem(seqpFigures, i + 3);
        arrpntCenters[i / 4] = getRectangleCenter(*arrcvpntpPoints[0], *arrcvpntpPoints[1], *arrcvpntpPoints[2], *arrcvpntpPoints[3]);
    }
    return &(arrpntCenters[0]);
}

int findCheckboxSimilarToChoiceSign(CvSeq* seqpCheckboxes, CvPoint* arrpntpPointsOfCheckboxes[], CvPoint* pntpSomePointOfCheckedFigure)
{
    int iSimilar = -1;
    //const int MIN_VECTOR_LENGTH_FOR_SIMILARITY = 40;
    for (int i = 0; i < seqpCheckboxes->total / 4; i++)
    {
        int x2 = pow(arrpntpPointsOfCheckboxes[i]->x - pntpSomePointOfCheckedFigure->x, 2);
        int y2 = pow(arrpntpPointsOfCheckboxes[i]->y - pntpSomePointOfCheckedFigure->y, 2);
        int vector = sqrt(x2 + y2);
        if (vector < MIN_VECTOR_LENGTH_FOR_SIMILARITY)
        {
            iSimilar = i + 1;
            break;
        }
    }
    return iSimilar;
}

int getCheckedPosition(CvSeq* seqpCheckboxes, CvSeq* seqCheckedFigure)
{
    int iResult = seqpCheckboxes->total / 4 + 1;
    CvPoint* pntpSomePointOfCheckedFigure = (CvPoint*)cvGetSeqElem(seqCheckedFigure, 0);

/*
        CvPoint* sign[seqCheckedFigure->total];
        for(int i = 0; i < seqCheckedFigure->total; i++)
        {
            sign[i] = (CvPoint*)cvGetSeqElem(seqCheckedFigure, i);
        }
*/

    CvPoint* arrpntpPointsOfCheckboxes[seqpCheckboxes->total / 4];
    for (int i = 0; i < seqpCheckboxes->total; i += 4)
    {
        arrpntpPointsOfCheckboxes[i / 4] = (CvPoint*)cvGetSeqElem(seqpCheckboxes, i);
    }
    // Order by y axis
    CvPoint pntBuf;
    for (int i = seqpCheckboxes->total / 4 - 1; i > 0; i--)
        for (int j = 0; j < i; j++)
        {
            if (arrpntpPointsOfCheckboxes[j]->y > arrpntpPointsOfCheckboxes[j + 1]->y)
            {
                pntBuf = *(arrpntpPointsOfCheckboxes[j]);
                *(arrpntpPointsOfCheckboxes[j]) = *(arrpntpPointsOfCheckboxes[j + 1]);
                *(arrpntpPointsOfCheckboxes[j + 1]) = pntBuf;
            }
        }

    //Similarity
    int iSimilar = findCheckboxSimilarToChoiceSign(seqpCheckboxes, arrpntpPointsOfCheckboxes, pntpSomePointOfCheckedFigure);

    if (iSimilar > 0)
        iResult = iSimilar;
    else
        for (int i = 0; i < seqpCheckboxes->total / 4 - 1; i++)
        {
            CvPoint* pntpSomePointOfCheckbox = arrpntpPointsOfCheckboxes[i];
            CvPoint* pntpSomePointOfNextCheckbox = arrpntpPointsOfCheckboxes[i + 1];

            if ( abs(pntpSomePointOfCheckbox->y - pntpSomePointOfCheckedFigure->y) < MIN_Y_LENGTH_FOR_SIMILARITY )
            {
                iResult = i + 1;
                break;
            }
            else
            {
                if (0 == i && pntpSomePointOfCheckedFigure->y < pntpSomePointOfCheckbox->y)
                {
                    iResult = 1;
                    break;
                }
                else
                {
                    if (pntpSomePointOfCheckedFigure->y > pntpSomePointOfCheckbox->y && pntpSomePointOfCheckedFigure->y < pntpSomePointOfNextCheckbox->y)
                    {
                        iResult = i + 2;
                        break;
                    }
                    else
                    {
                        if (i == seqpCheckboxes->total / 4 - 1)
                        {
                            iResult = seqpCheckboxes->total / 4 + 1;
                            break;
                        }
                    }
                }
            }
        }

    return iResult;
}

int getTotalConsideringSimilarityOfSequences(CvSeq* seqpCheckboxes, CvSeq* seqCheckedFigure)
{
    int iResult = seqpCheckboxes->total / 4;
    CvPoint* pntpCheckboxesCenters = getRectanglesCenters(seqpCheckboxes);
    CvPoint pntCheckedFigureCenter = getSeqCenter(seqCheckedFigure);

    /*
     * HARDCODE: creating fixed sized copy of array, because var sized array
     * rewrites on next for-cycle interation
     */
    CvPoint arrpntBuf[10];
    for (int i = 0; i < iResult; i++)
    {
        arrpntBuf[i] = pntpCheckboxesCenters[i];
    }

    //HARDCODE: do it the right way
    int iSimilar = 0;
    //const int MIN_VECTOR_LENGTH_FOR_SIMILARITY = 25;
    for (int i = 0; i < iResult; i++)
    {
        int x2 = pow(arrpntBuf[i].x - pntCheckedFigureCenter.x, 2);
        int y2 = pow(arrpntBuf[i].y - pntCheckedFigureCenter.y, 2);
        int vector = sqrt(x2 + y2);
        if (vector < MIN_VECTOR_LENGTH_FOR_SIMILARITY)
        {
            iSimilar = 1;
            break;
        }
    }

    return iSimilar ? iResult : (iResult + 1);
}

void getQuestionResults(    IplImage* iplimgSrc,
                            CvMemStorage* cvmempStorage,
                            int iThresh,
                            int iThresholdLevelToAllocateCheckedCheckboxes,
                            int bDebug,
                            char* chpWndname,
                            int* ipResultOut)
{
    IplImage* iplimgBackup = copyImage(iplimgSrc);
    iplimgSrc = avoidThreshold(iplimgBackup, iThresholdLevelToAllocateCheckedCheckboxes);

/*
window!
*/
    if (bDebug)
    {
        cvNamedWindow("test", 1);
        cvShowImage("test", iplimgSrc);
        cvWaitKey(0);
        cvDestroyWindow("test");
    }

    CvSeq* seqCheckedFigure = getCheckedSign(iplimgSrc, cvmempStorage);

    CvSeq* seqpCheckboxes = findSquares4(iplimgBackup, cvmempStorage, 0, MIN_CHECKBOX_AREA, iThresh);
    if (bDebug)
    {
        drawSquares(&iplimgBackup, seqpCheckboxes, chpWndname);
        cvWaitKey(0);
    }
    seqpCheckboxes = filterSimilarSquares( seqpCheckboxes, SIMILAR_CHECKBOXES_DISTANCE, cvmempStorage );
    seqpCheckboxes = filterImageBorderSquare( seqpCheckboxes, cvmempStorage );

/*
window!
*/
    if(bDebug)
    {
        drawSquares(&iplimgBackup, seqpCheckboxes, chpWndname);
        cvWaitKey(0);
    }

    if (seqpCheckboxes && seqpCheckboxes->total > 0)
    {

/*
window!
*/
        if (bDebug)
        {
            /* TODO: заменить следующую строку функцией copyImage и проверить
             * что будет
             */
            IplImage* iplimgCanvasCopy = cvCreateImage(cvSize(iplimgSrc->width, iplimgSrc->height), iplimgSrc->depth, iplimgSrc->nChannels);
            drawSquares(&iplimgCanvasCopy, seqpCheckboxes, chpWndname);
            cvWaitKey(0);
        }

        if (seqCheckedFigure && seqCheckedFigure->total > 0)
        {
            int iTotal = getTotalConsideringSimilarityOfSequences(seqpCheckboxes, seqCheckedFigure);
            *(ipResultOut + 1) = iTotal;
            int checkedPosition = getCheckedPosition(seqpCheckboxes, seqCheckedFigure);
            *(ipResultOut + 0) = checkedPosition;
        }
        else
        {
            *(ipResultOut + 0) = NO_ANSWER; // none
            *(ipResultOut + 1) = NO_ANSWER;
        }
    }
    else
    {
        *(ipResultOut + 0) = TEXT_ANSWER;
        *(ipResultOut + 1) = TEXT_ANSWER; //text question
    }
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
void recognize( IplImage* iplimgpTarget,
                CvSeq* seqpQuestionRectangles,
                CvMemStorage* cvmemStorage,
                int iThresh,
                int iThreshChecked,
                int bDebug,
                char* chpWndname,
                int** ippResults,
                int* ipTotalQuestions )
{
    //loadConfig();
    
    *ipTotalQuestions = seqpQuestionRectangles->total / 4;
    
    *ippResults = (int*)malloc(2 * seqpQuestionRectangles->total / 4 * sizeof(int));
    int iMainBorderWidth = iplimgpTarget->width;
    IplImage* iplimgSubimage;
    for (int i = 0; i < seqpQuestionRectangles->total; i += 4)
    {
        CvPoint* arrcvpntpPoints[4];
        for (int j = 0; j < 4; j++)
        {
            arrcvpntpPoints[j] = (CvPoint*)cvGetSeqElem(seqpQuestionRectangles, i + j);
        }
        
        int iQuestionBorderWidth = arrcvpntpPoints[1]->x - arrcvpntpPoints[0]->x;

        // If width like question's width
        double dQuestionWidthRatio = (double)iMainBorderWidth / (double)iQuestionBorderWidth;
        const double ADMISSION_RATIO = 0.02;
        if ( dQuestionWidthRatio > (QUESTION_TO_OUTER_RECT_WIDTH_RATIO - ADMISSION_RATIO) &&
             dQuestionWidthRatio < (QUESTION_TO_OUTER_RECT_WIDTH_RATIO + ADMISSION_RATIO)) // origin 455 - 40 (px) = 415 (px)
        {
            CvRect cvrectCheckboxesAreaRectangle = cvRect(
                arrcvpntpPoints[0]->x + (1 - CHECKBOXES_AREA_WIDTH_TO_QUESTION_WIDTH_RATIO) * iQuestionBorderWidth,
                arrcvpntpPoints[0]->y,
                CHECKBOXES_AREA_WIDTH_TO_QUESTION_WIDTH_RATIO * iQuestionBorderWidth,
                arrcvpntpPoints[2]->y - arrcvpntpPoints[1]->y );
            iplimgSubimage = getSubimage(iplimgpTarget, cvrectCheckboxesAreaRectangle);
/*
window!
*/
            if (bDebug)
            {
                cvNamedWindow( "Checkboxes", 1 );
                cvShowImage( "Checkboxes", iplimgSubimage );
                //cvWaitKey(0);
            }

            getQuestionResults(iplimgSubimage, cvmemStorage, iThresh, iThreshChecked, bDebug, chpWndname, *ippResults + 2 * i / 4 );
/*
window!
*/
            if (bDebug)
            {
                cvDestroyWindow("Checkboxes");
            }

            cvCircle(iplimgpTarget, *(arrcvpntpPoints[0]), 10, CV_RGB(255, 0, 0), 1, 8, 0);
            cvCircle(iplimgpTarget, *(arrcvpntpPoints[1]), 10, CV_RGB(0, 255, 0), 1, 8, 0);
            cvCircle(iplimgpTarget, *(arrcvpntpPoints[2]), 10, CV_RGB(0, 0, 255), 1, 8, 0);
            cvCircle(iplimgpTarget, *(arrcvpntpPoints[3]), 10, CV_RGB(0, 255, 255), 1, 8, 0);
        }
    }
}

