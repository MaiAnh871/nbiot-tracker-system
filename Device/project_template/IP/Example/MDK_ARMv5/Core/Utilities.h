#ifndef UTILITIES_
#define UTILITIES_

#include "ht32_board.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "Setting.h"
#include "Collections.h"


/* Extern variables and functions */
extern void Error_Blinking_LED_1(void);
extern void Connecting_Blinking_LED_2(void);

extern void Initialize_Log(void);
extern void Write_Char_Log(u16 character);
extern void Write_String_Log(char * input_string);

char** Tokenize_String(char *str, const char *delim,
		unsigned char *num_tokens);

void removeSubstring(char *str, const char *sub);

void removeChars(char *str, const char *sub);

void floatToString(float num, char *str, unsigned char decimal_digit);

#endif /* UTILITIES_ */