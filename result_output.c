#include "helpers.h"

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


