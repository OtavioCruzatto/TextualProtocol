/*
 * textualProtocol.c
 *
 *  Created on: Aug 10, 2024
 *      Author: Otavio
 */

#include "textualProtocol.h"

// ======== Init =========== //
void textualProtocolInit(TextualProtocol *textualProtocol, uint8_t starterChar, uint8_t delimeter, UART_HandleTypeDef huart)
{
	textualProtocol->starterChar = starterChar;
	textualProtocol->delimiter = delimeter;
	textualProtocol->enableEcho = TRUE;
	textualProtocol->huart = huart;

	textualProtocolClear(textualProtocol, CLEAR_ALL);
}

void textualProtocolAppendByte(TextualProtocol *textualProtocol, uint8_t receivedByte)
{
	if ((receivedByte == CHAR_CR) || (receivedByte == CHAR_LF))
	{
		if (textualProtocol->length >= QTY_MIN_RX_DATA_BYTES)
		{
			if (textualProtocol->enableEcho == TRUE)
			{
				textualProtocolSendNewLine(textualProtocol);
			}
			textualProtocol->enableDecoding = TRUE;
		}
		else
		{
			textualProtocolClear(textualProtocol, CLEAR_ALL);
			textualProtocolSendStatusMessage(textualProtocol, STATUS_MESSAGE_SYNTAX_ERROR);
		}
	}
	else if ((receivedByte >= ' ') && (receivedByte <= 'z') && (textualProtocol->enableDecoding == FALSE))
	{
		if (textualProtocol->byteIndex < QTY_MAX_RX_DATA_BYTES)
		{
			textualProtocol->dataPacket[textualProtocol->byteIndex] = receivedByte;
			textualProtocol->byteIndex++;
			textualProtocol->length++;
		}
		else
		{
			textualProtocolClear(textualProtocol, CLEAR_ALL);
			textualProtocolSendStatusMessage(textualProtocol, STATUS_MESSAGE_SYNTAX_ERROR);
		}
	}
}

void textualProtocolClear(TextualProtocol *textualProtocol, TextualProtocolClear clear)
{
	uint8_t i = 0;

	switch (clear)
	{
		case CLEAR_ALL:
			textualProtocol->length = 0;
			textualProtocol->byteIndex = 0;
			textualProtocol->qtyOfDelimiters = 0;
			textualProtocol->enableDecoding = FALSE;
			textualProtocol->textualProtocolRxStatus = INVALID_RX_TEXTUAL_PROTOCOL;
			textualProtocol->commandLength = 0;
			memset(textualProtocol->dataPacket, 0x00, QTY_MAX_RX_DATA_BYTES);
			memset(textualProtocol->indexesOfDelimiters, 0x00, QTY_MAX_OF_DELIMITERS);
			memset(textualProtocol->command, 0x00, QTY_MAX_OF_BYTES_IN_COMMAND);

			for (i = 0; i < QTY_MAX_OF_VALUES; i++)
			{
				memset(textualProtocol->values[i], 0x00, QTY_MAX_OF_BYTES_PER_VALUE);
			}

			break;

		case CLEAR_VALUES:
			for (i = 0; i < QTY_MAX_OF_VALUES; i++)
			{
				memset(textualProtocol->values[i], 0x00, QTY_MAX_OF_BYTES_PER_VALUE);
			}
			break;

		case CLEAR_DATA_PACKET:
			textualProtocol->length = 0;
			textualProtocol->byteIndex = 0;
			memset(textualProtocol->dataPacket, 0x00, QTY_MAX_RX_DATA_BYTES);
			break;

		case CLEAR_INDEXES_OF_DELIMITERS:
			textualProtocol->qtyOfDelimiters = 0;
			memset(textualProtocol->indexesOfDelimiters, 0x00, QTY_MAX_OF_DELIMITERS);
			break;

		case CLEAR_COMMAND:
			textualProtocol->commandLength = 0;
			memset(textualProtocol->command, 0x00, QTY_MAX_OF_BYTES_IN_COMMAND);
			break;

		case CLEAR_AFTER_DECODE:
			textualProtocol->length = 0;
			textualProtocol->byteIndex = 0;
			textualProtocol->qtyOfDelimiters = 0;
			textualProtocol->enableDecoding = FALSE;
			memset(textualProtocol->dataPacket, 0x00, QTY_MAX_RX_DATA_BYTES);
			memset(textualProtocol->indexesOfDelimiters, 0x00, QTY_MAX_OF_DELIMITERS);
			break;

		default:
			break;
	}
}

