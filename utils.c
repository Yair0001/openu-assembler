#include "utils.h"
#include "constants.h"
#include "firstpass.h"

operation_info operations[] = {{mov, delivery_0_1_3, delivery_1_3, operand_2}, {cmp, delivery_0_1_3, delivery_0_1_3, operand_2}, {add, delivery_0_1_3, delivery_1_3, operand_2}, {sub, delivery_0_1_3, delivery_1_3, operand_2}, {not, none, delivery_1_3, operand_1}, {clr, none, delivery_1_3, operand_1}, {lea, delivery_1, delivery_1_3, operand_2}, {inc, none, delivery_1_3, operand_1}, {dec, none, delivery_1_3, operand_1}, {jmp, none, delivery_1_2_3, operand_1}, {bne, none, delivery_1_2_3, operand_1}, {red, none, delivery_1_3, operand_1}, {prn, none, delivery_0_1_3, operand_1}, {jsr, none, delivery_1_2_3, operand_1}, {rts, none, none, operand_0}, {stop, none, none, operand_0}};
operation_info emptyOper = {none_oper, none, none, operand_0};
char *pointerToStartOfNonSpace(char *charArr)
{
    /*This function returns a pointer to the first non space char in an array it also removes non printable elements*/
    while (*charArr != '\0' && (isspace(*charArr) || !isprint(*charArr)))
    {
        charArr++;
    }
    return charArr;
}

char *pointerToAfterChar(char *charArr, char ch)
{ /*This function returns a pointer to the first char after ch*/
    while (*charArr != '\0')
    {
        if (*charArr == ch)
        {
            return charArr;
        }
        charArr++;
    }
    return NULL;
}

char *formatString(char *charArr)
{
    /*This function returns a formated string by replacing the first space or \n with NULL*/
    char *newString = (char *)malloc((strlen(charArr) + 1) * sizeof(char));
    strcpy(newString, pointerToStartOfNonSpace(charArr));
    newString[strcspn(newString, " ")] = '\0';
    newString[strcspn(newString, "\n")] = '\0';
    return newString;
}

char *addExtToFileName(char *fileName, char *extension)
{
    /*This function adds an Extension to a string (in this case a file name) Ex. File -> File.am (if extension is .am)*/
    char *newFileName;
    newFileName = (char *)malloc((strlen(fileName) + strlen(extension) + 1) * sizeof(char));
    strcpy(newFileName, fileName);
    strcat(newFileName, extension);
    return newFileName;
}

int isSpaceStr(char *charArr)
{
    /*This function checks if the entire string is made out of whitespaces*/
    int i = 0;
    while (charArr[i] != '\0')
    {
        if (!isspace(charArr[i]))
        {
            return 0;
        }
        i++;
    }
    return 1;
}

operation_info returnOperation(char *charArr)
{
    /*This function checks if a given macro name is one of the 16 commands and if it is returns it's number*/
    int i;
    char operatorArray[16][5] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};

    for (i = 0; i < sizeof(operatorArray); i++)
    {
        if (strcmp(charArr, operatorArray[i]) == 0)
        {
            return operations[i];
        }
    }
    return emptyOper;
}

int returnRegisterNumber(char *macroName)
{
    /*This function checks if a given macro name is a register name and if it is, it returns it's number*/
    if (macroName[0] == 'r')
    {
        if ((strlen(macroName + 1) == 1) && ((macroName[1] >= '0') && (macroName[1] <= '7')))
        {
            return macroName[1] - '0';
        }
    }
    return -1;
}

int returnDirectVal(char *macroName)
{
    /*This function checks if a given macro name is a register name and if it is, it returns it's number*/
    if (macroName[0] == '#')
    {
        if ((strlen(macroName + 1) == 1) && ((macroName[1] >= '0') && (macroName[1] <= '7')))
        {
            return macroName[1] - '0';
        }
    }
    return -1;
}

int isLabelDec(char *charArr)
{
    /*This function checks if there is a label in the line*/
    int labelLength = 0, i;
    if (!isalpha(charArr[0]))
    {
        return 0;
    }
    for (i = 0; i < SIZE_OF_LABEL_MAX; i++)
    {
        if (charArr[i] == ':')
        {
            return labelLength;
        }
        if (isspace(charArr[i]))
        {
            return 0;
        }
        else if (isalpha(charArr[i]) || isdigit(charArr[i]))
        {
            labelLength++;
            continue;
        }
        else
        {
            /*there is , or ! or somthing like this in label Name*/
            return 0;
        }
    }
    if (charArr[i] == ':') /*only if the label is length 30*/
        return labelLength;
    return 0;
}

