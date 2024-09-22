// src/drivers/hw_config.c
#include "no-OS-FatFS/FatFs_SPI/sd_driver/spi.h"
#include "no-OS-FatFS/FatFs_SPI/sd_driver/sd_card.h"
#include "board.h" // Include your board definitions

// Custom SPI configuration using board constants
spi_t sd_spi = {
    // Renamed from spi0 to sd_spi to avoid confusion
    .hw_inst = SD_SPI_INSTANCE,    // SPI hardware instance from board.h
    .miso_gpio = SD_SPI_MISO_PIN,  // GPIO for MISO from board.h
    .mosi_gpio = SD_SPI_MOSI_PIN,  // GPIO for MOSI from board.h
    .sck_gpio = SD_SPI_SCK_PIN,    // GPIO for SCK from board.h
    .baud_rate = SD_SPI_BAUD_RATE, // SPI baud rate from board.h
    .set_drive_strength = true,
    .mosi_gpio_drive_strength = GPIO_DRIVE_STRENGTH_8MA,
    .sck_gpio_drive_strength = GPIO_DRIVE_STRENGTH_8MA,
};

// Custom SD card 0 configuration using board constants
sd_card_t sd0 = {
    .pcName = "0:",           // Logical drive name
    .spi = &sd_spi,           // Associated SPI instance (renamed to sd_spi)
    .ss_gpio = SD_SPI_SS_PIN, // Slave select GPIO for the SD card from board.h
    .use_card_detect = false, // Use a card detect pin
    //.card_detect_gpio = SD_CARD_DETECT_PIN, // Card detect GPIO from board.h
    .card_detected_true = 1, // Active-high card detect
    .set_drive_strength = true,
    .ss_gpio_drive_strength = GPIO_DRIVE_STRENGTH_8MA,
};

// Functions to return the number of SPIs and SD cards
size_t spi_get_num()
{
    return 1;
}

spi_t *spi_get_by_num(size_t num)
{
    return &sd_spi; // Updated reference to sd_spi
}

size_t sd_get_num()
{
    return 1;
}

sd_card_t *sd_get_by_num(size_t num)
{
    return &sd0;
}