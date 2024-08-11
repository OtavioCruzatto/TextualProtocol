/*
 * textualProtocol.c
 *
 *  Created on: Aug 10, 2024
 *      Author: Otavio
 */

#include "textualProtocol.h"

// ======== Init =========== //
void textualProtocolInit(TextualProtocol *textualProtocol, uint8_t starterChar, uint8_t separator, UART_HandleTypeDef huart)
{
	textualProtocol->starterChar = starterChar;
	textualProtocol->separator = separator;
	textualProtocol->enableEcho = TRUE;
	textualProtocol->huart = huart;

	textualProtocol->command = 0;
	textualProtocol->length = 0;
	textualProtocol->byteIndex = 0;
	textualProtocol->enableDecoding = FALSE;
	textualProtocol->textualProtocolRxStatus = INVALID_RX_TEXTUAL_PROTOCOL;
	memset(textualProtocol->dataPacket, 0x00, QTY_MAX_RX_DATA_BYTES);
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
			textualProtocolClear(textualProtocol);
			textualProtocolSendSyntaxError(textualProtocol);
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
			textualProtocolClear(textualProtocol);
			textualProtocolSendSyntaxError(textualProtocol);
		}
	}
}

void textualProtocolClear(TextualProtocol *textualProtocol)
{
	textualProtocol->command = 0;
	textualProtocol->length = 0;
	textualProtocol->byteIndex = 0;
	textualProtocol->enableDecoding = FALSE;
	textualProtocol->textualProtocolRxStatus = INVALID_RX_TEXTUAL_PROTOCOL;
	memset(textualProtocol->dataPacket, 0x00, QTY_MAX_RX_DATA_BYTES);
}

void textualProtocolSendSyntaxError(TextualProtocol *textualProtocol)
{
	HAL_UART_Transmit(&textualProtocol->huart, (uint8_t *) "Syntax Error\r\n", 14, HAL_MAX_DELAY);
}

void textualProtocolSendNewLine(TextualProtocol *textualProtocol)
{
	HAL_UART_Transmit(&textualProtocol->huart, (uint8_t *) "\r\n", 2, HAL_MAX_DELAY);
}

void textualProtocolDecode(TextualProtocol *textualProtocol)
{
	if (textualProtocol->dataPacket[0] != textualProtocol->starterChar)
	{
		textualProtocolSendSyntaxError(textualProtocol);
		textualProtocolClear(textualProtocol);
		return;
	}

	textualProtocolClear(textualProtocol);
}



Bool textualProtocolGetEchoEnable(TextualProtocol *textualProtocol)
{
	return textualProtocol->enableEcho;
}
