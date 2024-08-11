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
}

// ======== LED =========== //
void appExecuteBlinkLed(App *app)
{
	blinkLedExecuteBlink(&app->blinkLed);
}

uint32_t appGetBlinkDelay(App *app)
{
	return blinkLedGetBlinkDelay(&app->blinkLed);
}
