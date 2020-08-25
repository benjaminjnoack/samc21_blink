**Contents**

* `samc21_bootloader` [subtree](git@github.com:benjaminjnoack/samc21_bootloader.git)
* `src` blink application code

**TODO**

* find a way to get the debug symbols into the bootloaded image
    * only assembly debugging is available from the concatenated binary image
    * binary has the advantage of being what we will likely send over CAN
    * could have a single ELF if the debug code didn't have main and other unique symbols
        * would be a mess of `#define` statements
 
* blink using an interrupt to demonstrate that the interrupt table has been offset correctly