// process single character commands from serial interface

#include <Arduino.h>
#include <EEPROM.h>

#include "pcRead.h"
#include "spdTrp.h"

// -----------------------------------------------------------------------------
uint16_t
eeRead16 (
    int     addr)
{
    uint16_t val;
    EEPROM.get (addr, val);

    return val;
}

// -------------------------------------
void
eeWrite16 (
    int      addr,
    uint16_t val )
{
    EEPROM.put (addr, val);
}

// -------------------------------------
void
eeDisp (void)
{
    Serial.print ("eeDisp:");
    for (int adr = 0; adr < 16; adr++)  {
        if (! (adr % 16))  {
            sprintf (s, "\n  0x%03x: ", adr);
            Serial.print (s);
        }
        sprintf (s, " %02x", EEPROM.read (adr));
        Serial.print (s);
    }

    Serial.println ();
}

// -----------------------------------------------------------------------------
void
pcRead (void)
{
    static int  val = 0;

    if (Serial.available()) {
        int c = Serial.read ();

        switch (c)  {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            val = c - '0' + (10 * val);
            break;

        case 'D':
            eeWrite16 (EE_Dist, val);
            setConversion ();
            val = 0;
            break;

        case 'd':
            sprintf (s, "%s: distanceX10 (in) %d",
                __func__, eeRead16 (EE_Dist));
            Serial.println (s);
            break;

        case 'e':
            eeDisp ();
            break;

        case 'S':
            eeWrite16 (EE_Scale, val);
            setConversion ();
            val = 0;
            break;

        case 's':
            sprintf (s, "%s: scale  %d", __func__, eeRead16 (EE_Scale));
            Serial.println (s);
            break;

        case 'v':
            Serial.println (Version);
            break;

        case '\n':
        case '\r':
            break;

        case '?':
            Serial.println ("\npcRead:\n");
            Serial.println ("    [0-9] append to #");
            Serial.println ("  # D - set distance to #");
            Serial.println ("    d - report distance");
            Serial.println ("    e - display EE contents");
            Serial.println ("  # S - set scale to #");
            Serial.println ("    s - report scale");
            Serial.println ("    v - report version");
            Serial.println ("    ? - list of commands");
            break;

        default:
            Serial.print ("unknown char ");
            Serial.println (c,HEX);
            break;
        }
    }
}
