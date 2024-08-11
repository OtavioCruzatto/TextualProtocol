/*
 * app.h
 *
 *  Created on: Aug 10, 2024
 *      Author: Otavio
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#include "enums.h"
#include "defs.h"
#include "blinkLed.h"

typedef struct App App;

struct App
{
	// ======== LED =========== //
	BlinkLed blinkLed;
};

// ======== Init =========== //
void appInit(App *app, GPIO_TypeDef* ledPort, uint16_t ledPin,
			UART_HandleTypeDef huart);

// ======== LED =========== //
void appExecuteBlinkLed(App *app);

// ======= Getters and Setters ======== //
uint32_t appGetBlinkDelay(App *app);

#endif /* INC_APP_H_ */
