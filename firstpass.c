#include "firstpass.h"
#include "constants.h"
#include "utils.h"

/*
    This file contains all functions regarding the first pass, functions that we use for help and firstPass.
    we convert each line into binary and add it all to a new file (except for the labels, they are added with & at the start of them Ex. labelName, in the file it will be &labelName (because we don't have it's adress yet)) and check for errors in the file after macro parsing.
*/

labelsList *firstPass(FILE *inputFileStream, int *lenInstruction, int *lenData, int *error)
{
    /*This function implements the first pass and calculates all words except from the label adresses*/

    int IC = 0, DC = 0;                  /*The instruction counter and data counter*/
    int numOfErrors = 0, lineNumber = 0; /*number of errors in the file and the line number*/
    int currentNumberOfOperationLines;   /*contains the number of words for this line. (L) */
    enum labelType dataType;             /*contains the dataType. (.string or .data)*/
    enum firstPassState currentState = None;
    labelsList *labels = NULL; /*contains the current label.*/
    labelsList *head = labels;
    char *origLine = (char *)malloc(SIZE_OF_LINE_IN_MAX * sizeof(char));
    char *formatedLine = origLine; /*pointer to the curr line in the file.*/
    FILE *outputFile;              /*contains the temporary file we create for secondPass.*/
    outputFile = fopen(FIRSTPASS_OUTPUT_FILENAME, "w");

    /*We pass through the file passed from preAssembler*/
    while (fgets(formatedLine, SIZE_OF_LINE_IN_MAX, inputFileStream) != NULL)
    {
        char *encodedWord = NULL;
        int labelNameLen = 0;
        char *checkLabel;
        lineNumber++;
        labelNameLen = isLabelDec(formatedLine); /*contains the label length (not 0 if it's a label)*/
        if (labelNameLen != 0)
        {
            currentState = labelDef;
            checkLabel = (char *)calloc(labelNameLen + 1, sizeof(char));
            strncpy(checkLabel, formatedLine, labelNameLen);
            if (returnOperation(checkLabel).operationNum != none_oper)
            {
                printf("\nError, label name can't be a name of a operation, label name:%s\n", checkLabel);
                numOfErrors++;
            }
            if (returnRegisterNumber(checkLabel) != -1)
            {
                printf("\nError, label name can't be a name of a register, label name:%s\n", checkLabel);
                numOfErrors++;
            }
            if (returnDirectVal(checkLabel) != -1)
            {
                printf("\nError, label name can't be a direct number, label name:%s\n", checkLabel);
                numOfErrors++;
            }
            free(checkLabel);
        }
        /*if there is instruction .data or .string (0 is no, .data is data, .string is string)*/
        if ((dataType = isInstructionStore(formatedLine, labelNameLen, currentState)))
        {
            if (currentState == labelDef) /*there is a label*/
            {
                /*add the label to the labelTable*/
                char *labelName;
                labelsList *labelFound = NULL;
                labelName = (char *)malloc((labelNameLen + 1) * sizeof(char));
                strncpy(labelName, formatedLine, labelNameLen);
                labelName[labelNameLen] = '\0';
                labelName[strcspn(labelName, ":")] = '\0';

                if ((labelFound = findLabelByName(head, labelName)) != NULL)
                {
                    /*add the label as a .entry*/
                    if ((labelFound->type == entry) && (labelFound->location == -1))
                    {
                        labelFound->location = DC + MEMORY_START;
                        labelFound->type = entryData;

                        if (dataType == data)
                        {
                            int numOfData = sizeOfDataLabel(formatedLine);
                            char *encodedData;
                            if (count_char_in_string(formatedLine, ',') != numOfData - 1)
                            {
                                printf("\nError, invalid data label declaration, label name:%s\n", labelName);
                                numOfErrors++;
                            }
                            encodedData = dataToBinary(formatedLine, numOfData, labelNameLen);
                            fprintf(outputFile, "%s", encodedData);
                            free(encodedData);
                            DC += numOfData;
                        }
                        else if (dataType == str)
                        {
                            if (checkValidDotString(formatedLine, labelNameLen) == 1)
                            {
                                int strLen = sizeOfStringLabel(formatedLine);
                                char *encodedStr = stringToBinary(formatedLine, strLen, labelNameLen);
                                fprintf(outputFile, "%s\n", encodedStr);
                                free(encodedStr);
                                DC += strLen;
                            }
                            else if (checkValidDotString(formatedLine, labelNameLen) == -1)
                            {
                                printf("string not declared properly, label name: %s\n", labelName);
                                numOfErrors++;
                            }
                            else if (checkValidDotString(formatedLine, labelNameLen) == -2)
                            {
                                printf("string contains invalid characters, label name: %s\n", labelName);
                                numOfErrors++;
                            }
                        }
                        currentState = None;
                    }
                    else /*cant define the same label more than once*/
                    {
                        printf("\nError, can't define the same label name more than once, label name: %s\n", labelName);
                        numOfErrors++;
                    }
                    if (labelName != NULL)
                    {
                        free(labelName);
                    }
                    continue;
                }
                /*adding the current label to the linked list*/
                if (labels == NULL)
                {
                    labels = (labelsList *)malloc(sizeof(labelsList));
                    if (head == NULL)
                    {
                        head = labels;
                    }
                }
                else
                {
                    labels->next = (labelsList *)malloc(sizeof(labelsList));
                    labels = labels->next;
                }

                labels->name = labelName;

                /*update current label with data*/
                labels->location = DC + MEMORY_START;
                labels->type = data;
                labels->next = NULL;
            }

            if (dataType == data)
            {
                char *encodedData;
                int numOfData = sizeOfDataLabel(formatedLine);
                if (count_char_in_string(formatedLine, ',') != numOfData - 1)
                {
                    printf("\nError, invalid data label declaration\n");
                    numOfErrors++;
                }
                encodedData = dataToBinary(formatedLine, numOfData, labelNameLen);
                fprintf(outputFile, "%s", encodedData);
                free(encodedData);
                DC += numOfData;
            }
            else if (dataType == str)
            {
                if (checkValidDotString(formatedLine, labelNameLen) == 1)
                {
                    int strLen = sizeOfStringLabel(formatedLine);
                    char *encodedStr = stringToBinary(formatedLine, strLen, labelNameLen);
                    fprintf(outputFile, "%s\n", encodedStr);
                    free(encodedStr);
                    DC += strLen;
                }
                else if (checkValidDotString(formatedLine, labelNameLen) == -1)
                {
                    *(formatedLine + labelNameLen) = 0;
                    printf("Error, string not declared properly, label name: %s\n", formatedLine);
                    numOfErrors++;
                }
                else if (checkValidDotString(formatedLine, labelNameLen) == -2)
                {
                    *(formatedLine + labelNameLen) = 0;
                    printf("Error, invalid charcters in string, label name: %s\n", formatedLine);
                    numOfErrors++;
                }
            }
            currentState = None;
            continue;
        }

        /*check if the line is a .extern or .entry label decleration*/
        else if ((strncmp(formatedLine, ENTRY_MARK, strlen(ENTRY_MARK)) == 0) || (strncmp(formatedLine, EXTERN_MARK, strlen(EXTERN_MARK)) == 0))
        {
            char *labelName = NULL;
            labelsList *labelFound = NULL;

            if (strncmp(formatedLine, ENTRY_MARK, strlen(ENTRY_MARK)) == 0)
            {
                /*if there is an existing label and it's marked .entry*/
                labelName = formatString(formatedLine + strlen(ENTRY_MARK));
                if ((labelFound = findLabelByName(head, labelName)) != NULL)
                {
                    if (labelFound->type == external)
                    {
                        printf("Error, label cant be decleared as extern and entry in the same time, label name: %s\n", labelName);
                        *error = 1;
                        numOfErrors++;
                    }
                    if (labelFound->type == data)
                    {
                        labelFound->type = entryData;
                    }
                    else if (labelFound->type == code)
                    {
                        labelFound->type = entry;
                    }

                    if (labelName != NULL)
                    {
                        free(labelName);
                    }

                    continue;
                }
            }
            else if (strncmp(formatedLine, EXTERN_MARK, strlen(EXTERN_MARK)) == 0)
            {
                labelName = formatString(formatedLine + strlen(EXTERN_MARK));
                if ((labelFound = findLabelByName(head, labelName)) != NULL)
                {
                    printf("\nError, can't define label more then one time, label name: %s\n", labelName);
                    free(labelName);
                    numOfErrors++;

                    continue;
                }
            }

            /*we create the label*/
            if (labels == NULL)
            {
                labels = (labelsList *)malloc(sizeof(labelsList));
                if (head == NULL)
                {
                    head = labels;
                }
            }
            else
            {
                labels->next = (labelsList *)malloc(sizeof(labelsList));
                labels = labels->next;
            }

            labels->name = labelName;
            labels->location = -1; /*-1 means no value*/
            labels->next = NULL;

            if (strncmp(formatedLine, ENTRY_MARK, strlen(ENTRY_MARK)) == 0)
            { /*if the new label is .entry*/
                labels->type = entry;
            }
            else
            {
                /*if the new label is .extern*/
                labels->type = external;
            }

            /*check errors in labelName*/
            if (isSpaceStr(labelName))
            {
                printf("\nError, name of label can't be blank\n");
                numOfErrors++;
            }
            if (returnOperation(labels->name).operationNum != none_oper)
            {
                printf("\nError, name of label can't be a name of a command, label name: %s\n", labels->name);
                numOfErrors++;
            }

            if (returnRegisterNumber(labels->name) != -1)
            {
                printf("\nError, name of label can't be a name of a register, label name: %s\n", labels->name);
                numOfErrors++;
            }

            continue;
        }

        /*check if there is a normal label dec (without .data/.string/.extern/.entry) and append it to label table with value IC*/
        else if (currentState == labelDef)
        {
            char *labelName;
            labelsList *labelFound = NULL;

            labelName = (char *)malloc((labelNameLen + 2) * sizeof(char));
            strncpy(labelName, formatedLine, labelNameLen + 1);
            labelName[strcspn(labelName, ":")] = '\0';

            if ((labelFound = findLabelByName(head, labelName)) != NULL)
            {
                /*if it's an entry label we set IC correctly*/
                if ((labelFound->type == entry) && (labelFound->location == -1))
                {
                    labelFound->location = IC + MEMORY_START;
                }

                else
                {
                    /*cant define the same label more than once*/
                    printf("Error, can't define the same label name more than once, label name: %s\n", labelName);
                    numOfErrors++;
                }

                if (labelName != NULL)
                {
                    free(labelName);
                }

                continue;
            }

            /*adding the current label to the linked list*/
            if (labels == NULL)
            {
                labels = (labelsList *)malloc(sizeof(labelsList));
                if (head == NULL)
                {
                    head = labels;
                }
            }
            else
            {
                labels->next = (labelsList *)malloc(sizeof(labelsList));
                labels = labels->next;
            }

            labels->name = labelName;
            labels->location = IC + MEMORY_START;
            labels->type = code;
            labels->next = NULL;
            currentState = None;
        }

        /*encode the words of the curr operation*/
        encodedWord = parseLine(formatedLine, &currentNumberOfOperationLines, error);

        if (encodedWord != NULL)
        {
            fprintf(outputFile, "%s\n", encodedWord);
            free(encodedWord);
        }

        /*update IC -> IC + L*/
        IC += currentNumberOfOperationLines;
    }

    if (numOfErrors > 0)
    { /*if there were errors in this file we stop*/
        remove(FIRSTPASS_OUTPUT_FILENAME);
        free(origLine);
        fclose(outputFile);
        freeLabelsList(head);
        *error = 1;
        return NULL;
    }

    /*update DC -> DC + IC*/
    updateDC(head, IC);
    *lenInstruction = IC;
    *lenData = DC;
    free(origLine);
    fclose(outputFile);
    return head;
}

