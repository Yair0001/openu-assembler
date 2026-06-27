#ifndef SECONDPASS_H_
#define SECONDPASS_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "firstpass.h"

int secondPass(FILE *fileOutputStream, char *fileName, labelsList *head, int lenInstruction, int lenData);
int writeEntry(labelsList *head, FILE *entryFileStream);
#endif