enum labelType returnDotInstructionTypeFromStr(char *typeStr)
{ /*This function returns which type of instruction the string passed is (if it is one)*/
    typeStr = pointerToStartOfNonSpace(typeStr);

    if (strcmp(typeStr, DATA_MARK) == 0) /*if there is a .data instruction*/
    {
        return data;
    }
    else if (strcmp(typeStr, STRING_MARK) == 0) /*if there is a .string instruction*/
    {
        return str;
    }
    else if (strcmp(typeStr, EXTERN_MARK) == 0) /*if there is a .data instruction*/
    {
        return external;
    }
    else if (strcmp(typeStr, ENTRY_MARK) == 0) /*if there is a .string instruction*/
    {
        return entry;
    }
    else
    {
        return code;
    }
}

char *parseLine(char *line, int *numOfLines, int *status)
{
    /*This function gets a line of text and parses it if its an operation*/
    char *formatStr = pointerToStartOfNonSpace(line);
    char *strForTok = malloc((strlen(formatStr) + 1) * sizeof(char));
    char *token, *parsedStr = NULL, *operationStr = NULL;
    int labelNameLen = 0;
    operation_info operation;
    *status = 0;
    *numOfLines = 0;

    labelNameLen = isLabelDec(formatStr);
    strcpy(strForTok, formatStr + labelNameLen);
    token = strtok(strForTok, "\n\t: ");

    if (token == NULL)
    {
        *status = -2; /*missing info*/
        goto finish;
    }
    if (returnDotInstructionTypeFromStr(token) != code)
    {
        *status = 1; /* non code instruction*/
        goto finish;
    }
    operation = returnOperation(token);
    operationStr = token;
    if (operation.operationNum == none_oper)
    {
        *status = -1; /* invalid operation*/
        goto finish;
    }
    if (operation.operationNum == jmp || operation.operationNum == bne || operation.operationNum == jsr) /*we splitted the parsing into two parts*/
    {
        parsedStr = parseBneJmpJsr(numOfLines, operation, status, formatStr);
    }
    else
    {
        parsedStr = parseCmd(numOfLines, operation, status, formatStr);
    }
    if (*status < 0)
    {
        goto finish;
    }

finish:
    free(strForTok);
    return parsedStr;
}

