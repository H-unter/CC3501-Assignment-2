#ifndef SD_CARD_H
#define SD_CARD_H

#include <stdio.h>
#include "ff.h"
#include <string>
#include <vector>

/*!
 * \brief SDCard class handles mounting, unmounting, and file operations on the SD card.
 */
class SDCard
{
public:
    /*!
     * \brief Constructor: Initializes the SD card object.
     */
    SDCard();

    /*!
     * \brief Mounts the SD card.
     *
     * This function mounts the SD card using the FatFs library.
     * It must be called before any file operations are performed.
     *
     * \return true if the SD card was mounted successfully, false otherwise.
     */
    bool mount();

    /*!
     * \brief Unmounts the SD card.
     *
     * This function unmounts the SD card to safely stop file operations.
     */
    void unmount();

    /*!
     * \brief Writes a string to a specified file on the SD card.
     *
     * \param filename The name of the file to write to.
     * \param data The data to write to the file.
     * \return true if the data was written successfully, false otherwise.
     */
    bool write_file(const std::string &filename, const std::string &data);

    /*!
     * \brief Lists files and directories in a specified directory on the SD card.
     *
     * This function retrieves all files and directories in the specified directory.
     *
     * \param directory The directory to list (e.g., "/").
     * \return A vector of strings representing the file and directory names.
     */
    std::vector<std::string> list_files(const std::string &directory);

private:
    FATFS fs;     // FatFs file system object
    bool mounted; // Indicates whether the SD card is currently mounted
};

#endif // SD_CARD_H
