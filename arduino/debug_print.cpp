#include "debug_print.h"

// Debug On / Off
//#define USER_DEBUG

/*********************************************************************************
  Debug print
*********************************************************************************/
void debugPrint(String text) {
#ifdef USER_DEBUG
  text = "[DBG]" + text;
  Serial.println(text);
#endif
}