void textualProtocolSendStatusMessage(TextualProtocol *textualProtocol, TextualProtocolStatusMessages statusMessage)
{
	switch (statusMessage)
	{
		case STATUS_MESSAGE_OK:
			HAL_UART_Transmit(&textualProtocol->huart, (uint8_t *) "OK\r\n", 4, HAL_MAX_DELAY);
			break;

		case STATUS_MESSAGE_SYNTAX_ERROR:
			HAL_UART_Transmit(&textualProtocol->huart, (uint8_t *) "Syntax Error\r\n", 14, HAL_MAX_DELAY);
			break;

		case STATUS_MESSAGE_UNKNOWN_COMMAND:
			HAL_UART_Transmit(&textualProtocol->huart, (uint8_t *) "Unknown Command\r\n", 17, HAL_MAX_DELAY);
			break;

		case STATUS_MESSAGE_VALUE_ERROR:
			HAL_UART_Transmit(&textualProtocol->huart, (uint8_t *) "Value Error\r\n", 13, HAL_MAX_DELAY);
			break;

		default:
			break;
	}
}

void textualProtocolSendNewLine(TextualProtocol *textualProtocol)
{
	HAL_UART_Transmit(&textualProtocol->huart, (uint8_t *) "\r\n", 2, HAL_MAX_DELAY);
}

void textualProtocolDecode(TextualProtocol *textualProtocol)
{
	if (textualProtocol->dataPacket[0] != textualProtocol->starterChar)
	{
		textualProtocolSendStatusMessage(textualProtocol, STATUS_MESSAGE_SYNTAX_ERROR);
		textualProtocolClear(textualProtocol, CLEAR_ALL);
		return;
	}

	textualProtocolExtractCommand(textualProtocol);
	textualProtocolFindDelimiters(textualProtocol);

	if (textualProtocol->qtyOfDelimiters > 0)
	{
		textualProtocolExtractValues(textualProtocol);
	}

	textualProtocolClear(textualProtocol, CLEAR_AFTER_DECODE);
}

void textualProtocolExtractCommand(TextualProtocol *textualProtocol)
{
	uint8_t i = 1;

	if (((textualProtocol->dataPacket[1] >= 'A') && (textualProtocol->dataPacket[1] <= 'Z'))
		||
		((textualProtocol->dataPacket[1] >= 'a') && (textualProtocol->dataPacket[1] <= 'z')))
	{
		textualProtocol->textualProtocolRxStatus = VALID_RX_TEXTUAL_PROTOCOL;
		while (textualProtocol->commandLength < (QTY_MAX_OF_BYTES_IN_COMMAND - 1))
		{
			if (textualProtocol->dataPacket[i] == textualProtocol->delimiter)
			{
				return;
			}
			else if ((textualProtocol->dataPacket[i] >= ' ') && (textualProtocol->dataPacket[i] <= '}'))
			{
				textualProtocol->command[textualProtocol->commandLength] = textualProtocol->dataPacket[i];
				textualProtocol->commandLength++;
				i++;
			}
			else
			{
				return;
			}
		}
	}
	else
	{
		textualProtocolSendStatusMessage(textualProtocol, STATUS_MESSAGE_UNKNOWN_COMMAND);
		textualProtocolClear(textualProtocol, CLEAR_ALL);
		return;
	}
}

void textualProtocolFindDelimiters(TextualProtocol *textualProtocol)
{
	uint8_t i = 0;
	uint8_t i_delimeters = 0;

	for (i = 0; i < QTY_MAX_RX_DATA_BYTES; i++)
	{
		if (i_delimeters < QTY_MAX_OF_DELIMITERS)
		{
			if (textualProtocol->dataPacket[i] == textualProtocol->delimiter)
			{
				textualProtocol->indexesOfDelimiters[i_delimeters] = i;
				textualProtocol->qtyOfDelimiters++;
				i_delimeters++;
			}
		}
	}
}

void textualProtocolExtractValues(TextualProtocol *textualProtocol)
{
	uint8_t value = 0;
	uint8_t byte = 0;
	for (value = 0; value < textualProtocol->qtyOfDelimiters; value++)
	{
		for (byte = 0; byte < (QTY_MAX_OF_BYTES_PER_VALUE - 1); byte++)
		{
			uint8_t index_delimiter = textualProtocol->indexesOfDelimiters[value];
			uint8_t data = textualProtocol->dataPacket[index_delimiter + byte + 1];
			if ((data == ',') || (data == 0))
			{
				break;
			}

			textualProtocol->values[value][byte] = data;
		}
	}
}

void textualProtocolPrintCurrentValues(TextualProtocol *textualProtocol)
{
	char message[35];
	uint8_t value = 0;

	for (value = 0; value < QTY_MAX_OF_VALUES; value++)
	{
		memset(message, 0x00, 35);
		sprintf(message, "Value %d: %s\r\n", value, textualProtocol->values[value]);
		HAL_UART_Transmit(&textualProtocol->huart, ((uint8_t *) message), strlen(message), HAL_MAX_DELAY);
	}
}

Bool textualProtocolGetEchoEnable(TextualProtocol *textualProtocol)
{
	return textualProtocol->enableEcho;
}
