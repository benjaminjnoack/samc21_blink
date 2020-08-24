**Contents**

* `samc21_bootloader` [subtree](git@github.com:benjaminjnoack/samc21_bootloader.git)
* `src` blink application code

**TODO**

* find a way to get the debug symbols into the bootloaded image

OK, so the bootloader boot_app call is actually being made in the concatenated binary
(NOTE: don't step over main!)
But R0 and R1 have all kinds of garbage in them, (instead of the pointers I want to be there).
So I'm thinking this function was inlined?

$ xxd samc21_blink.bin | head
00000000: 7824 0020 5d41 0000 5941 0000 5941 0000  x$. ]A..YA..YA..

So I manually set the registers right before the asm instructions ran:

Target.SetReg ("R0", 0x20002478);
Target.SetReg ("R1", 0x0000415d);
