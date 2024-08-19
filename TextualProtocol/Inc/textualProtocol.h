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
#include <stdio.h>
#include "defs.h"
#include "enums.h"
#include "stm32f4xx_hal.h"

#define	QTY_MIN_RX_DATA_BYTES		2 	// STARTER_CHAR

/*
 * QTY_MAX_RX_DATA_BYTES =
 * QTY_MAX_OF_BYTES_IN_COMMAND +
 * (QTY_MAX_OF_VALUES * QTY_MAX_OF_BYTES_PER_VALUE) +
 * QTY_MAX_OF_DELIMITERS +
 * STARTER_CHAR +
 * NEW_LINE(CR AND LF) +
 * NULL_CHAR
 *
 * QTY_MAX_RX_DATA_BYTES = 11 + (10 * 21) + 10 + 1 + 2 + 1 = 235
 */
#define QTY_MAX_RX_DATA_BYTES		235

#define QTY_MAX_OF_VALUES			10
#define QTY_MAX_OF_BYTES_PER_VALUE	21
#define QTY_MAX_OF_DELIMITERS		QTY_MAX_OF_VALUES
#define	QTY_MAX_OF_BYTES_IN_COMMAND	11

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

typedef enum TEXTUAL_PROTOCOL_STATUS_MESSAGES
{
	STATUS_MESSAGE_OK = 0xAA,
	STATUS_MESSAGE_SYNTAX_ERROR,
	STATUS_MESSAGE_UNKNOWN_COMMAND,
	STATUS_MESSAGE_VALUE_ERROR
} TextualProtocolStatusMessages;

typedef enum TEXTUAL_PROTOCOL_CLEAR
{
	CLEAR_ALL = 0x01,
	CLEAR_VALUES,
	CLEAR_DATA_PACKET,
	CLEAR_INDEXES_OF_DELIMITERS,
	CLEAR_COMMAND,
	CLEAR_AFTER_EXTRACT_DATA
} TextualProtocolClear;

typedef enum TEXTUAL_PROTOCOL_RX_COMMANDS
{
	CMD_RX_COMMAND_UNKNOWN = 0x00,
	CMD_RX_SET_ECHO_STATUS = 0x01,
	CMD_RX_SET_BLINK_PATTERN = 0x02
} DecodedReceivedCommands;

typedef struct
{
	uint8_t starterChar;
	uint8_t delimiter;
	uint8_t commandLength;
	uint8_t commandCode;
	uint8_t length;
	uint8_t byteIndex;
	Bool enableExtractData;
	Bool enableEcho;
	Bool enableDecodeExtractedCommand;
	uint8_t qtyOfDelimiters;
	uint8_t dataPacket[QTY_MAX_RX_DATA_BYTES];
	uint8_t values[QTY_MAX_OF_VALUES][QTY_MAX_OF_BYTES_PER_VALUE];
	uint8_t indexesOfDelimiters[QTY_MAX_OF_DELIMITERS];
	uint8_t command[QTY_MAX_OF_BYTES_IN_COMMAND];
	TextualProtocolRxStatus textualProtocolRxStatus;
	UART_HandleTypeDef huart;
} TextualProtocol;

void textualProtocolInit(TextualProtocol *textualProtocol, uint8_t starterChar, uint8_t separator, UART_HandleTypeDef huart);
void textualProtocolAppendByte(TextualProtocol *textualProtocol, uint8_t receivedByte);
void textualProtocolClear(TextualProtocol *textualProtocol, TextualProtocolClear clear);
void textualProtocolSendStatusMessage(TextualProtocol *textualProtocol, TextualProtocolStatusMessages statusMessage);
void textualProtocolExtractData(TextualProtocol *textualProtocol);
void textualProtocolSendNewLine(TextualProtocol *textualProtocol);
void textualProtocolExtractCommand(TextualProtocol *textualProtocol);
void textualProtocolFindDelimiters(TextualProtocol *textualProtocol);
void textualProtocolExtractValues(TextualProtocol *textualProtocol);
void textualProtocolDecodeExtractedCommand(TextualProtocol *textualProtocol);
void textualProtocolPrintCurrentData(TextualProtocol *textualProtocol);

Bool textualProtocolGetEchoEnable(TextualProtocol *textualProtocol);

#endif /* INC_TEXTUALPROTOCOL_H_ */
