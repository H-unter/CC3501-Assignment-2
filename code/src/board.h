// hardware related definitions
#define SDI12_UART_INSTANCE uart1 //
#define SDI12_UART_TX_PIN 8
#define SDI12_UART_RX_PIN 9
#define ENABLE_DRIVE_PIN 15

// sdi-12 parameter definitions
#define SDI12_BAUD_RATE 1200
// sensor response/transmit times (pg.41 of datasheet)
#define SDI12_MAX_RESPONSE_TIME_MS 15
#define SDI12_MAX_TRANSMIT_TIME_MS 380            // min sensor response time for most commands
#define SDI12_MAX_TRANSMIT_TIME_CONCURRENT_MS 810 // max sensor response time for concurrent measurement