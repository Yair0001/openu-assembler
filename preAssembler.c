#include "preAssembler.h"
#include "constants.h"
#include "utils.h"

/*
    This file contains the macro parsing function, we go through the original file and parse all the macros and check for errors.
*/

int parseMacros(FILE *inFileStream, FILE *outFileStream)
{ /*this function outputs to the file stream outFileStream the contance of the file stream inFileStream with parsed macros*/

    macrosList *macros = NULL; /*contains the current macro*/
    macrosList *head = macros;
    char *origLine = (char *)malloc(SIZE_OF_LINE_IN_MAX * sizeof(char));
    char *formatedLine = origLine; /*pointer to the curr line in the file.*/
    enum macroState currentState = copy;
    int macroSize = 0;
    int numOfErrors = 0;
    long int macroStart = 0;
    long int macroEnd = 0;
    int numOfLinesFromParseLine = 0, currentLine = 0;
    while (fgets(formatedLine, SIZE_OF_LINE_IN_MAX, inFileStream) != NULL) /*getting a line form the input file stream*/
    {
        int error = 0;
        char *temp = NULL;
        macrosList *macroFound = NULL;
        if (currentState != countMacro)
        {
            currentLine++;
        }

        if (isSpaceStr(formatedLine) || pointerToStartOfNonSpace(formatedLine)[0] == ';') /*if the line only containes spaces continue to the next one*/
            continue;

        formatedLine = pointerToStartOfNonSpace(formatedLine); /*formating the line*/

        if ((macroFound = checkMacroNameExists(head, formatedLine)) != NULL) /*if the line is a macro*/
        {
            fprintf(outFileStream, "%s", macroFound->data);

            /*if the macro data is either nothing or '\n'*/
            if (*macroFound->data != '\n' && *macroFound->data != '\0')
                fprintf(outFileStream, "\n");
        }
        else if ((strncmp(formatedLine, MACRO_START_STRING, strlen(MACRO_START_STRING)) == 0) && (strlen(temp = formatString(formatedLine)) == strlen(MACRO_START_STRING)))
        {
            /*if the line is the start of a macro*/
            char *macroName;

            if (checkMacroNameExists(head, formatedLine + strlen(MACRO_START_STRING)) != NULL)
            {
                printf("Can't have 2 declerations of the same macro | in line: %d\n", currentLine);
                numOfErrors++;
            }
            if (currentState == countMacro)
            {
                printf("Error, can't be a macro inside a macro | in line: %d\n", currentLine);
                numOfErrors++;
            }
            currentState = countMacro;
            macroStart = ftell(inFileStream);
            if (macros == NULL)
            {
                macros = (macrosList *)malloc(sizeof(macrosList));
                if (head == NULL)
                {
                    head = macros;
                }
            }
            else
            {
                macros->next = (macrosList *)malloc(sizeof(macrosList));
                macros = macros->next;
            }

            macroName = (char *)malloc((strlen(formatedLine) + 1) * sizeof(char));
            strcpy(macroName, pointerToStartOfNonSpace(formatedLine + strlen(MACRO_START_STRING)));
            macroName[strcspn(macroName, "\n")] = '\0';
            macros->name = macroName;
            macros->data = NULL;
            macros->next = NULL;
            if (isSpaceStr(macroName))
            {
                printf("\nError, name of macro can't be blank, in line: %d\n", currentLine);
                numOfErrors++;
            }
            if (returnOperation(macros->name).operationNum != none_oper)
            {
                printf("\nError, name of macro can't be a name of a command, line number: %d\n", currentLine);
                numOfErrors++;
            }

            if (returnRegisterNumber(macros->name) != -1)
            {
                printf("\nError, name of macro can't be a name of a register, line number: %d\n", currentLine);
                numOfErrors++;
            }
        }                                                                                                                                            /*added 1 cause there is \n in formatedLine*/
        else if ((strncmp(formatedLine, MACRO_END_STRING, strlen(MACRO_END_STRING)) == 0) && (strlen(formatedLine) == strlen(MACRO_END_STRING) + 1)) /*if the line is an end of a macro*/
        {
            if (currentState == countMacro) /*if we reached the end of the counting of the length of the macro*/
            {
                macroSize = MAX((macroEnd - macroStart), 0); /*calculating the size of the macro by subtrationg the end from the start*/
                fseek(inFileStream, macroStart, SEEK_SET);   /*moving the input stream to the start of the macro*/
                currentState = saveMacro;
                macros->data = (char *)malloc(macroSize + 1);
                *(macros->data) = 0;
            }
            else
            {
                if (macros->data[strlen(macros->data) - 1] == '\n') /*if the end of the macro is a newline replace ir with \0 to remove it*/
                {
                    macros->data[strlen(macros->data) - 1] = '\0';
                }
                currentState = copy;
            }
        }
        else if (currentState == countMacro)
        {
            macroEnd = ftell(inFileStream); /*setting the end postion*/
        }
        else if (currentState == saveMacro)
        {
            strcat(macros->data, formatedLine);                              /*saving data to the macro*/
            free(parseLine(formatedLine, &numOfLinesFromParseLine, &error)); /*we run parse line to get errors*/
        }
        else if (currentState == copy)
        {
            fprintf(outFileStream, "%s", formatedLine);                      /*copy the formated line to the output stream if the current line is not a macro*/
            free(parseLine(formatedLine, &numOfLinesFromParseLine, &error)); /*we run parse line to get errors*/
        }

        /*check for errors in current line*/
        if ((error) == -1 || error == -9)
        {
            printf("Error, invalid line, line Number: %d\n", currentLine);
            numOfErrors++;
        }
        else if ((error) == -2)
        {
            printf("Error, missing info, line Number: %d\n", currentLine);
            numOfErrors++;
        }
        else if ((error) == -6)
        {
            printf("Error, Invalid Operand, line number: %d\n", currentLine);
            numOfErrors++;
        }
        else if ((error) == -8)
        {
            printf("Error, in those operations there shoulden't be any spaces between the brackets, line number: %d\n", currentLine);
            numOfErrors++;
        }
        free(temp);
    }

    free(origLine);
    freeMacrosList(head);
    if (numOfErrors > 0)
    {
        return -1;
    }
    return 0;
}

void printMacrosList(macrosList *head)
{ /*This function prints the macros list*/
    puts("---macros---");
    while (head != NULL)
    {
        printf("name: %s data: %s\n", head->name, head->data);
        head = head->next;
    }
    puts("---macros---");
}

void freeMacrosList(macrosList *head)
{ /*this function frees the macros list*/
    macrosList *tmp;
    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        if (tmp->name != NULL)
        {
            free(tmp->name);
        }
        if (tmp->data != NULL)
        {
            free(tmp->data);
        }
        free(tmp);
    }
}

macrosList *checkMacroNameExists(macrosList *head, char *macroName)
{ /*this function checks if a macro with a name that equals to macroName exists if yes, it returns a pointer to the macro if no, it returns null*/
    macrosList *tmp = head;

    char *macroNameCpy = formatString(macroName);

    while (tmp != NULL)
    {
        if ((strncmp(tmp->name, macroNameCpy, strlen(tmp->name)) == 0) && (strlen(tmp->name) == strlen(macroNameCpy)))
        {
            free(macroNameCpy);
            return tmp;
        }
        tmp = tmp->next;
    }

    free(macroNameCpy);
    return NULL;
}