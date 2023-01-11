// model RR speed trap

#include "seg7disp.h"

const byte PinSensor [] = { A1, A2 };
const unsigned Nsensor = sizeof(PinSensor);

unsigned long msecLst;
unsigned long msec;


char s [80];

// -----------------------------------------------------------------------------
//     1 mph       = 17.6 in/sec   12 * 5280 / 3600
//     1 scale-mph = 0.2023 in/sec
//     trap speed mph = scale-mph * (1000 * msec) / trap-distance-inches
//       scale-mph =  (trap-dist * 1000 / 0.2023) / msec

float mph_msec = 1.0;

void
setConversion (
    float   trapDistInches )
{
    mph_msec = 1000 * trapDistInches / ((12.0 * 5280 / 3600) / 87);
}

// ---------------------------------------------------------
enum { None = -1, Disp = -2 };
int pinTrap = None;

// -------------------------------------
void
trap (void)
{
    if (Disp == pinTrap)  {
        if (msec - msecLst > 2000)  {
            seg7off ();
            pinTrap = None;
            Serial.println (" rdy");
        }
    }
    else if (None == pinTrap)  {
        for (unsigned n = 0; n < Nsensor; n++)  {
            if (LOW == digitalRead (PinSensor [n]))  {
                msecLst = msec;
                pinTrap = Nsensor - n - 1;
                break;
            }
        }
    }
    else if (LOW == digitalRead (PinSensor [pinTrap]))  {
        unsigned long msecTime = msec - msecLst;
        float         mphX10   = 10 * mph_msec / msecTime;
        seg7disp ((int)mphX10);

        msecLst = msec;
        pinTrap = Disp;

        char t [20];
        dtostrf (mphX10, 6, 2, t);
        sprintf (s, " %8ld %8ld %s", msecLst, msec, t);
        Serial.println (s);
    }
}

// ---------------------------------------------------------
void
loop (void)
{
    msec = millis ();
    trap ();
}

// ---------------------------------------------------------
void
setup (void)
{
    Serial.begin (9600);
    seg7init ();

    setConversion (10.0);

    for (unsigned n = 0; n < Nsensor; n++)
        pinMode (PinSensor [n], INPUT_PULLUP);
}