void printLabelsList(labelsList *head)
{ /*This function prints the labels list*/
    puts("---labels---");
    while (head != NULL)
    {
        printf("name: %s count: %d\n", head->name, head->location);
        head = head->next;
    }
    puts("---labels---");
}

void updateDC(labelsList *head, int IC)
{ /*This function updates the DC -> DC + IC (last IC)*/
    while (head != NULL)
    {
        if (head->type == data || head->type == entryData)
        {
            head->location += IC;
        }
        head = head->next;
    }
}

void freeLabelsList(labelsList *head)
{ /*this function frees the labels list*/
    labelsList *tmp;
    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        if (tmp->name != NULL)
        {
            free(tmp->name);
        }
        free(tmp);
    }
}

labelsList *findLabelByName(labelsList *head, char *labelName)
{
    /*this function checks if a label with a name that equals to lableName exists,
    if yes, it returns a pointer to the label if no, it returns null*/
    labelsList *tmp = head;

    char *lableNameCpy = formatString(labelName);

    while (tmp != NULL && tmp->name != NULL)
    {
        if ((strcmp(tmp->name, lableNameCpy) == 0))
        {
            free(lableNameCpy);
            return tmp;
        }
        tmp = tmp->next;
    }

    free(lableNameCpy);
    return NULL;
}