char *parseCmd(int *numOfLines, operation_info operation, int *status, char *fullStr)
{ /*This function parses the current line if it has an operation and creates the words for it*/
    int inputType = 0, outputType = 0, inputVal = 0, outputVal = 0;
    char *operStr = (char *)calloc((SIZE_OF_LINE_PARSED + 1) * MAX_NUM_OF_CMDS + 1, sizeof(char));
    char *parsedStr = (char *)calloc((SIZE_OF_LINE_PARSED + 1) * (MAX_NUM_OF_CMDS - 1) + 2, sizeof(char));
    char *token;

    *numOfLines = 0;
    if (operation.numOfOperators == operand_0)
    {
        char *firstWord = createFirstWord(0, 0, 0, operation.operationNum, 0, 0);
        if (count_char_in_string(fullStr, ',') != 0)
        {
            *status = -9; /*invalid line*/
        }
        (*numOfLines)++;
        strcat(parsedStr, firstWord);

        free(firstWord);
        if (operStr != NULL)
        {
            free(operStr);
        }

        return parsedStr;
    }

    else if (operation.numOfOperators == operand_1)
    {
        char *operandStr;

        if (count_char_in_string(fullStr, ',') != 0)
        {
            *status = -9; /*invalid line*/
        }
        token = strtok(NULL, " ,\n");
        if (token == NULL)
        {
            *status = -2;

            if (operStr != NULL)
            {
                free(operStr);
            }
            if (parsedStr != NULL)
            {
                free(parsedStr);
            }

            return NULL;
        }
        if (parseParmStr(token, &outputType, &outputVal) == -1)
        {
            *status = -6;
        }

        if (token == NULL)
        {
            *status = -6; /*invalid operand*/

            if (operStr != NULL)
            {
                free(operStr);
            }
            if (parsedStr != NULL)
            {
                free(parsedStr);
            }

            return NULL;
        }

        *numOfLines = 2;
        operandStr = NULL;

        if (outputType == 3 || outputType == 1 || outputType == 0)
        {
            char *firstWord = createFirstWord(0, outputType, 0, operation.operationNum, 0, 0);
            strcat(parsedStr, firstWord);
            free(firstWord);
        }

        if (outputType == 0)
        {
            char *temp = toBinary(0, 2);
            operandStr = toBinary(outputVal, SIZE_OF_LINE_PARSED - 2);
            strcat(operStr, "\n");
            strcat(operStr, operandStr);
            strcat(operStr, temp);
            free(temp);
            strcat(parsedStr, operStr);
        }
        else if (outputType == 1)
        {
            strcat(operStr, "\n");
            strcat(operStr, LABEL_SYMBOL);
            strcat(operStr, token);
            strcat(parsedStr, operStr);
        }
        else if (outputType == 3)
        {
            char *temp = toBinary(0, 8);
            operandStr = toBinary(outputVal, 6);
            strcat(operStr, "\n");
            strcat(operStr, operandStr);
            strcat(operStr, temp);
            free(temp);
            strcat(parsedStr, operStr);
        }
        if (operandStr != NULL)
        {
            free(operandStr);
        }
    }

    else if (operation.numOfOperators == operand_2)
    {
        char *firstWord;
        char *oper1;
        char *oper2;
        char *tokenSecondParm;
        int temp;
        if ((temp = count_char_in_string(fullStr, ',')) != 1)
        {
            *status = -9; /*invalid line*/
        }
        token = strtok(NULL, " ,\n");
        if (token == NULL)
        {
            *status = -6; /*invalid operand*/

            if (operStr != NULL)
            {
                free(operStr);
            }
            if (parsedStr != NULL)
            {
                free(parsedStr);
            }

            return NULL;
        }
        if (parseParmStr(token, &inputType, &inputVal) == -1)
        {
            *status = -6;
        }
        tokenSecondParm = strtok(NULL, " ,\n");
        if (tokenSecondParm == NULL)
        {
            *status = -6; /*invalid operand*/

            if (operStr != NULL)
            {
                free(operStr);
            }
            if (parsedStr != NULL)
            {
                free(parsedStr);
            }

            return NULL;
        }
        if (parseParmStr(tokenSecondParm, &outputType, &outputVal) == -1)
        {
            *status = -6;
        }

        *numOfLines = 3;
        firstWord = createFirstWord(0, outputType, inputType, operation.operationNum, 0, 0);
        strcat(parsedStr, firstWord);
        free(firstWord);
        oper1 = NULL;
        oper2 = NULL;
        if (inputType == 3)
        {
            if (outputType == 3)
            {
                char *temp = toBinary(0, 2);
                (*numOfLines)--;
                oper1 = toBinary(inputVal, 6);
                oper2 = toBinary(outputVal, 6);

                strcat(operStr, "\n");
                strcat(operStr, oper1);
                strcat(operStr, oper2);
                strcat(operStr, temp);
                free(temp);
                strcat(parsedStr, operStr);

                free(oper1);
                free(oper2);
            }
            else if (outputType == 1)
            {
                char *temp = toBinary(0, 8);
                oper1 = toBinary(inputVal, 6);
                oper2 = tokenSecondParm;

                strcat(operStr, "\n");
                strcat(operStr, oper1);
                strcat(operStr, temp);
                free(temp);
                strcat(operStr, "\n");
                strcat(operStr, LABEL_SYMBOL);
                strcat(operStr, oper2);
                strcat(parsedStr, operStr);

                free(oper1);
            }
            else if (outputType == 0)
            {
                char *temp = toBinary(0, 8);
                oper1 = toBinary(inputVal, 6);
                oper2 = toBinary(outputVal, SIZE_OF_LINE_PARSED - 2);
                strcat(oper1, temp);
                free(temp);
                temp = toBinary(0, 2);
                strcat(oper2, temp);
                free(temp);
                strcat(operStr, "\n");
                strcat(operStr, oper1);
                strcat(operStr, "\n");
                strcat(operStr, oper2);
                strcat(parsedStr, operStr);

                free(oper1);
                free(oper2);
            }
        }
        else if (inputType == 1)
        {
            if (outputType == 3)
            {
                char *temp = toBinary(0, 6);
                oper1 = token;
                oper2 = toBinary(outputVal, 6);

                strcat(operStr, "\n");
                strcat(operStr, LABEL_SYMBOL);
                strcat(operStr, token);
                strcat(operStr, "\n");
                strcat(operStr, temp);
                free(temp);
                temp = toBinary(0, 2);
                strcat(operStr, oper2);
                strcat(operStr, temp);
                free(temp);
                strcat(parsedStr, operStr);

                free(oper2);
            }
            else if (outputType == 1)
            {
                oper1 = token;
                oper2 = tokenSecondParm;

                strcat(operStr, "\n");
                strcat(operStr, LABEL_SYMBOL);
                strcat(operStr, oper1);
                strcat(operStr, "\n");
                strcat(operStr, LABEL_SYMBOL);
                strcat(operStr, oper2);
                strcat(parsedStr, operStr);
            }
            else if (outputType == 0)
            {
                char *temp = toBinary(0, 2);
                oper1 = token;
                oper2 = toBinary(outputVal, SIZE_OF_LINE_PARSED - 2);

                strcat(oper2, temp);
                free(temp);
                strcat(operStr, "\n");
                strcat(operStr, LABEL_SYMBOL);
                strcat(operStr, oper1);
                strcat(operStr, "\n");
                strcat(operStr, oper2);
                strcat(parsedStr, operStr);

                free(oper2);
            }
        }
        else if (inputType == 0)
        {
            if (outputType == 0)
            {
                char *temp = toBinary(0, 2);
                oper1 = toBinary(inputVal, SIZE_OF_LINE_PARSED - 2);
                oper2 = toBinary(outputVal, SIZE_OF_LINE_PARSED - 2);

                strcat(oper1, temp);
                strcat(oper2, temp);
                free(temp);
                strcat(operStr, "\n");
                strcat(operStr, oper1);
                strcat(operStr, "\n");
                strcat(operStr, oper2);
                strcat(parsedStr, operStr);

                free(oper1);
                free(oper2);
            }

            else if (outputType == 1)
            {
                char *temp = toBinary(0, 2);
                oper1 = toBinary(inputVal, SIZE_OF_LINE_PARSED - 2);
                oper2 = tokenSecondParm;

                strcat(oper1, temp);
                free(temp);
                strcat(operStr, "\n");
                strcat(operStr, oper1);
                strcat(operStr, "\n");
                strcat(operStr, LABEL_SYMBOL);
                strcat(operStr, oper2);
                strcat(parsedStr, operStr);

                free(oper1);
            }

            else if (outputType == 3)
            {
                char *temp = toBinary(0, 2);
                oper1 = toBinary(inputVal, SIZE_OF_LINE_PARSED - 2);
                oper2 = toBinary(outputVal, 6);

                strcat(oper1, temp);
                free(temp);
                temp = toBinary(0, 6);
                strcat(operStr, "\n");
                strcat(operStr, oper1);
                strcat(operStr, "\n");
                strcat(operStr, temp);
                free(temp);
                temp = toBinary(0, 2);
                strcat(operStr, oper2);
                strcat(operStr, temp);
                free(temp);
                strcat(parsedStr, operStr);

                free(oper1);
                free(oper2);
            }
        }
    }
    free(operStr);
    return parsedStr;
}

