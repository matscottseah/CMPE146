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
/*******************************************************************************
 * MSP432 CRC32 - CRC32 Calculation
 *
 * Description: In this example, the CRC32 module is used to calculate a CRC32
 * checksum in CRC32 mode. This value is compared versus a software calculated
 * checksum. The idea here is to have the user use the debugger to step through
 * the code and observe the CRC calculation in the debugger. Note that this
 * code example was made to use the standard CRC32 polynomial value of
 * 0xCBF43926.
 *
 *              MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *
 ******************************************************************************/
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdlib.h>

#define CRC32_POLY              0xEDB88320
#define CRC32_INIT              0xFFFFFFFF

static uint8_t myData[10] = {0};
static uint32_t calculateCRC32(uint8_t* data, uint32_t length);

volatile uint32_t hwCalculatedCRC, swCalculatedCRC;

//  Exercise 1.1
void initRandomArray(uint8_t *buffer, const int length) {
    int i;
    bool odd = true;

    for (i = 0; i < length; i++) {
        int randomNumber = rand();

        if (odd && (randomNumber%2 != 1)) {
            while (randomNumber%2 != 1) {
                randomNumber = rand();
            }
        } else if (!odd && (randomNumber%2 != 0)) {
            while (randomNumber%2 != 0) {
                randomNumber = rand();
            }
        }

        buffer[i] = randomNumber;
        odd = !odd;
    }
}

//  Exercise 1.2
uint32_t reversedBits(uint32_t num) {
    uint32_t bitCount = sizeof(num) * 8;
    uint32_t reversedNumber = 0;
    uint32_t i;
    uint32_t temp;

    for (i = 0; i < bitCount; i++) {
        temp = (num & (1 << i));
        if (temp) reversedNumber |= (1 << ((bitCount - 1) - i));
    }

    return reversedNumber;
}

uint32_t compute_simple_checksum(const uint8_t *data, const uint32_t length) {
    uint32_t sum = 0;
    int i;

    for (i = 0; i < length; i++) {
        sum += data[i];
    }

    return reversedBits(sum);
}

//![Simple CRC32 Example] 
int main(void)
{
    int lengthOfMyData = sizeof(myData);

    //  Exercise 1.1
    initRandomArray((uint8_t*)myData, lengthOfMyData);

    //  Exercise 1.2
    uint32_t checksum = compute_simple_checksum((uint8_t*)myData, lengthOfMyData);

    uint32_t ii;

    /* Stop WDT */
    MAP_WDT_A_holdTimer();

    MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE);

    for (ii = 0; ii < lengthOfMyData; ii++)
        MAP_CRC32_set8BitData(myData[ii], CRC32_MODE);

    /* Getting the result from the hardware module */
    hwCalculatedCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;

    /* Calculating the CRC32 checksum through software */
    swCalculatedCRC = calculateCRC32((uint8_t*)myData, lengthOfMyData);

    /* Pause for the debugger */
    __no_operation();
}
//![Simple CRC32 Example] 

/* Standard software calculation of CRC32 */
static uint32_t calculateCRC32(uint8_t* data, uint32_t length)
{
    uint32_t ii, jj, byte, crc, mask;;

    crc = 0xFFFFFFFF;

    for(ii=0;ii<length;ii++)
    {
        byte = data[ii];
        crc = crc ^ byte;

        for (jj = 0; jj < 8; jj++)
        {
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (CRC32_POLY & mask);
        }

    }

    return ~crc;
}
