#ifndef UTILS_H_
#define UTILS_H_

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "firstpass.h"


enum deliveryTypes
{
    delivery_1,
    delivery_0_1_3,
    delivery_1_2_3,
    delivery_1_3,
    none
};
enum operandNum
{
    operand_0,
    operand_1,
    operand_2
};
enum operation_cmd
{
    mov,
    cmp,
    add,
    sub,
    not,
    clr,
    lea,
    inc,
    dec,
    jmp,
    bne,
    red,
    prn,
    jsr,
    rts,
    stop,
    none_oper
};

typedef struct OP_STRUCT
{
    enum operation_cmd operationNum;
    enum deliveryTypes allowedOrigTypes;
    enum deliveryTypes allowedDstTypes;
    int numOfOperators;
} operation_info;

extern operation_info operations[];

char *pointerToStartOfNonSpace(char *charArr);

char *pointerToAfterChar(char *charArr, char ch);

char *formatString(char *charArr);

void copyFile(FILE *dst, FILE *src);

char *addExtToFileName(char *fileName, char *extension);

int isSpaceStr(char *charArr);

operation_info returnOperation(char *macroName);

char *parseLine(char *line, int *numOfLines, int *error);

int isDeliveryTypeIncludedInDeliveryGroup(enum deliveryTypes deliveryTypeGroup, int deliveryType);

int returnRegisterNumber(char *macroName);

char *toBinary(int num, int size);

int isLabelDec(char *charArr);

void revStr(char *charArr);

void remChar(char *charArr, char toRemove);

char *remWhiteSpaces(char *charArr);

char *parseCmd(int *numOfLines, operation_info operation, int *status,char* fullStr);

int calcNumOfLines(int inputType, int outputType);

char *parseBneJmpJsr(int *numOfLines, operation_info operation, int *status,char* fullStr);

char *createFirstWord(int A_R_E, int outputType, int inputType, int opCode, int firstParmType, int secondParmType);

int parseParmStr(char *str, int *type, int *val);

int returnDirectVal(char *macroName);

int count_char_in_string(char *string, char ch);


#endif