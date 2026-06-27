#ifndef MACRO_H_
#define MACRO_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

enum macroState{copy,saveMacro,countMacro};


int parseMacros(FILE *inFileStream, FILE *outFileStream);

typedef struct macrosLinkedList
{
    struct macrosLinkedList *next;
    char* name;
    char* data;
} macrosList;

void printMacrosList(macrosList *head);

void freeMacrosList(macrosList *head);

macrosList* checkMacroNameExists(macrosList *head, char *macroName);

#endif