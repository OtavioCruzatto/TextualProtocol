/*
 * app.h
 *
 *  Created on: Aug 10, 2024
 *      Author: Otavio
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#include <stdlib.h>
#include "enums.h"
#include "defs.h"
#include "blinkLed.h"
#include "textualProtocol.h"

typedef struct
{
	// ======== LED =========== //
	BlinkLed blinkLed;

	// ======== Textual protocol =========== //
	TextualProtocol textualProtocol;
} App;

// ======== Init =========== //
void appInit(App *app, GPIO_TypeDef* ledPort, uint16_t ledPin,
			UART_HandleTypeDef huart);

// ======== App =========== //
void appExecuteReceivedCommandRoutine(App *app);

// ======== LED =========== //
void appExecuteBlinkLed(App *app);

// ======== Textual protocol =========== //
void appAppendTpByte(App *app, uint8_t receivedByte);
void appExtractTpData(App *app);
void appDecodeExtractedTpCommand(App *app);
void appPrintCurrentTpData(App *app);
void appClearTpData(App *app, TextualProtocolClear textualProtocolClear);

// ======= Getters and Setters ======== //
uint32_t appGetBlinkDelay(App *app);
Bool appGetTpEchoEnable(App *app);
TextualProtocolRxCommandStatus appGetCommandStatus(App *app);

#endif /* INC_APP_H_ */
