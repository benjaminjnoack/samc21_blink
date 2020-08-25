**Contents**

* `samc21_bootloader` [subtree](git@github.com:benjaminjnoack/samc21_bootloader.git)
* `src` blink application code

**TODO**

* find a way to get the debug symbols into the bootloaded image

OK, so the bootloader boot_app call is actually being made in the concatenated binary
(NOTE: don't step over main!)
But R0 and R1 have all kinds of garbage in them, (instead of the pointers I want to be there).
So I'm thinking this function was inlined?

```
$ xxd samc21_blink_bootloaded.bin | grep 4000
00004000: 7824 0020 5d41 0000 5941 0000 5941 0000  x$. ]A..YA..YA..
```

or in little endian
```
$ xxd -e samc21_blink_bootloaded.bin | grep 00004000:
00004000: 20002478 0000415d 00004159 00004159  x$. ]A..YA..YA..
```

I manually set the registers right before the asm instructions ran:

Target.SetReg ("R0", 0x20002478);
Target.SetReg ("R1", 0x0000415d);

