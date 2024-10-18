#ifndef SD_CARD_H
#define SD_CARD_H

#include <string>
#include <vector>
#include "ff.h" // FatFs library

/*!
 * \brief SDCard class for handling file operations on an SD card using FatFs.
 *
 * This class provides a simple interface to mount/unmount the SD card, open/close files,
 * and write data, using the FatFs file system.
 */
class SDCard
{
public:
    /*!
     * \brief SDCard Constructor
     */
    SDCard();

    /*!
     * \brief Mount the SD card
     * \return true if the SD card is successfully mounted, false otherwise
     */
    bool mount();

    /*!
     * \brief Unmount the SD card
     * \return true if the SD card is successfully unmounted, false otherwise
     */
    bool unmount();

    /*!
     * \brief Open a file on the SD card for writing or appending
     * \param filename The name of the file to open
     * \param file FIL object to hold the file reference
     * \return true if the file is successfully opened, false otherwise
     */
    bool open_file(const std::string &filename, FIL &file);

    /*!
     * \brief Close the file on the SD card
     * \param file The FIL object representing the file to close
     * \return true if the file is successfully closed, false otherwise
     */
    bool close_file(FIL &file);

    /*!
     * \brief Write data to the open file on the SD card and flush to disk
     * \param file The open file object
     * \param data The string data to write
     * \return true if data is successfully written and synced, false otherwise
     */
    bool write_file_sync(FIL &file, const std::string &data);

private:
    FATFS fatfs; // FatFs file system object
};

#endif // SD_CARD_H
