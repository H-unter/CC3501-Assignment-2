#include "sd_card.h"

/*!
 * \brief Constructor: Initializes the SD card object.
 */
SDCard::SDCard() : mounted(false) {}

/*!
 * \brief Mounts the SD card.
 *
 * This function mounts the SD card using the FatFs library.
 * It must be called before any file operations are performed.
 *
 * \return true if the SD card was mounted successfully, false otherwise.
 */
bool SDCard::mount()
{
    FRESULT fr = f_mount(&fs, "0:", 1);
    mounted = (FR_OK == fr);
    return mounted;
}

/*!
 * \brief Unmounts the SD card.
 *
 * This function unmounts the SD card to safely stop file operations.
 */
void SDCard::unmount()
{
    if (mounted)
    {
        f_unmount("0:");
        mounted = false;
    }
}

/*!
 * \brief Writes a string to a specified file on the SD card.
 *
 * \param filename The name of the file to write to.
 * \param data The data to write to the file.
 * \return true if the data was written successfully, false otherwise.
 */
bool SDCard::write_file(const std::string &filename, const std::string &data)
{
    if (!mounted)
        return false;

    FIL file;
    FRESULT fr = f_open(&file, filename.c_str(), FA_OPEN_APPEND | FA_WRITE);
    if (fr != FR_OK)
        return false;

    UINT bytes_written;
    fr = f_write(&file, data.c_str(), data.size(), &bytes_written);

    f_close(&file);
    return (fr == FR_OK && bytes_written == data.size());
}

/*!
 * \brief Lists files and directories in a specified directory on the SD card.
 *
 * This function retrieves all files and directories in the specified directory.
 *
 * \param directory The directory to list (e.g., "/").
 * \return A vector of strings representing the file and directory names.
 */
std::vector<std::string> SDCard::list_files(const std::string &directory)
{
    std::vector<std::string> file_list;
    if (!mounted)
        return file_list;

    DIR dir;
    FILINFO fno;

    FRESULT fr = f_opendir(&dir, directory.c_str());
    if (fr == FR_OK)
    {
        while (true)
        {
            fr = f_readdir(&dir, &fno);
            if (fr != FR_OK || fno.fname[0] == 0)
                break; // Break on error or end of directory
            file_list.push_back(fno.fname);
        }
        f_closedir(&dir);
    }
    return file_list;
}
