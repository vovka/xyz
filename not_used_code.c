/*
int rand255()
{
    int r = rand();
    int r255 = (int)((float)r / (float)RAND_MAX * 255);
    return r255;
}

*/
/*
int isIncludeSimilarCenter(CvPoint** rectanglesCenters, CvPoint* figureCenter, int numberOfCenters)
{
    int result = 0;
    const int MIN_VECTOR_LENGTH_FOR_SIMILARITY = 25;
    //for (int i = 0; i < getSizeOfCvPointArray(rectanglesCenters); i++)
    for (int i = 0; i < numberOfCenters; i++)
    {
        int vector = sqrt(
            pow((*rectanglesCenters)[i].x - figureCenter->x, 2) +
            pow((*rectanglesCenters)[i].y - figureCenter->y, 2)
        );
        if (vector < MIN_VECTOR_LENGTH_FOR_SIMILARITY)
        {
            result = 1;
            break;
        }
    }
    return result;
}
*/

/*
int getNewIndexOfFigure(CvPoint* checkboxes, CvPoint* checkedFigure)
{
    for (int i = 0; i < getSizeOfCvPointArray(checkboxes); i++)
    {
        if (checkedFigure->y < checkboxes[i].y)
        {
            return i;
        }
    }
    return -1;
}
*/

/*
CvPoint* mergeCheckboxesCenters(CvPoint* checkboxes, CvPoint* checkedFigure, int numberOfCenters)
{
    //HARDCODE:
    if (!isIncludeSimilarCenter(&checkboxes, checkedFigure, numberOfCenters))
    {
        int newLength = getSizeOfCvPointArray(checkboxes) + 1;
        CvPoint* centers[newLength];
        int shift = 0;
        int newIndex = getNewIndexOfFigure(checkboxes, checkedFigure);
        for (int i = 0; i < newLength; i++)
        {
            if ( i == newIndex )
            {
                centers[i] = checkedFigure;
                shift++;
            }
            else
            {
                centers[i + shift] = &checkboxes[i];
            }
        }
        return centers;
    }
    else
    {
        int newLength = getSizeOfCvPointArray(checkboxes);
        CvPoint* centers[newLength];
        //int shift = 0;
        int newIndex = getNewIndexOfFigure(checkboxes, checkedFigure);
        for (int i = 0; i < newLength; i++)
        {
            if ( i == newIndex )
            {
                centers[i] = checkedFigure;
                //shift++;
            }
            else
            {
                centers[i] = &checkboxes[i];
            }
        }
        return centers;
    }
}
*/

/*
CvPoint** mergeCheckboxesCentersSeq(CvSeq* checkboxes, CvSeq* checkedFigure)
{
    CvPoint* checkboxesCenters = getRectanglesCenters(checkboxes);
    CvPoint checkedCenter = getSeqCenter(checkedFigure);
    CvPoint** result = mergeCheckboxesCenters(checkboxesCenters, &checkedCenter, checkboxes->total);
    return result;
}
*/

/*
int getSizeOfCvPointArray(CvPoint* totalCheckboxesCenters[])
{
    return sizeof(*totalCheckboxesCenters) / sizeof(CvPoint);
}
*/

/*
// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
void findContours( IplImage* tmpImg )
{
    IplImage* img0 = cvCreateImage(cvSize(tmpImg->width, tmpImg->height), (int)IPL_DEPTH_32F, 3);
    cvConvertScale(tmpImg, img0, 1, 0);
    IplImage* templ = cvCreateImage( cvSize( 15, 15 ), (int)IPL_DEPTH_32F, 3 );
    cvRectangle(templ, cvPoint(0, 0), cvPoint(15, 15), CV_RGB(255, 255, 255), -1, 8, 0);
    cvCircle(templ, cvPoint(7, 7), 5, CV_RGB(0, 0, 0), 1, 1, 0);
/*
window!
*
    cvShowImage( "Template", templ );
    // Allocate Output Images:
    IplImage* ftmp[CV_TM_CCOEFF_NORMED + 1]; // ftmp will hold results
    int iwidth = img0->width - templ->width + 1;
    int iheight = img0->height - templ->height + 1;
    for(int i = CV_TM_SQDIFF; i <= CV_TM_CCOEFF_NORMED; ++i){
            ftmp[i]= cvCreateImage( cvSize( iwidth, iheight ), IPL_DEPTH_32F, 1 );
    }
    // Do the matching of the template with the image
    for( int i = CV_TM_SQDIFF; i <= CV_TM_CCOEFF_NORMED; ++i ){
        cvMatchTemplate( img0, templ, ftmp[i], i );
/*
window!
*
        cvShowImage( "Match", ftmp[i] );
        cvNormalize( ftmp[i], ftmp[i], 1, 0, CV_MINMAX, NULL );
/*
window!
*
        cvShowImage( "Match Normalized", ftmp[i] );
        cvWaitKey(0);
    }
/*
window!
*
    cvDestroyWindow("Match");
    cvDestroyWindow("Match Normalized");
    cvDestroyWindow("Template");
}

*/

