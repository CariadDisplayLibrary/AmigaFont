#include <AmigaFont.h>

typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;
typedef int8_t int8_t;
typedef int16_t int16_t;
typedef int32_t int32_t;

#include <sys/endian.h>

AmigaFont::AmigaFont(const uint8_t *fd) : Font(fd) {
    uint16_t moveq = fd[0x20] << 8 | fd[0x21];
    uint16_t rts = fd[0x22] << 8 | fd[0x23];
    struct Node *node = (struct Node *)&fd[0x24];
    uint32_t name = swap32(node->ln_Name);
    _header = (struct TextFont *)&fd[0x20 + name + 32];
    _name = (const char *)&fd[0x20 + name + 1];
}


int AmigaFont::getStringWidth(const char *str) {
    int w = 0;
    while (*str) {
        w += getCharacterWidth(*str++);
    }
    return w;
}

int AmigaFont::getStringHeight(const char *str) {
    return swap16(_header->tf_YSize);
}

uint8_t AmigaFont::getCharacterWidth(uint8_t glyph) {
    if (_header->tf_Flags & 0x20) { // Proportional
        glyph -= _header->tf_LoChar;
        uint32_t spaceDataStart = 0x20 + swap32(_header->tf_CharSpace);
        int16_t space = _fontData[spaceDataStart + (glyph * 2)] << 8 | _fontData[spaceDataStart + (glyph * 2) + 1];
        uint32_t kernDataStart = 0x20 + swap32(_header->tf_CharKern);
        int16_t kern = _fontData[spaceDataStart + (glyph * 2)] << 8 | _fontData[kernDataStart + (glyph * 2) + 1];
        return space + kern;
    } else {
        return swap16(_header->tf_XSize);
    }
}

uint8_t AmigaFont::getStartGlyph() {
    return _header->tf_LoChar;
}

uint8_t AmigaFont::getEndGlyph() {
    return _header->tf_HiChar;
}

static inline uint8_t extractBit(const uint8_t *data, uint32_t offset) {
    uint8_t b = offset >> 3;
    uint8_t o = offset & 0x07;

    return (data[b] & (0x80 >> o)) ? 1 : 0;
}

int AmigaFont::drawChar(Cariad *dev, int x, int y, uint8_t c, color_t fg, color_t bg) {

    if (c < _header->tf_LoChar || c > _header->tf_HiChar) return 0;

    char s[2] = {c, 0};
    int lines = getStringHeight(s);

    c -= _header->tf_LoChar;

    uint32_t tabOff = 0x20 + swap32(_header->tf_CharLoc) + ((c) * 4);
    uint16_t bitOff = _fontData[tabOff] << 8 | _fontData[tabOff + 1];
    uint16_t bitRun = _fontData[tabOff + 2] << 8 | _fontData[tabOff + 3];

    uint16_t modulo = swap16(_header->tf_Modulo);
    uint32_t fontDataStart = 0x20 + swap32(_header->tf_CharData);

    int16_t space = swap16(_header->tf_XSize);
    int16_t kern = 0;

    if (_header->tf_Flags & 0x20) { // Proportional
        uint32_t spaceDataStart = 0x20 + swap32(_header->tf_CharSpace);
        space = _fontData[spaceDataStart + (c * 2)] << 8 | _fontData[spaceDataStart + (c * 2) + 1];
        uint32_t kernDataStart = 0x20 + swap32(_header->tf_CharKern);
        kern = _fontData[kernDataStart + (c * 2)] << 8 | _fontData[kernDataStart + (c * 2) + 1];
    }

    if (fg != bg) {
        if (kern > 0) {
            dev->fillRectangle(x, y, kern, lines, bg);
        }
        dev->openWindow(x + kern, y, bitRun, lines);
        for (int line = 0; line < lines; line++) {

            uint32_t lineOffset = fontDataStart + (modulo * line);
            for (int p = 0; p < bitRun; p++) {
                uint8_t b = extractBit(&_fontData[lineOffset], bitOff + p);
                if (b) {
                    dev->windowData(fg);
                } else {
                    dev->windowData(bg);
                }
            }
        }
        dev->closeWindow();
        if (space > bitRun) {
            dev->fillRectangle(x + kern + bitRun, y, space - bitRun, lines, bg);
        }
    } else {
        for (int line = 0; line < lines; line++) {

            uint32_t lineOffset = fontDataStart + (modulo * line);
            for (int p = 0; p < bitRun; p++) {
                uint8_t b = extractBit(&_fontData[lineOffset], bitOff + p);
                if (b) {
                    dev->setPixel(x + p + kern, y + line, fg);
                }
            }
        }
    }

    return space + kern;
}
