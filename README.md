**Contents**

* `samc21_bootloader` [subtree](git@github.com:benjaminjnoack/samc21_bootloader.git)
* `src` blink application code

**TODO**

* find a way to get the debug symbols into the bootloaded image
    * ensure the bootloader and app can both be flashed and debugged separately
    * there is no great advantage to seeing it jump from the boot loader to the app
        * except perhaps to ensure the micro is in a known state (peripheral config) before the app boots
    * only assembly debugging is available from the concatenated binary image
    * binary has the advantage of being what we will likely send over CAN
    * could have a single ELF if the debug code didn't have main and other unique symbols
        * would be a mess of `#define` statements
 
* blink using an interrupt to demonstrate that the interrupt table has been offset correctly
* app image validation
* bootloaded target
    * CLion requires a target it knows about