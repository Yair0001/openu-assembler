#include <stdio.h>
#include "preAssembler.h"
#include "firstpass.h"
#include "secondpass.h"
#include "constants.h"
#include "utils.h"

/*

    SUBMITED BY: Gal Bareket & Yair Matzliach

    This File handles the whole process of creating the files and passing them to the preAssembler, first and second pass, we get input from the command line which contains the program name and files to work on (can be more than 1).

    ASSUMPTIONS FOR THE PROJECT:
      1. We are assuming that there are no label names that start with the letter r and right afterwards a number.

      2. We are assuming that there are no spaces inside Parenthesis used by the special functions bne jmp and jsr.

      3. We are assuming that if there are any errors in the preAssembler phase we don't create the .am file.

      4. We are assuming that there aren't float numbers in .data
*/

int main(int argc, char **argv)
{
    int fileNumber = 1;
    while (fileNumber < argc) /*the first element of argv is the file name of the program and the others are parameters*/
    {
        char *currentFileName = addExtToFileName(argv[fileNumber], ORIGINAL_FILE_EXT); /*generate a string with the filename of the input file*/
        FILE *currentFile = fopen(currentFileName, "r");                               /*opening the input file in read only mode*/
        char *fileFirstPassName;
        FILE *fileFirstPass;
        char *fileAssembledName;
        FILE *fileAssembled;
        labelsList *labelTable;
        int lenInstruction;
        int lenData;
        int error = 0, errorMacro = 0;
        if (currentFile == NULL) /*if the file is null print an error and continue to the next file*/
        {
            printf("\nThe program encountered an error while opening the file: %s\n", argv[fileNumber]);
            fileNumber++;
            continue; /*need to check other files too*/
        }
        fileFirstPassName = addExtToFileName(argv[fileNumber], AFTER_PRE_ASSEMBLER_EXT); /*generating the file name of the file after macros*/
        fileFirstPass = fopen(fileFirstPassName, "w+");                                  /*opening the output file in write mode*/
        errorMacro = parseMacros(currentFile, fileFirstPass);                            /*running the parseMacros function from macros.c*/
        fclose(fileFirstPass);
        fileFirstPass = fopen(fileFirstPassName, "r");
        labelTable = firstPass(fileFirstPass, &lenInstruction, &lenData, &error); /*first pass returns the label Table*/
        fclose(fileFirstPass);
        if (error > 0 || errorMacro < 0)
        {
            printf("There were errors in the file so the program skipped the file: %s\n",fileFirstPassName);
            fileNumber++;

            if (labelTable != NULL)
            {
                freeLabelsList(labelTable);
                labelTable = NULL;
            }
            fclose(currentFile);
            remove(fileFirstPassName);
            free(fileFirstPassName);
            free(currentFileName);

            continue;
        }

        /*create the object file and open it for secondPass*/
        fileAssembledName = addExtToFileName(argv[fileNumber], AFTER_SECOND_PASS_EXT);
        fileAssembled = fopen(fileAssembledName, "w+");

        if (secondPass(fileAssembled, argv[fileNumber], labelTable, lenInstruction, lenData) != 0) /*there was an error*/
        {
            /*freeing memory and closing files*/
            if (labelTable != NULL)
            {
                freeLabelsList(labelTable);
                labelTable = NULL;
            }
            fclose(fileAssembled);
            remove(fileAssembledName);
            free(fileAssembledName);
            free(currentFileName);
            free(fileFirstPassName);
            fclose(currentFile);

            fileNumber++; /*switching to the next file*/
        }
        else
        {

            /*freeing memory and closing files*/
            free(currentFileName);
            fclose(currentFile);
            free(fileAssembledName);
            fclose(fileAssembled);
            free(fileFirstPassName);

            if (labelTable != NULL)
            {
                freeLabelsList(labelTable);
            }

            fileNumber++; /*switching to the next file*/
        }
    }

    return 0;
}