char *parseBneJmpJsr(int *numOfLines, operation_info operation, int *status, char *fullStr)
{ /*This function parses the current line if it has an operation like bne or jmp or jsr and creates the words for it*/
    int outputType = 1, secondParmVal = 0, secondParmType = 0, firstParmVal = 0, firstParmType = 0;
    char *parmStr = (char *)calloc((SIZE_OF_LINE_PARSED + 1) * (MAX_NUM_OF_CMDS - 1) + 1, sizeof(char));
    char *parsedStr = (char *)calloc((SIZE_OF_LINE_PARSED + 1) * MAX_NUM_OF_CMDS + 2, sizeof(char));
    char *token;
    char *firstWord;
    char *tokenSecondParm = NULL;
    int parmType = 0;
    int parmVal = 0;
    char *blankLine = toBinary(0, SIZE_OF_LINE_PARSED);
    *numOfLines = 0;
    token = strtok(NULL, " :,()\n");

    if (token == NULL)
    {
        *status = -6; /*invalid parm*/
        if (blankLine != NULL)
        {
            free(blankLine);
        }
        if (parsedStr != NULL)
        {
            free(parsedStr);
        }
        if (parmStr != NULL)
        {
            free(parmStr);
        }

        return NULL;
    }

    if (parseParmStr(token, &parmType, &parmVal) == -1)
    {
        *status = -6;
    }
    if (parmType != 1)
    {
        *status = -6; /*invalid parm*/

        if (blankLine != NULL)
        {
            free(blankLine);
        }
        if (parsedStr != NULL)
        {
            free(parsedStr);
        }
        if (parmStr != NULL)
        {
            free(parmStr);
        }

        return NULL;
    }
    else
    {
        (*numOfLines)++;
        strcat(parmStr, "\n");
        strcat(parmStr, LABEL_SYMBOL);
        strcat(parmStr, token);
    }
    if ((token = strtok(NULL, ",)")) != NULL)
    {
        if (count_char_in_string(fullStr, ',') != 1)
        {
            *status = -9; /*invalid line*/
        }
        outputType = 2;
        if (parseParmStr(token, &firstParmType, &firstParmVal) == -1)
        {
            *status = -6; /*invalid parm*/
        }
        if ((tokenSecondParm = strtok(NULL, ",)")) != NULL)
        {
            if (parseParmStr(tokenSecondParm, &secondParmType, &secondParmVal) == -1)
            {
                *status = -6; /*invalid parm*/
            }
        }
        else
        {
            *status = -7; /*missing parm*/
        }
        if (firstParmType == -1 && secondParmType == -1)
        {
            *status = -7; /*missing parm*/
        }
        else
        {
            char *firstParmStr = NULL;
            char *secondParmStr = NULL;

            if (secondParmType == 3 && firstParmType == 3)
            {
                char *temp = toBinary(0, 2);
                (*numOfLines) += 1;
                firstParmStr = toBinary(firstParmVal, 6);
                secondParmStr = toBinary(secondParmVal, 6);
                strcat(parmStr, "\n");
                strcat(parmStr, firstParmStr);
                strcat(parmStr, secondParmStr);
                strcat(parmStr, temp);
                free(temp);
            }
            else
            {
                (*numOfLines) += 2;
                if (firstParmType != 1)
                {
                    char *temp = toBinary(0, 2);
                    strcat(parmStr, "\n");
                    firstParmStr = toBinary(firstParmVal, SIZE_OF_LINE_PARSED - 2);
                    strcat(parmStr, firstParmStr);
                    strcat(parmStr, temp);
                    free(temp);
                }
                else
                {
                    strcat(parmStr, "\n");
                    strcat(parmStr, LABEL_SYMBOL);
                    strcat(parmStr, token);
                }
                if (secondParmType != 1)
                {
                    char *temp = toBinary(0, 2);
                    strcat(parmStr, "\n");
                    secondParmStr = toBinary(secondParmVal, SIZE_OF_LINE_PARSED - 2);
                    strcat(parmStr, secondParmStr);
                    strcat(parmStr, temp);
                    free(temp);
                }
                else
                {
                    strcat(parmStr, "\n");
                    strcat(parmStr, LABEL_SYMBOL);
                    strcat(parmStr, tokenSecondParm);
                }
            }
            if (secondParmStr != NULL)
            {
                free(secondParmStr);
            }
            if (firstParmStr != NULL)
            {
                free(firstParmStr);
            }
        }
    }
    else
    {
        if (count_char_in_string(fullStr, ',') != 0)
        {
            *status = -9; /*invalid line*/
        }
    }
    (*numOfLines)++;
    firstWord = createFirstWord(0, outputType, 0, operation.operationNum, MAX(secondParmType, 0), MAX(firstParmType, 0));
    strcat(parsedStr, firstWord);
    free(firstWord);
    strcat(parsedStr, parmStr);

    if (blankLine != NULL)
    {
        free(blankLine);
    }
    if (parmStr != NULL)
    {
        free(parmStr);
    }

    return parsedStr;
}

