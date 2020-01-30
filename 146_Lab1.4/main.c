#include "msp.h"


/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    int *address = (int *)0x201000;
    char *descriptions[17] = {
        "TLV checksum",
        "Device Info Tag",
        "Device Info Length",
        "Device ID",
        "Hardware Revision",
        "Boot-Code Revision",
        "ROM Driver Library Revision",
        "Die Record Tag",
        "Die Record Length",
        "Die X Position",
        "Die Y Position",
        "Wafer ID",
        "Lot ID",
        "Reserved",
        "Reserved",
        "Reserved",
        "Test Results"
    };

    int i;
    for(i = 0; i < 17; i = i + 1) {
        printf("\n%i. %s\n", i + 1, descriptions[i]);
        printf("Memory address is: 0x%08X\n", address);
        printf("Content of that address is: 0x%08X\n", *address);
        address += 0x1;
    }
}
