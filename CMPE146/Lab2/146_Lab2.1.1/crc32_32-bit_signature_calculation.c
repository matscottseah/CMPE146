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

static uint8_t myData[1024] = {0};
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
uint32_t compute_simple_checksum(const uint8_t *data, const uint32_t length) {
    uint32_t sum = 0;
    int i;

    for (i = 0; i < length; i++) {
        sum += data[i];
    }

    return ~sum;
}

//  Exercise 1.3
void startTimer() {
    /* Setup counter */
    MAP_Timer32_initModule(TIMER32_0_BASE,
                           TIMER32_PRESCALER_1,
                           TIMER32_32BIT,
                           TIMER32_FREE_RUN_MODE);

    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
}

uint32_t getTimerValue() {
    return MAP_Timer32_getValue(TIMER32_0_BASE);
}

uint32_t computeElapsedTimeInMicroseconds(const uint32_t t0, const uint32_t t1) {
    float clockFrequency = MAP_CS_getMCLK();
    float elapsedTime = t0 - t1;
    float elapsedTimeInMicroseconds = (elapsedTime/clockFrequency) * 1000000;

    return elapsedTimeInMicroseconds;
}

//![Simple CRC32 Example] 
int main(void)
{
    startTimer();

    int lengthOfMyData = sizeof(myData);

    //  Exercise 1.1  ------------------------------------------------------------

    initRandomArray((uint8_t*)myData, lengthOfMyData);

    //  Exercise 1.2  ------------------------------------------------------------

    uint32_t simpleChecksum_t0 = getTimerValue();

    uint32_t simpleChecksum = compute_simple_checksum((uint8_t*)myData, lengthOfMyData);

    uint32_t simpleChecksum_t1 = getTimerValue();

    //  Exercise 1.3  ------------------------------------------------------------
    uint32_t simpleChecksumElapsedTime = computeElapsedTimeInMicroseconds(simpleChecksum_t0, simpleChecksum_t1);
    printf("\nSimple Checksum: %u\nElapsed Time: %u us\n", simpleChecksum, simpleChecksumElapsedTime);

    //  --------------------------------------------------------------------------

    uint32_t ii;

    /* Stop WDT */
    MAP_WDT_A_holdTimer();

    MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE);

    for (ii = 0; ii < lengthOfMyData; ii++)
        MAP_CRC32_set8BitData(myData[ii], CRC32_MODE);

    uint32_t hwChecksum_t0 = getTimerValue();

    /* Getting the result from the hardware module */
    hwCalculatedCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;

    uint32_t hwChecksum_t1 = getTimerValue();

    //  Exercise 1.3  ------------------------------------------------------------
    uint32_t hwChecksumElapsedTime = computeElapsedTimeInMicroseconds(hwChecksum_t0, hwChecksum_t1);
    printf("\nHardware Checksum: %u\nElapsed Time: %u us\n", hwCalculatedCRC, hwChecksumElapsedTime);

    //  --------------------------------------------------------------------------

    uint32_t swChecksum_t0 = getTimerValue();

    /* Calculating the CRC32 checksum through software */
    swCalculatedCRC = calculateCRC32((uint8_t*)myData, lengthOfMyData);

    uint32_t swChecksum_t1 = getTimerValue();

    //  Exercise 1.3  ------------------------------------------------------------
    uint32_t swChecksumElapsedTime = computeElapsedTimeInMicroseconds(swChecksum_t0, swChecksum_t1);
    printf("\nSoftware Checksum: %u\nElapsed Time: %u us\n", swCalculatedCRC, swChecksumElapsedTime);
    uint32_t speedup = swChecksumElapsedTime - hwChecksumElapsedTime;
    printf("\nSpeedup: %u us\n", speedup);

    //  Exercise 1.4  ------------------------------------------------------------
    myData[20] = ~(myData[20]);
    printf("\nReverse myData[20]:");

    for (ii = 0; ii < lengthOfMyData; ii++)
            MAP_CRC32_set8BitData(myData[ii], CRC32_MODE);

    hwChecksum_t0 = getTimerValue();

    /* Getting the result from the hardware module */
    hwCalculatedCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;

    hwChecksum_t1 = getTimerValue();

    hwChecksumElapsedTime = computeElapsedTimeInMicroseconds(hwChecksum_t0, hwChecksum_t1);
    printf("\nHardware Checksum: %u\nElapsed Time: %u us\n", hwCalculatedCRC, hwChecksumElapsedTime);

    //  --------------------------------------------------------------------------

    simpleChecksum_t0 = getTimerValue();

    simpleChecksum = compute_simple_checksum((uint8_t*)myData, lengthOfMyData);

    simpleChecksum_t1 = getTimerValue();

    simpleChecksumElapsedTime = computeElapsedTimeInMicroseconds(simpleChecksum_t0, simpleChecksum_t1);
    printf("\nSimple Checksum: %u\nElapsed Time: %u us\n", simpleChecksum, simpleChecksumElapsedTime);

    //  --------------------------------------------------------------------------

    myData[21] = ~(myData[21]);
    printf("\nReverse myData[21]:");

    for (ii = 0; ii < lengthOfMyData; ii++)
            MAP_CRC32_set8BitData(myData[ii], CRC32_MODE);

    hwChecksum_t0 = getTimerValue();

    /* Getting the result from the hardware module */
    hwCalculatedCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;

    hwChecksum_t1 = getTimerValue();

    hwChecksumElapsedTime = computeElapsedTimeInMicroseconds(hwChecksum_t0, hwChecksum_t1);
    printf("\nHardware Checksum: %u\nElapsed Time: %u us\n", hwCalculatedCRC, hwChecksumElapsedTime);

    //  --------------------------------------------------------------------------

    simpleChecksum_t0 = getTimerValue();

    simpleChecksum = compute_simple_checksum((uint8_t*)myData, lengthOfMyData);

    simpleChecksum_t1 = getTimerValue();

    simpleChecksumElapsedTime = computeElapsedTimeInMicroseconds(simpleChecksum_t0, simpleChecksum_t1);
    printf("\nSimple Checksum: %u\nElapsed Time: %u us\n", simpleChecksum, simpleChecksumElapsedTime);

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
