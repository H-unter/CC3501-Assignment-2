#include <vector>
#include <iostream>
#include <chrono>
#include <atomic>
#include <mutex>
#include <thread>
#include <semaphore>

#include "hardware/pio.h"
#include "ws2812.pio.h"

void ws2812_program_impl(uint32_t data);
void ws2812_idle_detection_thread();

pio_program_t ws2812_program = ws2812_program_impl;

// Array in which to receive the LED data during each call to pio_sm_put_blocking
std::vector<uint32_t> mock_ws2812_leds;
std::mutex mock_ws2812_leds_mutex;
std::binary_semaphore mock_ws2812_semaphore (0); // to signal that the idle detection thread should wake up

// Storage for the last update 
std::atomic<std::chrono::steady_clock::time_point> last_update;

void ws2812_program_init(PIO pio, unsigned int sm, unsigned int offset, unsigned int pin, float freq, bool rgbw)
{
    last_update.store(std::chrono::steady_clock::now());
    std::thread idle_detection (ws2812_idle_detection_thread);
    idle_detection.detach();
}

void ws2812_program_impl(uint32_t data) 
{
    // Store the LED colour we received
    std::lock_guard<std::mutex> guard(mock_ws2812_leds_mutex);
    mock_ws2812_leds.push_back(data);
    // Reset the idle detection timer (because the real LEDs wait for the bus to go idle before latching the colours)
    last_update.store(std::chrono::steady_clock::now());
    // Signal to the idle detection thread
    mock_ws2812_semaphore.release();
}

// Display the LED status 280us after the last message was posted. This emulates the real wire protocol where idle 
// means to latch the latest values.
void ws2812_idle_detection_thread()
{
    for (;;) {
        // Wait for activity
        mock_ws2812_semaphore.acquire();
        std::this_thread::sleep_for(std::chrono::microseconds(280));
        
        for (;;) {
            // Check that enough time has elapsed
            auto elapsed = std::chrono::steady_clock::now() - last_update.load();
            if (elapsed > std::chrono::microseconds(280)) {
                std::lock_guard<std::mutex> guard(mock_ws2812_leds_mutex);
                if (mock_ws2812_leds.size() > 0 ) {
                    printf("Debug: LEDs (R,G,B) = ");

                    for (uint32_t v : mock_ws2812_leds) {
                        uint8_t r = (0xFF000000 & v) >> 24;
                        uint8_t g = (0xFF0000 & v) >> 16;
                        uint8_t b = (0xFF00 & v) >> 8;
                        printf("(%03u,%03u,%03u),", r, g, b);
                    }
                    printf("\n");

                    mock_ws2812_leds.clear();
                    break;
                }
            }

            // Otherwise, wait 10us and try again 
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    }
}