char *createFirstWord(int A_R_E, int outputType, int inputType, int opCode, int firstParmType, int secondParmType)
{
    /*This function recieves the ARE, output and input Type, the opCode
    and the first and second parameters and creates the first word*/

    char *returnStr = (char *)calloc((SIZE_OF_LINE_PARSED + 1) * MAX_NUM_OF_CMDS + 1, sizeof(char));
    char *A_R_E_Str = toBinary(A_R_E, SIZE_OF_A_R_E_BIN), *outputTypeStr = toBinary(outputType, SIZE_OF_DELIVERY_TYPE_BIN), *inputTypeStr = toBinary(inputType, SIZE_OF_DELIVERY_TYPE_BIN), *opCodeStr = toBinary(opCode, SIZE_OF_OPCODE_BIN), *firstParmTypeStr = toBinary(secondParmType, SIZE_OF_PAREMETER_BIN), *secondParmTypeStr = toBinary(firstParmType, SIZE_OF_PAREMETER_BIN);

    strcat(returnStr, firstParmTypeStr);
    strcat(returnStr, secondParmTypeStr);
    strcat(returnStr, opCodeStr);
    strcat(returnStr, inputTypeStr);
    strcat(returnStr, outputTypeStr);
    strcat(returnStr, A_R_E_Str);

    free(A_R_E_Str);
    free(outputTypeStr);
    free(inputTypeStr);
    free(opCodeStr);
    free(firstParmTypeStr);
    free(secondParmTypeStr);
    return returnStr;
}

