// drive 7-seg displays on MultiFuction boardj

#include <TimerOne.h>

// -------------------------------------
//  constants for multiplexed 7-segment display
#define Latch   4
#define Clock   7
#define Data    8
#define LED0   10
#define LED1   11

byte pins [] = { Latch, Clock, Data, LED0, LED1 };

const byte SEGMENT_MAP_DIGIT[] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 
    0x92, 0x82, 0xF8, 0X80, 0X90
};
const byte SEGMENT_OFF = 0xFF;
const byte SEGMENT_DEC = 0x80;

const byte DISP_MAP [] = { 1, 2, 4, 8 };
#define N_DISP  sizeof(DISP_MAP)

byte disp [N_DISP] = {SEGMENT_OFF, SEGMENT_OFF, SEGMENT_OFF, SEGMENT_OFF};

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
void seg7disp (
    int  valX10 )
{
    for (int i = N_DISP-1; i >= 0; i--, valX10 /= 10)
        disp [i] = SEGMENT_MAP_DIGIT [valX10 % 10];
}

// -----------------------------------------------------------------------------
// update the value of each digit
void seg7off (void)
{
    for (int i = N_DISP-1; i >= 0; i--)
        disp [i] = SEGMENT_OFF;
}

// -----------------------------------------------------------------------------
void seg7init (void)
{
    for (unsigned i = 0; i < sizeof(pins); i++)  {
        digitalWrite (pins [i], HIGH);
        pinMode      (pins [i], OUTPUT);
    }

    Timer1.initialize(5000);
    Timer1.attachInterrupt (isr); // blinkLED to run every 0.15 seconds
}

