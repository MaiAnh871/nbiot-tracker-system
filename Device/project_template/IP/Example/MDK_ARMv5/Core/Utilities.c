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

char* castToString(union Value data, enum ValueType value_type)
{
	static char cast_to_string_buffer[100];

	if (value_type == VALUE_BOOL)
	{
		if ((bool*) data.bool_value)
		{
			sprintf(cast_to_string_buffer, "true");
		}
		else
		{
			sprintf(cast_to_string_buffer, "false");
		}
		return cast_to_string_buffer;
	}
	else if (value_type == VALUE_CHAR)
	{
		sprintf(cast_to_string_buffer, "\"%c\"", *(char*) data.char_value);
	}
	else if (value_type == VALUE_UNSIGNED_CHAR)
	{
		sprintf(cast_to_string_buffer, "\"%c\"",
				*(unsigned char*) data.unsigned_char_value);
	}
	else if (value_type == VALUE_UINT8_T)
	{
		sprintf(cast_to_string_buffer, "%hu", *(uint8_t*) data.uint8_value);
	}
	else if (value_type == VALUE_SHORT)
	{
		sprintf(cast_to_string_buffer, "%hd", *(short*) data.short_value);
	}
	else if (value_type == VALUE_UINT16_T)
	{
		sprintf(cast_to_string_buffer, "%hu", *(uint16_t*) data.uint16_value);
	}
	else if (value_type == VALUE_UNSIGNED_SHORT)
	{
		sprintf(cast_to_string_buffer, "%hu",
				*(unsigned short*) data.unsigned_short_value);
	}
	else if (value_type == VALUE_INT)
	{
		sprintf(cast_to_string_buffer, "%d", *(int*) data.int_value);
	}
	else if (value_type == VALUE_UINT32_T)
	{
		sprintf(cast_to_string_buffer, "%u", *(uint32_t*) data.uint32_value);
	}
	else if (value_type == VALUE_LONG)
	{
		sprintf(cast_to_string_buffer, "%ld", *(long*) data.long_value);
	}
	else if (value_type == VALUE_FLOAT)
	{
		floatToString(*data.float_value, cast_to_string_buffer, 3);
	}
	else if (value_type == VALUE_CHAR_ARRAY)
	{
		sprintf(cast_to_string_buffer, "\"%s\"", (char*) data.char_array_value);
	}

//	sprintf(cast_to_string_buffer, format, *(typeof(data)*) data);
	return cast_to_string_buffer;
}