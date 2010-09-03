#include "helpers.h"

int isTextAnswer(int* vals)
{
    return TEXT_ANSWER == *(vals + 0);
}

int isNoAnswer(int* vals)
{
    return NO_ANSWER == *(vals + 1);
}

void jsonOutputResults(const char* filename, int** totalOptions, int*** selectedOptions, int totalQuestions)
{
    printf("Pending...\n");
/*
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
*/
}

void humanOutputResults(char* filename, int** totalOptions, int*** selectedOptions, int totalQuestions)
{
    printf("Pending...\n");
/*
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
*/
}

void defaultOutputResults(char* filename, int** totalOptions, int*** selectedOptions, int totalQuestions)
{
    printf("%s:\n", filename);
/*
    for (int i = totalQuestions - 1; i >= 0; i--)
    {
        printf("%d / %d\n", *(*results + 2 * i + 0), *(*results + 2 * i + 1));
    }
*/
    for (int i = 0; i < totalQuestions; i++)
    {
        printf("Question %d answers: ", i);
        for (int j = 0; j < *(*totalOptions + i); j++)
        {
            if ( 1 == *(*(*selectedOptions + i) + j) )
            {
                printf("%d ", j + 1);
            }
        }
        printf("\n");
    }
    printf("\n");
}

void machineOutputResults(char* filename, int** totalOptions, int*** selectedOptions, int totalQuestions)
{
/*
    for (int i = totalQuestions - 1; i >= 0; i--)
    {
        printf("%d %d ", *(*results + 2 * i + 0), *(*results + 2 * i + 1));
    }
*/
    printf("%d ", totalQuestions);   // The first number - total questions amount
    
    for (int i = 0; i < totalQuestions; i++)
    {
        printf("%d ", *(*totalOptions + i) );   // Then next <totalQuestions> numbers - optins amount in each question
    }

    for (int i = 0; i < totalQuestions; i++)
    {
        for (int j = 0; j < *(*totalOptions + i); j++)
        {
            printf("%d ", *(*(*selectedOptions + i) + j));    // The next numbers - results for each option in each question - 0 or 1
        }
    }

    printf("\n");
}

const int DEFAULT = 0;
const int JSON = 1;
const int HUMAN = 2;
const int MACHINE = 3;

int translateFormat(const char* format)
{
    if (0 == strcmp(format, "json"))
        return JSON;
    if (0 == strcmp(format, "human"))
        return HUMAN;
    if (0 == strcmp(format, "machine"))
        return MACHINE; 
    return DEFAULT;
}

//void outputResults(const char* filename, int** results, int totalQuestions, const char* format)
void outputResults(const char* filename, int** totalOptions, int*** selectedOptions, int totalQuestions, const char* format)
{
    int f = translateFormat(format);
    switch (f)
    {
        case 1:
            jsonOutputResults(filename, totalOptions, selectedOptions, totalQuestions);
            break;
        case 0:
            defaultOutputResults(filename, totalOptions, selectedOptions, totalQuestions);
            break;
        case 3:
            machineOutputResults(filename, totalOptions, selectedOptions, totalQuestions);
            break;
        default:    //HUMAN
            humanOutputResults(filename, totalOptions, selectedOptions, totalQuestions);
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


