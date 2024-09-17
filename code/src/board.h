// hardware related definitions
#define SDI12_UART_INSTANCE uart1 //
#define SDI12_UART_TX_PIN 4
#define SDI12_UART_RX_PIN 5
#define ENABLE_DRIVE_PIN 3 // for SDI12 data line drive enable TODO: rename for cleanliness

// sdi-12 parameter definitions
#define SDI12_BAUD_RATE 1200
// sensor response/transmit times (pg.41 of datasheet)
#define SDI12_MAX_RESPONSE_TIME_MS 15             // max sensor response time for most commands
#define SDI12_MAX_TRANSMIT_TIME_MS 380            // max sensor transmitting time for most commands
#define SDI12_MAX_TRANSMIT_TIME_CONCURRENT_MS 810 // max sensor transmitting time for concurrent measurement (longer than normal)

// SPI and SD card related definitions
#define SD_SPI_INSTANCE spi0     // SPI instance for SD card communication
#define SD_SPI_MISO_PIN 16       // MISO (Master In Slave Out) GPIO pin: SD_MISO from schematic
#define SD_SPI_SS_PIN 17         // Slave Select (SS) GPIO pin: SD_CS from schematic
#define SD_SPI_SCK_PIN 18        // Clock (SCK) GPIO pin: SD_CLK from schematic
#define SD_SPI_MOSI_PIN 19       // MOSI (Master Out Slave In) GPIO pin: SD_MOSI from schematic
#define SD_SPI_BAUD_RATE 1000000 // Baud rate for SD card SPI
