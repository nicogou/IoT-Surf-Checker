/****************************************************************************************************************************
  dynamicParams.h for SAMD_WiFiNINA.ino
  For SAMD boards using WiFiNINA Modules/Shields, using much less code to support boards with smaller memory

  WiFiManager_NINA_WM_Lite is a library for the Mega, Teensy, SAM DUE, SAMD and STM32 boards
  (https://github.com/khoih-prog/WiFiManager_NINA_Lite) to enable store Credentials in EEPROM/LittleFS for easy
  configuration/reconfiguration and autoconnect/autoreconnect of WiFi and other services without Hardcoding.

  Built by Khoi Hoang https://github.com/khoih-prog/WiFiManager_NINA_Lite
  Licensed under MIT license
 *****************************************************************************************************************************/

#ifndef dynamicParams_h
#define dynamicParams_h

#include "defines.h"

// USE_DYNAMIC_PARAMETERS defined in defined.h

/////////////// Start dynamic Credentials ///////////////

// Defined in <WiFiManager_NINA_Lite_SAMD.h>
/**************************************
  #define MAX_ID_LEN                5
  #define MAX_DISPLAY_NAME_LEN      16

  typedef struct
  {
  char id             [MAX_ID_LEN + 1];
  char displayName    [MAX_DISPLAY_NAME_LEN + 1];
  char *pdata;
  uint8_t maxlen;
  } MenuItem;
**************************************/

#if USE_DYNAMIC_PARAMETERS

#define MAX_SPOT_NAME_LEN 34
#define MAX_SPOT_ID_LEN 34

char Name_Spot1[MAX_SPOT_NAME_LEN + 1] = "Cote des Basques";
char Id_Spot1[MAX_SPOT_ID_LEN + 1] = "5842041f4e65fad6a7708bcf";

// char Name_Spot2 [MAX_SPOT_NAME_LEN + 1]  = "";
// char Id_Spot2  [MAX_SPOT_ID_LEN + 1]   = "";

// char Name_Spot3 [MAX_SPOT_NAME_LEN + 1]  = "";
// char Id_Spot3  [MAX_SPOT_ID_LEN + 1]   = "";

MenuItem myMenuItems[] =
    {
        {"sn1", "Spot 1 Name", Name_Spot1, MAX_SPOT_NAME_LEN},
        {"sid1", "Spot 1 ID", Id_Spot1, MAX_SPOT_ID_LEN},
        //{ "sn2", "Spot 2 Name", Name_Spot2,  MAX_SPOT_NAME_LEN },
        //{ "sid2", "Spot 2 ID",   Id_Spot2,   MAX_SPOT_ID_LEN },
        //{ "sn3", "Spot 3 Name", Name_Spot3,     MAX_SPOT_NAME_LEN },
        //{ "sid3", "Spot 3 ID",   Id_Spot3,    MAX_SPOT_ID_LEN },
};

uint16_t NUM_MENU_ITEMS = sizeof(myMenuItems) / sizeof(MenuItem); // MenuItemSize;

#else

MenuItem myMenuItems[] = {};

uint16_t NUM_MENU_ITEMS = 0;

#endif // USE_DYNAMIC_PARAMETERS

#endif // dynamicParams_h