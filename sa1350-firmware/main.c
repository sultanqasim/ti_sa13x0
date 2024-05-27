/*!
 *  @file main.c
 */
#include "SA1350_Firmware.h"


/** @brief main
 */
int32_t main(void)
{
    /* Call board initialization functions. */
    Board_initGeneral();

    if (!ChipInfo_ChipFamilyIs_CC13x0())
    {
        System_abort("Error unsupported device\n");
    }

    /* Initialize tasks */
    RfTask_init();
    DisplayTask_init();
    ButtonTask_init();
    UartTask_init();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
