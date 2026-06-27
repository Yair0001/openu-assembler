#include "secondpass.h"
#include "constants.h"
#include "utils.h"
#include "firstpass.h"

/*
    This file contains the second pass and writes everything from the temporary file to the ob file and replaces all the label names with their addresses.
    we also create the .ext file and .ent file if needed and check for errors.
*/

int secondPass(FILE *fileOutputStreamBin, char *fileName, labelsList *head, int lenInstruction, int lenData)
{
    FILE *fileStreamInputFile = fopen(FIRSTPASS_OUTPUT_FILENAME, "r");

    char *fileExtName = addExtToFileName(fileName, EXTERNAL_FILE_EXT);
    char *fileEntName = addExtToFileName(fileName, ENTRY_FILE_EXT);
    FILE *fileStreamExt = fopen(fileExtName, "w+");
    FILE *fileStreamEnt = fopen(fileEntName, "w+");

    int IC = MEMORY_START;
    int error = 0, isExtFile = 0, isEntFile = 0;
    char *origLine = (char *)malloc(SIZE_OF_LINE_IN_MAX * sizeof(char));
    char *formatedLine = origLine;
    fprintf(fileOutputStreamBin, "\t\t%d %d\n", lenInstruction, lenData);

    while (fgets(formatedLine, SIZE_OF_LINE_IN_MAX, fileStreamInputFile) != NULL)
    {
        formatedLine = pointerToStartOfNonSpace(formatedLine);

        /*if the line is not a label address add the curr line to the .ob file*/
        if (strncmp(formatedLine, LABEL_SYMBOL, strlen(LABEL_SYMBOL)) != 0)
        {
            fprintf(fileOutputStreamBin, "%04d\t  %s", IC, formatedLine);
        }
        else
        {
            /*if the line is a label address add the label adrress to the .ob file*/

            char *labelAddrBin = NULL, *labelNameToSearch = formatedLine + strlen(LABEL_SYMBOL);
            labelsList *label;

            if (strlen(formatedLine) > strlen(LABEL_SYMBOL))
            {
                labelNameToSearch[strcspn(labelNameToSearch, "\n")] = '\0';
                label = findLabelByName(head, labelNameToSearch);
            }

            if (label == NULL)
            {
                printf("Error, there isn't a label dec for %s\n", labelNameToSearch);
                error = 1;
                break;
            }

            else if (label->type != external)
            {
                char *temp = toBinary(2, 2);
                /*if the label isn't extern*/
                labelAddrBin = toBinary(label->location, SIZE_OF_LINE_PARSED - 2);
                fprintf(fileOutputStreamBin, "%04d\t  %s%s\n", IC, labelAddrBin, temp);
                free(temp);

                /*if label is .entry add it to .ent file*/
                if (label->type == entry || label->type == entryData)
                {
                    isEntFile = 1;
                }
            }
            else if (label->type == external)
            {
                char *temp = toBinary(1, 2);
                /*if the label is extern*/
                labelAddrBin = toBinary(0, SIZE_OF_LINE_PARSED - 2);
                fprintf(fileOutputStreamBin, "%04d\t  %s%s\n", IC, labelAddrBin, temp);
                free(temp);

                /*add label to .ext file*/
                fprintf(fileStreamExt, "%s\t  %d\n", labelNameToSearch, IC);
                isExtFile = 1;
            }
            if (labelAddrBin != NULL)
            {
                free(labelAddrBin);
            }
        }
        IC++;
    }
    isEntFile = writeEntry(head, fileStreamEnt);
    fclose(fileStreamEnt);
    fclose(fileStreamExt);
    fclose(fileStreamInputFile);

    if (isExtFile == 0 || error != 0)
    {
        /*if there are no .extern labels*/
        remove(fileExtName);
    }
    if (isEntFile == 0 || error != 0)
    {
    /*if there are no .entry labels*/
        remove(fileEntName);
    }

    free(origLine);
    free(fileExtName);
    free(fileEntName);
    remove(FIRSTPASS_OUTPUT_FILENAME);

    return error;
}

int writeEntry(labelsList *head, FILE *entryFileStream)
{ /*this function prints all the entry's into the file*/
    int entryExists = 0;
    while (head != NULL)
    {
        if (head->type == entry || head->type == entryData)
        {
            entryExists = 1;
            fprintf(entryFileStream, "%s\t  %d\n", head->name, head->location);
        }
        head = head->next;
    }
    return entryExists;
}