enum labelType isInstructionStore(char *charArr, int labelLen, int currentState)
{
    /*This function checks if the label is a data instruction of the likes of .data or .string and return which it is*/
    char *currLine, *instructLine;
    if (currentState == labelDef)
    {
        currLine = pointerToStartOfNonSpace(charArr + labelLen + 1);
    }
    else
    {
        currLine = pointerToStartOfNonSpace(charArr);
    }
    instructLine = formatString(currLine); /*points to the first word after the label to check if it's .data or .string*/

    if (strcmp(instructLine, DATA_MARK) == 0) /*if there is a .data instruction*/
    {
        free(instructLine);
        return data;
    }
    else if (strcmp(instructLine, STRING_MARK) == 0) /*if there is a .string instruction*/
    {
        free(instructLine);
        return str;
    }
    free(instructLine);
    return code;
}

int sizeOfStringLabel(char *charArr)
{
    /*This function returns the size of the string in a .string dec*/
    char *start = strchr(charArr, '"') + 1;
    char *end = strchr(start, '"') - 1;
    return end - start + 2;
}

int sizeOfDataLabel(char *charArr)
{
    /*This function returns the number of data given in a .data dec*/
    int count = 0;
    char *strCp = (char *)malloc((strlen(charArr) + 1) * sizeof(char));
    char *strCpStart = strCp;
    char *token;
    strcpy(strCp, charArr);
    token = strtok(strCp, ",");

    while (token != NULL)
    {
        count++;

        token = strtok(NULL, ",");
    }

    free(strCpStart);

    return count;
}

