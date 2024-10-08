/*
 * app.c
 *
 *  Created on: Aug 10, 2024
 *      Author: Otavio
 */

#include "app.h"

// ======== Init =========== //
void appInit(App *app, GPIO_TypeDef* ledPort, uint16_t ledPin, UART_HandleTypeDef huart)
{
	// ======== LED =========== //
	blinkLedInit(&app->blinkLed, ledPort, ledPin, PATTERN_TOGGLE_EACH_100_MS);

	// ======== Textual protocol =========== //
	textualProtocolInit(&app->textualProtocol, '$', ',', huart);
}

// ======== App =========== //
void appExecuteReceivedCommandRoutine(App *app)
{
	uint8_t sizeOfStringValue = 0;
	uint8_t blinkPattern = 0;

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
			sizeOfStringValue = strlen((char *) app->textualProtocol.values[0]);
			if (sizeOfStringValue > 0)
			{
				blinkPattern = atoi((char *) app->textualProtocol.values[0]);
				if ((blinkPattern >= 0) && (blinkPattern <=5))
				{
					blinkLedSetBlinkPattern(&app->blinkLed, blinkPattern);
					textualProtocolSendStatusMessage(&app->textualProtocol, STATUS_MESSAGE_OK);
				}
				else
				{
					textualProtocolSendStatusMessage(&app->textualProtocol, STATUS_MESSAGE_VALUE_ERROR);
				}
			}
			else
			{
				textualProtocolSendStatusMessage(&app->textualProtocol, STATUS_MESSAGE_VALUE_ERROR);
			}
			break;

		default:
			break;
	}

	textualProtocolClear(&app->textualProtocol, CLEAR_ALL);
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
