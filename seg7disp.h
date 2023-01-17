const byte SEGMENT_OFF = 0xFF;
const byte SEGMENT_DEC = 0x7F;

void seg7disp (int  val, int flip);
void seg7init (void);
void seg7off  (void);
void seg7on   (void);
void seg7segs (int  val, byte segs);
