#include "Utilities.h"

char** Tokenize_String(char *str, const char *delim,
		char *num_tokens)
{
	/* ATTENTION: Need to free the pointer after usage */
	/* Example:
	 *
	 * char **pointer = tokenizeString(...);
	 *
	 * // Do something with pointer
	 *
	 * free(pointer);
	 *
	 */

	char **tokens = malloc(MAXIMUM_TOKEN_COUNT * sizeof(char*));
	if (!tokens)
	{
		Error_Blinking_LED_1();
	}

	int i = 0;

	/* Use strtok() function to split string into tokens */
	char *token = strtok(str, delim);

	if (!token)
	{
		*num_tokens = 0;
	}

	while (token != NULL && i < MAXIMUM_TOKEN_COUNT)
	{
		tokens[i] = token;
		i++;
		token = strtok(NULL, delim);
	}

	*num_tokens = i;
	return tokens;
}

void removeSubstring(char *str, const char *sub)
{
	int len = strlen(sub);
	char *p = str;
	while ((p = strstr(p, sub)) != NULL)
	{
		memmove(p, p + len, strlen(p + len) + 1);
	}
}

void removeChars(char *str, const char *sub)
{
	int i, j, n = strlen(sub);
	for (i = j = 0; str[i] != '\0'; i++)
	{
		int k, match = 0;
		for (k = 0; k < n; k++)
		{
			if (str[i] == sub[k])
			{
				match = 1;
				break;
			}
		}
		if (!match)
		{
			str[j++] = str[i];
		}
	}
	str[j] = '\0';
}

void floatToString(float num, char *str, unsigned char decimal_digit)
{
	// Handle negative numbers
	if (num < 0)
	{
		*str++ = '-';
		num = -num;
	}

	// Convert integer part to string
	int integer_part = (int) num;
	int i = sprintf(str, "%d", integer_part);
	str += i;

	// Add decimal point
	*str++ = '.';

	// Convert fractional part to string
	float fractional_part = num - integer_part;
	for (i = 0; i < decimal_digit; i++)
	{
		fractional_part *= 10;
		int digit = (int) fractional_part;
		*str++ = digit + '0';
		fractional_part -= digit;
	}

	// Add null terminator
	*str = '\0';
}