char *dataToBinary(char *charArr, int numOfData, int labelLen)
{ /*This function gets data and encrypts it to binary*/
    char *token, *newToken;
    char *strCp = (char *)malloc((strlen(charArr) + 1) * sizeof(char));
    char *strCpStart = strCp;
    char *str;
    char *toReturn = (char *)calloc(((numOfData * (SIZE_OF_LINE_PARSED + 1)) + 1), sizeof(char));
    strcpy(strCp, charArr);
    token = strtok(strCp, ",");

    if (pointerToStartOfNonSpace(token)[0] == '.')
    {
        token = pointerToStartOfNonSpace(charArr);
        token += strlen(DATA_MARK);
    }
    else
    {

        token = pointerToStartOfNonSpace(charArr + labelLen + 1);
        token += strlen(DATA_MARK);
    }
    newToken = strtok(token, ",");

    while (newToken != NULL)
    {
        str = toBinary(atoi(newToken), SIZE_OF_LINE_PARSED);
        toReturn = strncat(toReturn, str, SIZE_OF_LINE_PARSED);
        toReturn = strcat(toReturn, "\n");
        newToken = strtok(NULL, ",");
        free(str);
    }

    free(strCpStart);

    return toReturn;
}

char *stringToBinary(char *charArr, int strLen, int labelLen)
{ /*This function recieves a string and encrypts it.*/
    char *toReturn = (char *)calloc(((strLen * (SIZE_OF_LINE_PARSED + 1)) + 1), sizeof(char));
    char *emptyLine = toBinary(0, SIZE_OF_LINE_PARSED);
    int i;
    char *start;
    start = strchr(charArr, '"') + 1;

    for (i = 0; start[i] != '"'; i++)
    {
        char *dataBinary = toBinary(start[i], SIZE_OF_LINE_PARSED);
        toReturn = strncat(toReturn, dataBinary, strlen(dataBinary));
        toReturn = strcat(toReturn, "\n");
        free(dataBinary);
    }
    toReturn = strncat(toReturn, emptyLine, strlen(emptyLine)); /*for '\0'*/
    free(emptyLine);
    return toReturn;
}

int checkValidDotString(char *charArr, int labelLen)
{ /*This function recieves a string and checks if it's valid.*/
    char *strCp = (char *)malloc((strlen(charArr) + 1) * sizeof(char));
    char *strCpStart = strCp;
    int i, flag;
    strcpy(strCp, charArr);
    if (pointerToStartOfNonSpace(strCp)[0] == '.')
    {
        strCp = pointerToStartOfNonSpace(strCp + strlen(STRING_MARK));
    }
    else
    {
        strCp = pointerToStartOfNonSpace(strCp + labelLen + 1);
        strCp = pointerToStartOfNonSpace(strCp + strlen(STRING_MARK));
    }

    if (strCp == NULL)
        return -1;

    if (strCp[0] != '"')
    {
        free(strCpStart);
        return -1;
    }

    flag = 1;
    for (i = 1; strCp[i]; i++)
    {
        if (strCp[i] != '\n' && strCp[i] != '"')
            continue;
        if (strCp[i] == '"' && ((strCp[i + 1] == '\0') || (isSpaceStr(strCp + i + 1))))
        {
            if (i <= 1)
            {
                free(strCpStart);
                return -1;
            }
            break;
        }

        else
        {
            flag = 0;
            break;
        }
    }

    if (flag == 0)
    {
        free(strCpStart);
        return -2;
    }

    free(strCpStart);
    return 1;
}