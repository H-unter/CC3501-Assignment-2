#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/time.h"
// RP2040 hardware includes
#include "hardware/rtc.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

// SD card driver
#include "ff.h"        // FatFs file system library
#include "hw_config.h" // Include your custom hardware config (SPI and SD card)

#include "board.h"      // contains constants for the board
#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"
#include "drivers/sdi12/sdi12.h"

int main()
{
    stdio_init_all();

    // Initialize the Real-Time Clock (RTC)
    rtc_init();

    puts("Reading file from SD card...");

    // Mount the SD card
    sd_card_t *pSD = sd_get_by_num(0);
    FRESULT fr = f_mount(&pSD->fatfs, pSD->pcName, 1);
    if (FR_OK != fr)
        panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);

    FIL fil;
    const char *const filename = "filename.txt";

    // Open the file for reading
    fr = f_open(&fil, filename, FA_READ);
    if (FR_OK != fr)
        panic("f_open(%s) error: %s (%d)\n", filename, FRESULT_str(fr), fr);

    // Read the file and print its content
    char buffer[128];
    while (f_gets(buffer, sizeof(buffer), &fil))
    {
        printf("%s", buffer);
    }

    // Close the file
    fr = f_close(&fil);
    if (FR_OK != fr)
    {
        printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    }

    // Unmount the SD card
    f_unmount(pSD->pcName);

    puts("\nFinished reading file.");
    for (;;)
    {
        tight_loop_contents();
    }
}