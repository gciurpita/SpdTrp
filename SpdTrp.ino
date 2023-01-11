const char *Version = "Model RR speed trap - 230111c";

#include "pcRead.h"
#include "seg7disp.h"
#include "spdTrp.h"

const byte PinSensor [] = { A1, A2 };
const unsigned Nsensor = sizeof(PinSensor);

const unsigned long WaitPeriod = 3000;
unsigned long msecLst;
unsigned long msec;
unsigned long msec0;

int   scale       = 87;
float distX10Inch = 100;

char s [SizeS];
char t [20];

// -----------------------------------------------------------------------------
//     1 mph       = 17.6 in/sec   12 * 5280 / 3600
//     1 scale-mph = 0.2023 in/sec
//     trap speed mph = scale-mph * (1000 * msec) / trap-distance-inches
//       scale-mph =  (trap-dist * 1000 / 0.2023) / msec

float mph_msec = 1.0;

void
setConversion (void)
{
    scale       = eeRead16 (EE_Scale);
    distX10Inch = eeRead16 (EE_Dist);

    mph_msec = 1000 * (distX10Inch /10.0) / ((12.0 * 5280 / 3600) / scale);

    dtostrf (mph_msec, 6, 2, t);
    sprintf (s, "%s: %d in, %d scale, %s mph_msec",
        __func__, (int)distX10Inch, scale, t);
    Serial.println (s);
}

// -------------------------------------
void
dispSpd (
    unsigned long msecTime )
{
    float         mphX10   = 10 * mph_msec / msecTime;
    seg7disp ((int)mphX10);

    dtostrf (mphX10 / 10.0, 6, 2, t);
    sprintf (s, " %8ld msec, %s mph", msecTime, t);
    Serial.println (s);
}

// ---------------------------------------------------------
enum { Idle, Trig, Fouled, Wait };
int state = Idle;

unsigned pinTrap;

// -------------------------------------
void
trap (void)
{
    // check inputs, return Nsensor if none active
    unsigned  pin;
    for (pin = 0; pin < Nsensor; pin++)  {
        if (LOW == digitalRead (PinSensor [pin]))  {
            break;
        }
    }

    // ---------------------------
    msec = millis ();

    switch (state) {
    case Idle:                  // check for trains entering trap
        if (Nsensor > pin)  {
            pinTrap = Nsensor - 1 - pin;
            msecLst = msec;
            state   = Trig;
        }
        break;

    case Trig:                  // check for train exiting
        if (pinTrap == pin)  {
            dispSpd (msec - msecLst);
            state = Wait;
        }
        break;

    case Wait:
        if (Nsensor != pin)     // check for cars crossing trap
            msecLst    = msec;
        else if (msec - msecLst >= WaitPeriod)  {   // timer
            state      = Idle;
            seg7off ();
        }
    }
}

// ---------------------------------------------------------
void
loop (void)
{
    trap ();
    pcRead ();
}

// ---------------------------------------------------------
void
setup (void)
{
    Serial.begin (9600);
    Serial.println (Version);

    seg7init ();
    setConversion ();

    for (unsigned n = 0; n < Nsensor; n++)
        pinMode (PinSensor [n], INPUT_PULLUP);
}
