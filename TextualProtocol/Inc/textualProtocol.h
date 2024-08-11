/*
 * textualProtocol.h
 *
 *  Created on: Aug 10, 2024
 *      Author: Otavio
 */

#ifndef INC_TEXTUALPROTOCOL_H_
#define INC_TEXTUALPROTOCOL_H_

#include <stdint.h>
#include <string.h>
#include "defs.h"
#include "enums.h"

#define	QTY_MIN_RX_DATA_BYTES	1 	// STARTER_CHAR
#define QTY_MAX_RX_DATA_BYTES	100

typedef enum ENTER_KEY_VALUE
{
	CHAR_LF = 0x0A,
	CHAR_CR = 0x0D
} EnterKeyValue;

typedef struct
{
	uint8_t starterChar;
	uint8_t separator;
	uint8_t command;
	uint8_t length;
	uint8_t byteIndex;
	Bool enableDecoding;
	uint8_t dataPacket[QTY_MAX_RX_DATA_BYTES];
} TextualProtocol;

void textualProtocolInit(TextualProtocol *textualProtocol, uint8_t starterChar, uint8_t separator);
void textualProtocolAppendByte(TextualProtocol *textualProtocol, uint8_t receivedByte);
void textualProtocolClear(TextualProtocol *textualProtocol);

#endif /* INC_TEXTUALPROTOCOL_H_ */
