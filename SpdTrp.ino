const char *Version = "Model RR speed trap - 230123a";

#include "pcRead.h"
#include "seg7disp.h"
#include "spdTrp.h"

#if 0
const byte PinSensor [] = { 5, 9 };
#else
const byte PinSensor [] = { A1, A2 };
#endif
const unsigned Nsensor = sizeof(PinSensor);

const unsigned long WaitPeriod = 3000;
const unsigned long TrigPeriod = 250;
unsigned long msecLst;
unsigned long msec;
unsigned long msec0;

int   scale       = 87;
float distX10Inch = 100;

int   testMode = 1;

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
    seg7disp ((int)mphX10, 1);

    dtostrf (mphX10 / 10.0, 6, 2, t);
    sprintf (s, " %8ld msec, %s mph", msecTime, t);
    Serial.println (s);
}

// ---------------------------------------------------------
enum { Idle, Trig, Fouled, Wait };
int state = Idle;

unsigned cntTrig;
unsigned pinExit;
byte     sensorSt [Nsensor];

// -------------------------------------
void
trap (void)
{
    static byte dots;

    // check inputs, return Nsensor if none active
    unsigned  pin;
    for (pin = 0; pin < Nsensor; pin++)
        sensorSt [pin] = digitalRead (PinSensor [pin]);

    // ---------------------------
    switch (state) {
    case Idle:                  // check for trains entering trap
        if ((! sensorSt [0]) || (! sensorSt [1]))  {
            pinExit = ! sensorSt [0] ? 1 : 0;
            msec0   = msecLst = msec;
            state   = Trig;
            cntTrig = 4 * 30;           // 4 * # seconds
            dots    = 2;
            Serial.println (" Trig");
        }
        break;

    case Trig:                  // check for train exiting
        if (! sensorSt [pinExit])  {
            dispSpd (msec - msec0);
            state = Wait;
            Serial.println (" Wait");
        }
        else if (msec - msecLst >= TrigPeriod)  {   // timer
            msecLst = msec;

            if (pinExit)
                dots = 1 == dots ? 8 : dots / 2;
            else
                dots = 8 == dots ? 1 : dots * 2;
            seg7segs (dots, SEGMENT_DEC);

            if (! --cntTrig)  {
                seg7off ();
                state = Idle;
                Serial.println (" Idle - timeout");
            }
        }
        break;

    case Wait:
        if ((! sensorSt [0]) || (! sensorSt [1]))
            msecLst    = msec;
        else if (msec - msecLst >= WaitPeriod)  {   // timer
            state      = Idle;
            seg7off ();
            Serial.println (" Idle");
        }
    }
}

// ---------------------------------------------------------
int
checkMode (void)
{
    int digs = 6;
    digs |= (LOW == digitalRead (PinSensor [0])) ? 8 : 0;
    digs |= (LOW == digitalRead (PinSensor [1])) ? 1 : 0;
    seg7segs (digs, 0xbf);

    return 6 != digs;
}

// ---------------------------------------------------------
void
loop (void)
{
    msec = millis ();

    if (testMode)  {
        if (checkMode ())
            msecLst = msec;
        if (msec - msecLst >= 10000)  {
            testMode = 0;
            seg7off ();
        }
    }
    else
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

#if 0
    for (int i = 1; i <= 0x80; i*=2)  {
        Serial.println (i);
        seg7segs (3, ~i);
        delay (250);
        seg7off ();
        delay (250);
    }
#endif
    seg7segs (0xF, 0);
    delay (2000);
    msecLst = millis ();
}
