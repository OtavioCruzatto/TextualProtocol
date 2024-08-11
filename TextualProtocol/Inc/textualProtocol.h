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
#include "stm32f4xx_hal.h"

#define	QTY_MIN_RX_DATA_BYTES	1 	// STARTER_CHAR
#define QTY_MAX_RX_DATA_BYTES	100

typedef enum ENTER_KEY_VALUE
{
	CHAR_LF = 0x0A,
	CHAR_CR = 0x0D
} EnterKeyValue;

typedef enum TEXTUAL_PROTOCOL_RX_STATUS
{
	INVALID_RX_TEXTUAL_PROTOCOL = 0x00,
	VALID_RX_TEXTUAL_PROTOCOL
} TextualProtocolRxStatus;

typedef struct
{
	uint8_t starterChar;
	uint8_t separator;
	uint8_t command;
	uint8_t length;
	uint8_t byteIndex;
	Bool enableDecoding;
	Bool enableEcho;
	uint8_t dataPacket[QTY_MAX_RX_DATA_BYTES];
	TextualProtocolRxStatus textualProtocolRxStatus;
	UART_HandleTypeDef huart;
} TextualProtocol;

void textualProtocolInit(TextualProtocol *textualProtocol, uint8_t starterChar, uint8_t separator, UART_HandleTypeDef huart);
void textualProtocolAppendByte(TextualProtocol *textualProtocol, uint8_t receivedByte);
void textualProtocolClear(TextualProtocol *textualProtocol);
void textualProtocolSendSyntaxError(TextualProtocol *textualProtocol);
void textualProtocolDecode(TextualProtocol *textualProtocol);
void textualProtocolSendNewLine(TextualProtocol *textualProtocol);

Bool textualProtocolGetEchoEnable(TextualProtocol *textualProtocol);

#endif /* INC_TEXTUALPROTOCOL_H_ */
