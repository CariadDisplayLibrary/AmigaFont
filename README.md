AmigaFont
=========

This is a Font class which takes the data from an Amiga bitmap font file
(not the .font file, the numeric file in the directory named after the font,
for instance `ruby/8`).

It handles proportional spacing and kerning, though negative kerning, when
background and foreground differ (i.e., in non-transparent background mode)
may cause the right portion of one character to be overwritten by the next.

Usage
-----

```C++
#include <AmigaFont.h>

const uint8_t *fontData = {
 0xaa, 0xbb, ... 0xff // All the font data (however you want it to be)
};


AmigaFont myFont(fontData);

tft.setFont(myFont);
```