int parseParmStr(char *str, int *type, int *val)
{ /*This function recieves a string of a parmeter and changes the *type to the type, and *val to its value, type would be one if there is an error*/
    switch (str[0])
    {
    case '#':
    {
        if (strlen(str) < 2)
        {
            *type = -1;
            return -1;
        }
        *type = 0;
        *val = strtol(str + 1, NULL, 10);
        break;
    }
    case 'r':
    {
        if (strlen(str) < 2)
        {
            *type = -1;
            return -1;
        }
        *type = 3;
        *val = strtol(str + 1, NULL, 10);
        if (*val > 7 || *val < 0)
        {
            *type = -1;
            return -1;
        }
        break;
    }
    default:
        *type = 1; /*label or invalid*/
        return 0;
        break;
    }
    return 0;
}

char *toBinary(int num, int size)
{ /*This function recieves a number and a size and creates a binary number for that number of the recieved size*/
    int numCpy = num;

    char *fullStr = (char *)calloc((size + 1), sizeof(char));
    char *toReturn = (char *)calloc((size + 1), sizeof(char));
    int *inBinary = (int *)malloc(size * sizeof(int));
    int i;
    if (num < 0)
    {
        numCpy = num * -1;
    }
    for (i = 0; numCpy > 0; i++)
    {
        inBinary[i] = numCpy % 2;
        numCpy = numCpy / 2;
    }
    for (i = i - 1; i >= 0; i--)
    {
        if (inBinary[i] == 1)
        {
            toReturn[i] = ONE_BIN_CHAR;
        }
        else
        {
            toReturn[i] = ZERO_BIN_CHAR;
        }
    }
    revStr(toReturn);

    for (i = 0; i < size - strlen(toReturn); i++)
    {
        fullStr[i] = ZERO_BIN_CHAR;
    }
    strcat(fullStr, toReturn);
    if (num < 0)
    {

        int carry = 1;
        for (i = 0; i < size; i++)
        {
            if (fullStr[i] == ONE_BIN_CHAR)
            {
                fullStr[i] = ZERO_BIN_CHAR;
            }
            else
            {
                fullStr[i] = ONE_BIN_CHAR;
            }
        }
        for (i = size - 1; i >= 0; i--)
        {
            if (fullStr[i] == ONE_BIN_CHAR && carry == 1)
            {
                fullStr[i] = ZERO_BIN_CHAR;
            }
            else if (fullStr[i] == ZERO_BIN_CHAR && carry == 1)
            {
                fullStr[i] = ONE_BIN_CHAR;
                carry = 0;
            }
        }
    }
    free(inBinary);
    free(toReturn);
    return fullStr;
}

void revStr(char *str)
{ /*This function reverses the recieved string*/
    int i, len, temp;
    len = strlen(str);

    for (i = 0; i < len / 2; i++)
    {
        temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

int count_char_in_string(char *string, char ch)
{
    /*The function counts and returns the number of times a single char was in a char array*/
    int count = 0;
    while (*string != '\0')
    {
        if (*string == ch)
        {
            count++;
        }
        string++;
    }
    return count;
}