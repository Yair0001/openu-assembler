#ifndef CONSTANTS_H_
#define CONSTANTS_H_

typedef int make_iso_compilers_happy;
#define AFTER_PRE_ASSEMBLER_EXT ".am"
#define ORIGINAL_FILE_EXT ".as"
#define AFTER_SECOND_PASS_EXT ".ob"
#define EXTERNAL_FILE_EXT ".ext"
#define ENTRY_FILE_EXT ".ent"
#define SIZE_OF_LINE_IN_MAX 81
#define SIZE_OF_LINE_PARSED 14
#define SIZE_OF_LABEL_MAX 30
#define MAX_NUM_OF_CMDS 4
#define MEMORY_START 100
#define SIZE_OF_A_R_E_BIN 2
#define SIZE_OF_DELIVERY_TYPE_BIN 2
#define SIZE_OF_OPCODE_BIN 4
#define SIZE_OF_PAREMETER_BIN 2
#define MACRO_START_STRING "mcr"
#define MACRO_END_STRING "endmcr"
#define DATA_MARK ".data"
#define STRING_MARK ".string"
#define ENTRY_MARK ".entry"
#define EXTERN_MARK ".extern"
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define LABEL_SYMBOL "&"
#define FIRSTPASS_OUTPUT_FILENAME "assembler.temp"
#define ZERO_BIN_CHAR '.'
#define ONE_BIN_CHAR '/'

#endif