// drive 7-seg displays on MultiFuction boardj

#include <TimerOne.h>

#include "seg7disp.h"
#include "spdTrp.h"

// -------------------------------------
//  constants for multiplexed 7-segment display
#define Latch   4
#define Clock   7
#define Data    8
#define LED0   10
#define LED1   11

byte pins [] = { Latch, Clock, Data, LED0, LED1 };

//    a      d
//   f b    c e
//    g      g
//   e c    b f
//    d h    a
//
//   h g f e d c b a

const byte SEGMENT_MAP_DIGIT[] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0X80, 0X90
};

const byte SEGMENT_MAP_DIGIT_F [] = {
    0xC0, 0xCF, 0xA4, 0x86, 0x8B, 0x92, 0x90, 0xC7, 0x80, 0x82
};

const byte DISP_MAP [] = { 1, 2, 4, 8 };
#define N_DISP  sizeof(DISP_MAP)

byte disp [N_DISP] = {SEGMENT_OFF, SEGMENT_OFF, SEGMENT_OFF, SEGMENT_OFF};

int flag;

// -----------------------------------------------------------------------------
// shift 16-bits from data into the shift register
void output (
    uint16_t  data)
{
    digitalWrite (Latch, LOW);

    for (unsigned i = 0; i < 16; i++, data <<= 1)  {
        digitalWrite (Data, 0 != (data & 0x8000));

        digitalWrite (Clock, HIGH);
        digitalWrite (Clock, LOW);
    }

    digitalWrite (Latch, HIGH);
}

// -----------------------------------------------------------------------------
// repeatedly display one digit
//      lower 8-bits selects the digit
//      upper 8-bits species the segments
void isr (void)
{
    static byte idx = 0;

    uint16_t val = (disp [idx] << 8) + DISP_MAP [idx];
    output (val);

    if (N_DISP <= ++idx)
        idx = 0;
}

// -----------------------------------------------------------------------------
// update the value of each digit
void seg7segs (
    int  val,
    byte segs )
{
    for (int i = N_DISP-1; i >= 0; i--, val /= 2)
        disp [i] = val & 1 ? segs : SEGMENT_OFF;
}

// -----------------------------------------------------------------------------
// update the value of each digit
void seg7disp (
    int  valX10,
    int  flip )
{
    Serial.println (__func__);

    int i;
    if (flip)  {
        for (i = 0; i < (int)N_DISP; i++, valX10 /= 10)
            disp [i] = SEGMENT_MAP_DIGIT_F [valX10 % 10];

        // blank leading zeros
        i = N_DISP-1;
        while (SEGMENT_MAP_DIGIT [0] == disp [i])
            disp [i--] = SEGMENT_OFF;
    }
    else  {
        for (i = N_DISP-1; i >= 0; i--, valX10 /= 10)
            disp [i] = SEGMENT_MAP_DIGIT [valX10 % 10];
        disp [N_DISP-2] &= SEGMENT_DEC;       // decimal pt

        // blank leading zeros
        i = 0;
        while (SEGMENT_MAP_DIGIT [0] == disp [i])
            disp [i++] = SEGMENT_OFF;
    }
}

// -----------------------------------------------------------------------------
// update the value of each digit
void seg7off (void)
{
    Serial.println (__func__);
    for (int i = N_DISP-1; i >= 0; i--)
        disp [i] = SEGMENT_OFF;
}

void seg7on (void)
{
    Serial.println (__func__);
    for (int i = N_DISP-1; i >= 0; i--)
        disp [i] = 0;
}

// -----------------------------------------------------------------------------
void seg7init (void)
{
    Serial.println (__func__);
    for (unsigned i = 0; i < sizeof(pins); i++)  {
        digitalWrite (pins [i], HIGH);
        pinMode      (pins [i], OUTPUT);
    }

    Timer1.initialize(5000);
    Timer1.attachInterrupt (isr); // blinkLED to run every 0.15 seconds
}

