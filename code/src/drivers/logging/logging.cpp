// Logging system, using the style that state is global in the C file.
#include "logging.h"

// --- Device driver internal state:

/// Drop messages whose level is below this threshold.
static LogLevel maxLogLevel = LogLevel::INFORMATION;

// --- Device driver functions
void setLogLevel(LogLevel newLevel)
{
    maxLogLevel = newLevel;
}

void log(LogLevel level, const char *msg)
{
    // Should we show this message?
    if (level < maxLogLevel)
    {
        return;
    }

    // Get the time since boot
    uint32_t time = to_ms_since_boot(get_absolute_time());
    uint32_t time_sec = time / 1000;
    uint32_t time_decimal = (time % 1000);

    // Convert the level to a string
    const char *levelStr;
    switch (level)
    {
    case LogLevel::INFORMATION:
        levelStr = "Information";
        break;
    case LogLevel::WARNING:
        levelStr = "Warning";
        break;
    case LogLevel::ERROR:
        levelStr = "Error";
        break;
    };
    printf("[%u.%03u %s]: %s\n", time_sec, time_decimal, levelStr, msg);
}

// sd card error codes
const char *FRESULT_str(FRESULT fr)
{
    switch (fr)
    {
    case FR_OK:
        return "Success";
    case FR_DISK_ERR:
        return "Disk Error";
    case FR_INT_ERR:
        return "Internal Error";
    case FR_NOT_READY:
        return "Not Ready";
    case FR_NO_FILE:
        return "No File";
    case FR_NO_PATH:
        return "No Path";
    case FR_INVALID_NAME:
        return "Invalid Name";
    case FR_DENIED:
        return "Access Denied";
    case FR_EXIST:
        return "File Already Exists";
    case FR_INVALID_OBJECT:
        return "Invalid Object";
    case FR_WRITE_PROTECTED:
        return "Write Protected";
    case FR_INVALID_DRIVE:
        return "Invalid Drive";
    case FR_NOT_ENABLED:
        return "Not Enabled";
    case FR_NO_FILESYSTEM:
        return "No Filesystem";
    case FR_MKFS_ABORTED:
        return "MKFS Aborted";
    case FR_TIMEOUT:
        return "Timeout";
    case FR_LOCKED:
        return "Locked";
    case FR_NOT_ENOUGH_CORE:
        return "Not Enough Core";
    case FR_TOO_MANY_OPEN_FILES:
        return "Too Many Open Files";
    case FR_INVALID_PARAMETER:
        return "Invalid Parameter";
    default:
        return "Unknown Error";
    }
}

// Datalogging helper functions

// Function to write the CSV header
void write_csv_header(SDCard &sd_card, FIL &file)
{
    char header_buffer[128]; // Increased buffer size to accommodate more columns
    // Updated header to reflect all data columns
    snprintf(header_buffer, sizeof(header_buffer), "elapsed_time_ms,loadcell_voltage_v,loadcell_weight_kg,dac_voltage_v,leaf_temp_c,sap_flow_ml/h\n");
    if (!sd_card.write_file_sync(file, header_buffer))
    {
        printf("Failed to write header\n");
    }
}

void write_csv_row(SDCard &sd_card, FIL &file, data_sample &data)
{
    char row_buffer[128];
    snprintf(row_buffer, sizeof(row_buffer), "%llu,%.2f,%.2f,%.2f,%.2f,%.2f\n",
             data.elapsed_time_ms, data.loadcell_voltage, data.loadcell_weight, data.dac_voltage, data.leaf_temperature, data.sap_flow);
    if (!sd_card.write_file_sync(file, row_buffer))
    {
        printf("Failed to write row\n");
    }
}