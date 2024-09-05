#include <vector>
#include "hardware/pio.h"

PIO pio0 = 0;
static std::vector<pio_program_t> pio_programs;

unsigned int pio_add_program(PIO pio, const pio_program_t* program)
{
    pio_programs.push_back(*program);
    return 0;
}

void pio_sm_put_blocking(PIO pio, unsigned int sm, uint32_t data)
{
    for (auto program : pio_programs) {
        program(data);
    }
}
