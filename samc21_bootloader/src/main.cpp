#include <cstdint>
#include "sam.h"
#include "memory_map.h"
#include "gnu_build_id.h"
#include "image.h"

#define LED_PIN PORT_PA14

/**
* https://static.docs.arm.com/ihi0042/g/aapcs32.pdf
* section 6.1.1 Core registers
* "The first four registers r0-r3 (a1-a4) are used to pass argument values into a subroutine..."
*/
__attribute__((naked)) void boot_app(uint32_t sp, uint32_t pc);

int main(void)
{
	SystemInit();

	REG_PORT_DIR0 |= LED_PIN;//set the direction to output
	REG_PORT_DIRSET0 |= LED_PIN;//set the pin high (LED on)

	uint32_t i;
	uint32_t sz = __gnu_build_id__.namesz;
	uint8_t data = 0;
	uint32_t *app_start;
	uint32_t sp;
	uint32_t pc;

	/**
	 * There are FAR easier ways to iterate through this data.
	 * However, we aren't actually using the data,
	 * and this prevents the compiler from optimizing it away
	 */
	for (i = 0; i < sz; i++) {
		data = __gnu_build_id__.data[i];
		if (data == '\0') {
			break;
		}
	}

	sz = __gnu_build_id__.namesz + __gnu_build_id__.descsz;
	for (i++; i < sz; i++) {
		data = __gnu_build_id__.data[i];
		if (data == '\0') {
			break;
		}
	}

	struct image_hdr *image_hdr = (struct image_hdr *) &__app_rom_start__;
	if (image_hdr->checksum == IMAGE_CHECKSUM_DEFAULT) {
		goto forever;
	}

	if (image_hdr->timestamp == 0) {
		goto forever;
	}

	sz = (image_hdr->host_size + 1) + (image_hdr->user_size + 1) + (IMAGE_MAGIC_LENGTH + 1);
	/**
	 * another dummy read to step through
	 * data contains ascii text and null terminators
	 * so there should never be any 0xFFs in there
	 */
	for (i = 0; i < sz; i++) {
		data = image_hdr->data[i];
		if (data == 0xFF) {
			break;
		}
	}

	/**
	 * TODO need the size of the image_hdr struct
	 *  plus the data it contains
	 * 	plus whatever gets us to the next word address
	 */
	sz += sizeof(struct image_hdr);
	if (sz % sizeof(uint32_t)) {
		sz += sizeof(uint32_t) - (sz % sizeof(uint32_t));
	}

	sz /= sizeof(uint32_t);

	app_start = (uint32_t *) (&__app_rom_start__ + sz);
	sp = app_start[0];
	pc = app_start[1];
	boot_app(sp, pc);

forever:
	while (1) {
		for (i = 0; i < 100000; i++);

		REG_PORT_OUTTGL0 |= LED_PIN;
	}
}

void boot_app(uint32_t sp, uint32_t pc)
{
	__asm("msr msp, r0\n"
		  "bx r1");
}
