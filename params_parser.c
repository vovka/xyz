#include <string.h>
#include <stdlib.h>

parseFilenameParam(int argc, char** argv, const char** filename)
{
    if (argc > 1)
    {
        *filename = argv[1];
    }
    else
    {
        printf("Incorrect usage. Put filename as a first parameter. \n");
        return 0;
    }
    return 1;
}

int isExistParam(char** argv, int* paramIndex, char** namesArr, int namesCount)
{
    int equal = -1;
    for (int i = 0; i < namesCount; i++)
    {
        equal = strcmp(namesArr[i], argv[*paramIndex]);
        if (0 == equal)
        {
            break;
        }
    }
    if ( 0 == equal )
    {
        return 1;
    }
    return 0;
}

int getNamedIntParam(char** argv, int* paramIndex, int* val, char** namesArr, int namesCount)
{
    int equal = -1;
    for (int i = 0; i < namesCount; i++)
    {
        equal = strcmp(namesArr[i], argv[*paramIndex]);
        if (0 == equal)
        {
            break;
        }
    }
    if ( 0 == equal )
    {
        (*paramIndex)++;
        *val = atoi(argv[*paramIndex]);
        return 1;
    }
    return 0;
}

double getNamedDoubleParam(char** argv, int* paramIndex, double* val, char** namesArr, int namesCount)
{
    int equal = -1;
    for (int i = 0; i < namesCount; i++)
    {
        equal = strcmp(namesArr[i], argv[*paramIndex]);
        if (0 == equal)
        {
/*
            int length = 0;
            for (;*(argv[*paramIndex + 1] + length) != 0; length++){}
            char strParam[length];
            for (int j = 0; j < length; j++)
            {
                strParam[j] = *(argv[*paramIndex + 1] + j);
            }
            char buf[length];
            for (int j = 0; j < length; j++)
            {
                buf[j] = strParam[j];
            }
            
            int l = strlen(buf);
*/
            (*paramIndex)++;
            //*val = atof(argv[*paramIndex]);
            //double r = strtof(strParam);
            double r = strtold(argv[*paramIndex], &(argv[*paramIndex]));
            *val = r;
            return 1;
        }
    }
    return 0;
}

int parseDialogParam(char* param, int* dialog)
{
    int equal = strcmp("-dlg", param);
    if ( 0 == equal )
    {
        *dialog = 1;
        return 1;
    }
    return 0;
}

int parseOutputResultsParam(char** argv, int* paramIndex, const char** outputResultsAs)
{
    int equal = strcmp("-of", argv[*paramIndex]);
    if (0 != equal)
    {
        equal = strcmp("--output-format", argv[*paramIndex]);
    }
    if ( 0 == equal )
    {
        (*paramIndex)++;
        *outputResultsAs = argv[*paramIndex];
        return 1;
    }
    return 0;
}

int parseMinSquaresAreaParam(char** argv, int* paramIndex, int* minSquaresArea)
{
    const int namesCount = 2;
    char* paramsNames[] = {"-msa", "--min-squares-area"};
    return getNamedIntParam(    argv,
                                paramIndex,
                                minSquaresArea,
                                paramsNames,
                                namesCount);
}

int parseThresholdLevelToAllocateCheckedCheckboxes(char** argv, int* paramIndex, int* thresholdLevelToAllocateCheckedCheckboxes)
{
    const int namesCount = 2;
    char* paramsNames[] = {"-tlcc", "--threshold-level-to-allocate-checked-checkboxes"};
    return getNamedIntParam(    argv,
                                paramIndex,
                                thresholdLevelToAllocateCheckedCheckboxes,
                                paramsNames,
                                namesCount);
}

int parseDebugModeParam(char** argv, int* paramIndex, int* debug)
{
    const int namesCount = 2;
    char* paramsNames[] = {"-dbg", "--debug"};
    *debug = isExistParam(  argv,
                            paramIndex,
                            paramsNames,
                            namesCount);
    return *debug;
}

int parseThresholdToAllocateSquaresParam(char** argv, int* paramIndex, int* thresh)
{
    const int namesCount = 2;
    char* paramsNames[] = {"-t", "--threshold"};
    return getNamedIntParam(    argv,
                                paramIndex,
                                thresh,
                                paramsNames,
                                namesCount);
}

int parseMinCheckboxAreaParam(char** argv, int* paramIndex, int* minCheckboxArea)
{
    const int namesCount = 2;
    char* paramsNames[] = {"-mca", "--min-checkbox-area"};
    return getNamedIntParam(    argv,
                                paramIndex,
                                minCheckboxArea,
                                paramsNames,
                                namesCount);
}

