#include "sd_card.h"
#include <stdio.h>

// SDCard constructor
SDCard::SDCard()
{
    // Constructor implementation
}

// Mount the SD card
bool SDCard::mount()
{
    FRESULT fr = f_mount(&fatfs, "0:", 1);
    return (fr == FR_OK);
}

// Unmount the SD card
bool SDCard::unmount()
{
    FRESULT fr = f_mount(NULL, "0:", 1); // Unmount by passing NULL
    return (fr == FR_OK);
}

// Open a file for writing or appending
bool SDCard::open_file(const std::string &filename, FIL &file)
{
    FRESULT fr = f_open(&file, filename.c_str(), FA_WRITE | FA_OPEN_APPEND);
    return (fr == FR_OK);
}

// Close the file on the SD card
bool SDCard::close_file(FIL &file)
{
    FRESULT fr = f_close(&file);
    return (fr == FR_OK);
}

// Write data to the file and flush (sync) it
bool SDCard::write_file_sync(FIL &file, const std::string &data)
{
    UINT bytes_written;
    FRESULT fr = f_write(&file, data.c_str(), data.size(), &bytes_written);
    if (fr != FR_OK || bytes_written != data.size())
    {
        return false;
    }

    // Sync the file (flush the data to the disk)
    fr = f_sync(&file);
    return (fr == FR_OK);
}
