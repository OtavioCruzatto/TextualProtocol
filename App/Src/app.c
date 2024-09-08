/*
 * app.c
 *
 *  Created on: Aug 10, 2024
 *      Author: Otavio
 */

#include "app.h"

// ======== Init =========== //
void appInit(App *app, GPIO_TypeDef* ledPort, uint16_t ledPin,
			UART_HandleTypeDef huart)
{
	// ======== LED =========== //
	blinkLedInit(&app->blinkLed, ledPort, ledPin, PATTERN_TOGGLE_EACH_100_MS);

	// ======== Textual protocol =========== //
	textualProtocolInit(&app->textualProtocol, '$', ',', huart);
}

// ======== LED =========== //
void appExecuteBlinkLed(App *app)
{
	blinkLedExecuteBlink(&app->blinkLed);
}

// ======== Textual protocol =========== //
void appAppendTpByte(App *app, uint8_t receivedByte)
{
	textualProtocolAppendByte(&app->textualProtocol, receivedByte);
}

void appExtractTpData(App *app)
{
	textualProtocolExtractData(&app->textualProtocol);
}

void appDecodeExtractedTpCommand(App *app)
{
	textualProtocolDecodeExtractedCommand(&app->textualProtocol);
}

void appPrintCurrentTpData(App *app)
{
	textualProtocolPrintCurrentData(&app->textualProtocol);
}

void appClearTpData(App *app, TextualProtocolClear clear)
{
	textualProtocolClear(&app->textualProtocol, clear);
}

void appExecuteReceivedCommandRoutine(App *app)
{
	switch (textualProtocolGetDecodedCommand(&app->textualProtocol))
	{
		case CMD_RX_COMMAND_UNKNOWN:
			break;

		case CMD_RX_SET_ECHO_STATUS:
			if (app->textualProtocol.values[0][0] == '0')
			{
				textualProtocalSetEchoEnable(&app->textualProtocol, FALSE);
				textualProtocolSendStatusMessage(&app->textualProtocol, STATUS_MESSAGE_OK);
			}
			else if (app->textualProtocol.values[0][0] == '1')
			{
				textualProtocalSetEchoEnable(&app->textualProtocol, TRUE);
				textualProtocolSendStatusMessage(&app->textualProtocol, STATUS_MESSAGE_OK);
			}
			else
			{
				textualProtocolSendStatusMessage(&app->textualProtocol, STATUS_MESSAGE_VALUE_ERROR);
			}
			break;

		case CMD_RX_SET_BLINK_PATTERN:
			textualProtocolSendStatusMessage(&app->textualProtocol, STATUS_MESSAGE_OK);
			break;
	}

	textualProtocolClear(&app->textualProtocol, CLEAR_ALL);
}

// ======= Getters and Setters ======== //
uint32_t appGetBlinkDelay(App *app)
{
	return blinkLedGetBlinkDelay(&app->blinkLed);
}

Bool appGetTpEchoEnable(App *app)
{
	return textualProtocolGetEchoEnable(&app->textualProtocol);
}

TextualProtocolRxCommandStatus appGetCommandStatus(App *app)
{
	return textualProtocolGetCommandStatus(&app->textualProtocol);
}
