/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 * MSP432 Empty Project
 *
 * Description: An empty project that uses DriverLib
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 * Author: 
*******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

void delay_ms(uint32_t count) {
    float clockFrequency = MAP_CS_getMCLK();
    uint32_t multiplier = clockFrequency / 10000;
    uint32_t milliseconds = count * 300;

    volatile uint32_t ii;

    /* Delay Loop */
    for(ii=0;ii<milliseconds;ii++)
    {
    }
}

uint32_t computeElapsedTimeInMilliseconds(const uint32_t t0, const uint32_t t1) {
    float clockFrequency = MAP_CS_getMCLK();
    float elapsedTime = t0 - t1;
    float elapsedTimeInMilliseconds = (elapsedTime/clockFrequency) * 1000;

    return elapsedTimeInMilliseconds;
}


int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    /* Initialize Timer */
    MAP_Timer32_initModule(TIMER32_0_BASE,
                           TIMER32_PRESCALER_1,
                           TIMER32_32BIT,
                           TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);

    /* Configuring P1.0 as output */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1);

    uint32_t delayInMilliseconds = 2000;

    uint32_t t0;
    uint32_t t1;
    uint32_t t2;

    while(1)
    {
        t0 = MAP_Timer32_getValue(TIMER32_0_BASE);

        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN0);
        delay_ms(delayInMilliseconds);

        t1 = MAP_Timer32_getValue(TIMER32_0_BASE);

        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);
        delay_ms(delayInMilliseconds);

        t2 = MAP_Timer32_getValue(TIMER32_0_BASE);

        uint32_t RED_time = computeElapsedTimeInMilliseconds(t0, t1);
        uint32_t GREEN_time = computeElapsedTimeInMilliseconds(t1, t2);

        printf("Time for red LED: %u ms\n", RED_time);
        printf("Time for green LED: %u ms\n", GREEN_time);
    }
}
