#include <cstdint>
#include "sam.h"
#include "memory_map.h"

#define LED_PIN PORT_PA14

__attribute__((naked)) void boot_app(uint32_t sp, uint32_t pc);

int main(void)
{
    SystemInit();

    REG_PORT_DIR0 |= LED_PIN;//set the direction to output
    REG_PORT_OUT0 |= LED_PIN;//set the pin high (LED off)

    /**
     * __app_rom_start__ is a linker symbol
     * The value of the symbol is the address of the start of app rom (0x00004000)
     */
    uint32_t *app_start = &__app_rom_start__;
    /**
     * The contents of the first memory word are the stack pointer for the app
     * It will be loaded into the MSP register of the processor
     */
    uint32_t sp = app_start[0];
    /**
     * The contents of the second memory word is the program counter for the app
     * It will be loaded into the PC register of the processor
     */
    uint32_t pc = app_start[1];

    /**
    * https://static.docs.arm.com/ihi0042/g/aapcs32.pdf
    * section 6.1.1 Core registers
    * "The first four registers r0-r3 (a1-a4) are used to pass argument values into a subroutine..."
     * TODO inline extended assembly
    */
    boot_app(sp, pc);

    while (1)
    {
        for (uint32_t i = 0; i < 100000; i++)
            ;

        REG_PORT_OUTTGL0 |= LED_PIN;
    }
}

void boot_app(uint32_t sp, uint32_t pc)
{
    __asm("msr msp, r0\n"
          "bx r1");
}
