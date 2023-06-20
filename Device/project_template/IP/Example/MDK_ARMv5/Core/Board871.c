#include "Board871.h"

void Board871_Initialize(struct Board871 * self) {
  self -> board871_log_content = (char * ) malloc(BOARD871_LOG_CONTENT * sizeof(char));
  if (!self -> board871_log_content) {
		Error_Blinking_LED_1();
  }

  BC660K_Initialize( & self -> bc660k);
  LC76F_Initialize(&self->lc76f);
  //	MC3416_Initialize(&self->mc3416);
}

void addData(struct Board871 * self, char key[], void *value,
		enum ValueType value_type)
{

	/* Fail add previous data, then not add more data */
	if (!self->add_data_successfully)
	{
		return;
	}

	DataNode *new_node = malloc(sizeof(DataNode));

	if (!new_node || (strlen(key) > KEY_MAX_LENGTH))
	{
		self->add_data_successfully = false;
		return;
	}

	strcpy(new_node->pair.key, key);

	switch (value_type)
	{
	case VALUE_BOOL:
		new_node->pair.value.bool_value = (bool*) value;
		new_node->pair.value_type = VALUE_BOOL;
		break;
	case VALUE_CHAR:
		new_node->pair.value.char_value = (char*) value;
		new_node->pair.value_type = VALUE_CHAR;
		break;
	case VALUE_UNSIGNED_CHAR:
		new_node->pair.value.unsigned_char_value = (unsigned char*) value;
		new_node->pair.value_type = VALUE_UNSIGNED_CHAR;
		break;
	case VALUE_UINT8_T:
		new_node->pair.value.uint8_value = (uint8_t*) value;
		new_node->pair.value_type = VALUE_UINT8_T;
		break;
	case VALUE_SHORT:
		new_node->pair.value.short_value = (short*) value;
		new_node->pair.value_type = VALUE_SHORT;
		break;
	case VALUE_UNSIGNED_SHORT:
		new_node->pair.value.unsigned_short_value = (unsigned short*) value;
		new_node->pair.value_type = VALUE_UNSIGNED_SHORT;
		break;
	case VALUE_UINT16_T:
		new_node->pair.value.uint16_value = (uint16_t*) value;
		new_node->pair.value_type = VALUE_UINT16_T;
		break;
	case VALUE_INT:
		new_node->pair.value.int_value = (int*) value;
		new_node->pair.value_type = VALUE_INT;
		break;
	case VALUE_LONG:
		new_node->pair.value.long_value = (long*) value;
		new_node->pair.value_type = VALUE_LONG;
		break;
	case VALUE_UINT32_T:
		new_node->pair.value.uint32_value = (uint32_t*) value;
		new_node->pair.value_type = VALUE_UINT32_T;
		break;
	case VALUE_FLOAT:
		new_node->pair.value.float_value = (float*) value;
		new_node->pair.value_type = VALUE_FLOAT;
		break;
	case VALUE_CHAR_ARRAY:
		new_node->pair.value.char_array_value = (char*) value;
		new_node->pair.value_type = VALUE_CHAR_ARRAY;
		break;
	default:
		return;
	}

	/* Set the next node to NULL */
	new_node->next = NULL;

	/* If the list is empty, set the new node as the head */
	if (!self->data_list)
	{
		self->data_list = new_node;
	}
	/* Otherwise, traverse the list to the end and append the new node */
	else
	{
		DataNode *current_node = self->data_list;
		while (current_node->next)
		{
			current_node = current_node->next;
		}
		current_node->next = new_node;
	}

}