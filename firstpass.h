#ifndef FIRSTPASS_H_
#define FIRSTPASS_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

enum firstPassState
{
    labelDef,
    None
};

enum labelType
{
    code,
    data,
    str,
    external,
    entry,
    entryData
};

typedef struct labelsLinkedList
{
    struct labelsLinkedList *next;
    char *name;
    int location;
    int type;
} labelsList;

labelsList *firstPass(FILE *inputFileStream,int* lenInstruction,int* lenData, int *error);

labelsList *findLabelByName(labelsList *head, char *lableName);

void printLabelsList(labelsList *head);

enum labelType isInstructionStore(char *charArr, int labelLen, int currentState);

int sizeOfStringLabel(char *charArr);

int sizeOfDataLabel(char *charArr);

char *dataToBinary(char *charArr, int numOfData, int labelLen);

char *stringToBinary(char *charArr, int strLen, int labelNameLen);

int checkValidDotString(char *charArr, int labelNameLen);

void updateDC(labelsList *head, int IC);

void freeLabelsList(labelsList *head);

#endif