int parseQuestionToOuterRectWidthRatioParam(char** argv, int* paramIndex, double* questionToOuterRectWidthRatio)
{
    const int namesCount = 2;
    char* paramsNames[] = {"-qtorwr", "--question-to-outer-rect-width-ratio"};
    return getNamedDoubleParam(    argv,
                                paramIndex,
                                questionToOuterRectWidthRatio,
                                paramsNames,
                                namesCount);
}

int parseCheckboxesAreaWidthToQuestionWidthRatio(char** argv, int* paramIndex, double* checkboxesAreaWidthToQuestionWidthRatio)
{
    const int namesCount = 2;
    char* paramsNames[] = {"-cawtqwr", "--checkboxes-area-width-to-question-width-ratio"};
    return getNamedDoubleParam(    argv,
                                paramIndex,
                                checkboxesAreaWidthToQuestionWidthRatio,
                                paramsNames,
                                namesCount);
}

int parseSimilarCheckboxesDistance(char** argv, int* paramIndex, int* similarCheckboxesDistance)
{
    const int namesCount = 2;
    char* paramsNames[] = {"-scd", "--similar-checkboxes-distance"};
    return getNamedIntParam(    argv,
                                paramIndex,
                                similarCheckboxesDistance,
                                paramsNames,
                                namesCount);
}

int parseSimilarSquaresDistance(char** argv, int* paramIndex, int* similarSquaresDistance)
{
    const int namesCount = 2;
    char* paramsNames[] = {"-ssd", "--similar-squares-distance"};
    return getNamedIntParam(    argv,
                                paramIndex,
                                similarSquaresDistance,
                                paramsNames,
                                namesCount);
}

int parseMinVectorLengthForSimilarity(char** argv, int* paramIndex, int* minVectorLengthForSimilarity)
{
    const int namesCount = 2;
    char* paramsNames[] = {"-mvlfs", "--min-vector-length-for-similarity"};
    return getNamedIntParam(    argv,
                                paramIndex,
                                minVectorLengthForSimilarity,
                                paramsNames,
                                namesCount);
}

int parseMinYLengthForSimilarity(char** argv, int* paramIndex, int* minYLengthForSimilarity)
{
    const int namesCount = 2;
    char* paramsNames[] = {"-mYlfs", "--min-y-length-for-similarity"};
    return getNamedIntParam(    argv,
                                paramIndex,
                                minYLengthForSimilarity,
                                paramsNames,
                                namesCount);
}

int parseCmdParameters(
        int argc,
        char** argv,
        const char** filename,
        int* dialog,
        const char** outputResultsAs,
        int* minSquaresArea,
        int* thresholdLevelToAllocateCheckedCheckboxes,
        int* debug,
        int* thresh, 
        int* minCheckboxArea,
        double* questionToOuterRectWidthRatio,
        double* checkboxesAreaWidthToQuestionWidthRatio,
        int* similarCheckboxesDistance,
        int* similarSquaresDistance,
        int* minVectorLengthForSimilarity,
        int* minYLengthForSimilarity
)
{
    int res = parseFilenameParam(argc, argv, filename);
    if (res)
    {
        for (int i = 1; i < argc; i++)
        {
            res = res && (
                parseDialogParam(argv[i], dialog) ||
                parseOutputResultsParam(argv, &i, outputResultsAs) ||
                parseMinSquaresAreaParam(argv, &i, minSquaresArea) ||
                parseThresholdLevelToAllocateCheckedCheckboxes(argv, &i, thresholdLevelToAllocateCheckedCheckboxes) ||
                parseThresholdToAllocateSquaresParam(argv, &i, thresh) ||
                parseMinCheckboxAreaParam(argv, &i, minCheckboxArea) ||
                parseQuestionToOuterRectWidthRatioParam(argv, &i, questionToOuterRectWidthRatio) ||
                parseCheckboxesAreaWidthToQuestionWidthRatio(argv, &i, checkboxesAreaWidthToQuestionWidthRatio) ||
                parseSimilarCheckboxesDistance(argv, &i, similarCheckboxesDistance) ||
                parseSimilarSquaresDistance(argv, &i, similarSquaresDistance) ||
                parseMinVectorLengthForSimilarity(argv, &i, minVectorLengthForSimilarity) ||
                parseMinYLengthForSimilarity(argv, &i, minYLengthForSimilarity) ||
                parseDebugModeParam(argv, &i, debug) || /* should always be last */
                1
            );
        }
    }
    return res;
}
