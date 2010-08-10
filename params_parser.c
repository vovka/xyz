int parseFilenameParam(int argc, char** argv, const char** filename)
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

int parseCmdParameters(
        int argc,
        char** argv,
        const char** filename,
        int* dialog,
        const char** outputResultsAs,
        int* minSquaresArea,
        int* thresholdLevelToAllocateCheckedCheckboxes,
        int* debug,
        int* thresh)
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
                parseDebugModeParam(argv, &i, debug) ||
                1
            );
        }
    }
    return res;
}
