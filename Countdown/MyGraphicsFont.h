#include <inttypes.h>
#ifdef __AVR__
#include <avr/pgmspace.h>
#elif defined (ESP8266)
#include <pgmspace.h>
#else
#define PROGMEM
#endif

#ifndef MYGRAPHICSFONT_H
#define MYGGRAPHICSFONT_H

static const uint8_t  MyGraphicsFont[] PROGMEM = { // the name here 'MyGiantFont' must be used to refer to this font
                                            // 'PROGMEM' forces the data into programme space to save RAM
    0x00, 0x00, // size     0 here indicates a fixed width font
    0x20, // width          these are 32 pixels wide (a full DMD)
    0x10, // height         max height of the characters = 16 (a full DMD)
    0x30, // first char     hex value of the first character ('1')
    0x03, // char count     the number of characters in this font

    // font data
    // only 2 characters defined - these are accessed by '1' (ascii 49/0X31) and '2' (ascii 50/0X32)

// first char is a '1' as a sample of the use of BIG characters on a vertical DMD
0X00, 0XF8, 0XFC, 0XFE, 0X0E, 0X0E, 0X0E, 0X0E, 0X0E, 0X0E, 0X0E, 0X0E, 0X0E, 0X0E, 0X0E, 0X0E, 0X0E, 0X0E, 0X0E, 0X0E, 0X0E, 0X0E, 0X0E, 0X0E,0X0E,0X0E,0X0E,0X0E,0XFE,0XFC,0XF8,0X00,0X00, 0X1F, 0X3F, 0X7F, 0X70, 0X70, 0X70, 0X70, 0X70, 0X70, 0X70, 0X70, 0X70, 0X70, 0X70, 0X70, 0X70, 0X70, 0X70, 0X70, 0X70, 0X70, 0X70, 0X70,0X70,0X70,0X70,0X70,0X7F,0X3F,0X1F,0X00,

// first char is a '1' as a sample of the use of BIG characters on a vertical DMD
0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XC0, 0XF0, 0XF0, 0XF0, 0X01, 0X03, 0X07, 0X0F, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X0F, 0X0F, 0X0F, // note the comma at the end

// second one is a simple graphic screen
0XFF, 0X01, 0X01, 0X31, 0X31, 0X01, 0X01, 0X31, 0X31, 0X01, 0X01, 0X01, 0X01, 0X31, 0X31, 0X01, 0X01, 0X31, 0X31, 0X01, 0X01, 0X01, 0X01, 0X31, 0X31, 0X01, 0X01, 0X31, 0X31, 0X01, 0X01, 0XFF, 0XFF, 0X80, 0X82, 0X84, 0X88, 0X88, 0X88, 0X88, 0X84, 0X82, 0X80, 0X80, 0X84, 0X84, 0X84, 0X84, 0X84, 0X84, 0X84, 0X84, 0X80, 0X80, 0X88, 0X84, 0X82, 0X82, 0X82, 0X82, 0X84, 0X88, 0X80, 0XFF // note NO comma on the last entry

};

#endif
