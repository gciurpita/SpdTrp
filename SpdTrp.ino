// model RR speed trap

#include "seg7disp.h"

const byte PinSensor [] = { A1, A2 };
const unsigned Nsensor = sizeof(PinSensor);

unsigned long msecLst;
unsigned long msec;


char s [80];
char t [20];

// -----------------------------------------------------------------------------
//     1 mph       = 17.6 in/sec   12 * 5280 / 3600
//     1 scale-mph = 0.2023 in/sec
//     trap speed mph = scale-mph * (1000 * msec) / trap-distance-inches
//       scale-mph =  (trap-dist * 1000 / 0.2023) / msec

float mph_msec = 1.0;

void
setConversion (
    float   trapDistInch,
    int     scale )
{
    mph_msec = 1000 * trapDistInch / ((12.0 * 5280 / 3600) / scale);

    dtostrf (mph_msec, 6, 2, t);
    sprintf (s, "%s: %d in, %d scale, %s mph_msec",
        __func__, (int)trapDistInch, scale, t);
    Serial.println (s);
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
            Serial.print (" rdy ");
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

        dtostrf (mphX10, 6, 2, t);
        sprintf (s, " %8ld msec, %s mph", msecTime, t);
        Serial.println (s);

        msecLst = msec;
        pinTrap = Disp;
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

    setConversion (10.0, 87);

    for (unsigned n = 0; n < Nsensor; n++)
        pinMode (PinSensor [n], INPUT_PULLUP);
}
