#ifndef _AMIGA_FONT
#define _AMIGA_FONT

#include <Cariad.h>

struct Node {
    uint32_t    ln_Succ;  /* Pointer to next (successor) */
    uint32_t    ln_Pred;  /* Pointer to previous (predecessor) */
    uint8_t     ln_Type;
    int8_t      ln_Pri;     /* Priority, for sorting */
    uint32_t    ln_Name;       /* ID string, null terminated */
} __attribute__((packed));  /* Note: word aligned */

struct Message {
    struct Node mn_Node;
    uint32_t    mn_ReplyPort;  /* message reply port */
    uint16_t    mn_Length;          /* total message length, in bytes */
                    /* (include the size of the Message */
                    /* structure in the length) */
} __attribute__((packed));

struct TextFont {
    struct Message tf_Message;  /* reply message for font removal */
                /* font name in LN    \    used in this */
    uint16_t    tf_YSize;       /* font height        |    order to best */
    uint8_t     tf_Style;       /* font style         |    match a font */
    uint8_t     tf_Flags;       /* preferences and flags  /    request. */
    uint16_t    tf_XSize;       /* nominal font width */
    uint16_t    tf_Baseline;    /* distance from the top of char to baseline */
    uint16_t    tf_BoldSmear;   /* smear to affect a bold enhancement */

    uint16_t    tf_Accessors;   /* access count */

    uint8_t     tf_LoChar;      /* the first character described here */
    uint8_t     tf_HiChar;      /* the last character described here */
    uint32_t    tf_CharData;    /* the bit character data */

    uint16_t    tf_Modulo;      /* the row modulo for the strike font data */
    uint32_t    tf_CharLoc;     /* ptr to location data for the strike font */
                /*   2 words: bit offset then size */
    uint32_t    tf_CharSpace;   /* ptr to words of proportional spacing data */
    uint32_t    tf_CharKern;    /* ptr to words of kerning data */
} __attribute__((packed));

class AmigaFont : public Font {
    private:
        struct TextFont *_header;
        const char *_name;
    public:
        AmigaFont(const uint8_t *fd);

        int getStringWidth(const char *str);
        int getStringHeight(const char *str);
        uint8_t getCharacterWidth(uint8_t glyph);
        uint8_t getStartGlyph();
        uint8_t getEndGlyph();
        int drawChar(Cariad *dev, int x, int y, uint8_t c, color_t fg, color_t bg);
        const char *getName() { return _name; }

};   

#endif
