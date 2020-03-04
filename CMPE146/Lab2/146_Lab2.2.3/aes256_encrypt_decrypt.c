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
 * MSP432 AES256 -  Encryption and Decryption Example
 *
 *  Description: This demo shows a simple example of encryption and decryption
 *  using the AES356 module.
 *
 *               MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *
 * Key: 000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f
 * Plaintext: 00112233445566778899aabbccddeeff
 * Ciphertext: 8ea2b7ca516745bfeafc49904b496089
 *
 *******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Statics */
static uint8_t Data[16] =
{ 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc,
        0xdd, 0xee, 0xff };
static uint8_t CipherKey[32] =
{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c,
        0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };
static uint8_t DataAESencrypted[1600];       // Encrypted data
static uint8_t DataAESdecrypted[1600];       // Decrypted data

void encrypt_message(char* str, uint8_t* encrypted, uint8_t* key) {
    /* Load a cipher key to module */
    MAP_AES256_setCipherKey(AES256_BASE, key, AES256_KEYLENGTH_256BIT);

    int stringLength = strlen(str);
    int numberOfGroups = (stringLength%16 == 0) ? (stringLength/16) : (stringLength/16) + 1;

    int i;
    for (i = 0; i < numberOfGroups; i++) {
        int startIndex = 16 * i;
        int endIndex = startIndex + 16;

        static uint8_t tempData[16];
        int k = 0;
        int j;
        for (j = startIndex; j < endIndex; j++) {
            tempData[k] = (uint8_t)(str[j]);
            k++;
        }

        static uint8_t tempEncrypted[16];
        /* Encrypt data with preloaded cipher key */
        MAP_AES256_encryptData(AES256_BASE, tempData, tempEncrypted);

        k = 0;
        for (j = startIndex; j < endIndex; j++) {
            DataAESencrypted[j] = tempEncrypted[k];
            k++;
        }
    }
}

void decrypt_message(uint8_t* data, int data_length, uint8_t* decrypted, uint8_t* key) {
    /* Load a decipher key to module */
    MAP_AES256_setDecipherKey(AES256_BASE, CipherKey,AES256_KEYLENGTH_256BIT);

    int numberOfGroups = (data_length%16 == 0) ? (data_length/16) : (data_length/16) + 1;

    int i;
    for (i = 0; i < numberOfGroups; i++) {
        int startIndex = 16 * i;
        int endIndex = startIndex + 16;

        static uint8_t tempData[16];
        int k = 0;
        int j;
        for (j = startIndex; j < endIndex; j++) {
            tempData[k] = data[j];
            k++;
        }

        static uint8_t tempDecrypted[16];
        /* Decrypt data with keys that were generated during encryption*/
        MAP_AES256_decryptData(AES256_BASE, tempData, tempDecrypted);

        k = 0;
        for (j = startIndex; j < endIndex; j++) {
            DataAESdecrypted[j] = tempDecrypted[k];
            k++;
        }
    }
}

int main(void) {
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();
    
    int i;
    char* message = "How much wood could a wood chuck chuck if a wood chuck could chuck wood?";

    encrypt_message(message, DataAESencrypted, CipherKey);

    decrypt_message(DataAESencrypted, strlen(message), DataAESdecrypted, CipherKey);

    printf("\nOriginal Message: %s", message);

    printf("\n\nEncrypted Message: ");
    for (i = 0; i < strlen(message); i++) {
        if (i%32 == 0) {
            printf("\n");
        }
        printf("0x%02X ", DataAESencrypted[i]);
    }

    printf("\n\nDecrypted Message: ");
    for (i = 0; i < strlen(message); i++) {
        printf("%c", (char)(DataAESdecrypted[i]));
    }
}
