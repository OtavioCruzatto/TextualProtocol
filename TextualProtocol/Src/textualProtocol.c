/*
 * textualProtocol.c
 *
 *  Created on: Aug 10, 2024
 *      Author: Otavio
 */

#include "textualProtocol.h"

// ======== Init =========== //
void textualProtocolInit(TextualProtocol *textualProtocol, uint8_t starterChar, uint8_t separator)
{
	textualProtocol->starterChar = starterChar;
	textualProtocol->separator = separator;
	textualProtocol->command = 0;
	textualProtocol->length = 0;
	textualProtocol->byteIndex = 0;
	textualProtocol->enableDecoding = FALSE;
	memset(textualProtocol->dataPacket, 0x00, QTY_MAX_RX_DATA_BYTES);
}

void textualProtocolAppendByte(TextualProtocol *textualProtocol, uint8_t receivedByte)
{
	if ((receivedByte == CHAR_CR) || (receivedByte == CHAR_LF))
	{
		textualProtocol->enableDecoding = TRUE;
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
			return;
		}
	}
}

void textualProtocolClear(TextualProtocol *textualProtocol)
{
	textualProtocol->command = 0;
	textualProtocol->length = 0;
	textualProtocol->byteIndex = 0;
	textualProtocol->enableDecoding = FALSE;
	memset(textualProtocol->dataPacket, 0x00, QTY_MAX_RX_DATA_BYTES);
}
