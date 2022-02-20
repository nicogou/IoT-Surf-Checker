/*********************************************************************************************************************************
  SAMD_WiFiNINA.ino
  For SAMD boards using WiFiNINA Modules/Shields, using much less code to support boards with smaller memory

  WiFiManager_NINA_WM_Lite is a library for the Mega, Teensy, SAM DUE, SAMD and STM32 boards
  (https://github.com/khoih-prog/WiFiManager_NINA_Lite) to enable store Credentials in EEPROM/LittleFS for easy
  configuration/reconfiguration and autoconnect/autoreconnect of WiFi and other services without Hardcoding.

  Built by Khoi Hoang https://github.com/khoih-prog/WiFiManager_NINA_Lite
  Licensed under MIT license
  **********************************************************************************************************************************/

#include "defines.h"
#include "Credentials_secret.h"
#include "dynamicParams.h"
#include "surf-checker.h"

Surf_Checker *s_f;

void heartBeatPrint(void)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        s_f->get_data();
        s_f->display_data();
    }
    else
        Serial.println("Not connected to WiFi");
}

void check_status()
{
    static unsigned long checkstatus_timeout = 0;

    // KH
#define HEARTBEAT_INTERVAL 20000L
    // Print hearbeat every HEARTBEAT_INTERVAL (20) seconds.
    if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0))
    {
        heartBeatPrint();
        checkstatus_timeout = millis() + HEARTBEAT_INTERVAL;
    }
}

WiFiManager_NINA_Lite *WiFiManager_NINA;

#if USING_CUSTOMS_STYLE
const char NewCustomsStyle[] /*PROGMEM*/ = "<style>div,input{padding:5px;font-size:1em;}input{width:95%;}body{text-align: center;}\
button{background-color:blue;color:white;line-height:2.4rem;font-size:1.2rem;width:100%;}fieldset{border-radius:0.3rem;margin:0px;}</style>";
#endif

void setup()
{
    s_f = new Surf_Checker();

    delay(200);

    Serial.print(F("\nStarting SAMD_WiFiNINA on "));
    Serial.println(BOARD_TYPE);
    Serial.println(WIFIMANAGER_NINA_LITE_VERSION);
    WiFiManager_NINA = new WiFiManager_NINA_Lite();

    // Optional to change default AP IP(192.168.4.1) and channel(10)
    // WiFiManager_NINA->setConfigPortalIP(IPAddress(192, 168, 120, 1));
    WiFiManager_NINA->setConfigPortalChannel(0);

#if USING_CUSTOMS_STYLE
    WiFiManager_NINA->setCustomsStyle(NewCustomsStyle);
#endif

#if USING_CUSTOMS_HEAD_ELEMENT
    WiFiManager_NINA->setCustomsHeadElement("<style>html{filter: invert(10%);}</style>");
#endif

#if USING_CORS_FEATURE
    WiFiManager_NINA->setCORSHeader("Your Access-Control-Allow-Origin");
#endif

    s_f->lightup_led_setup(true);
    // Set customized DHCP HostName
    WiFiManager_NINA->begin(HOST_NAME);
    // Or use default Hostname "SAMD-WiFiNINA-XXXXXX"
    // WiFiManager_NINA->begin();
}

#if USE_DYNAMIC_PARAMETERS
void displayCredentials()
{
    Serial.println(F("\nYour stored Credentials :"));

    for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
    {
        Serial.print(myMenuItems[i].displayName);
        Serial.print(F(" = "));
        Serial.println(myMenuItems[i].pdata);
    }
}

void displayCredentialsInLoop()
{
    static bool displayedCredentials = false;

    if (!displayedCredentials)
    {
        for (int i = 0; i < NUM_MENU_ITEMS; i++)
        {
            if (!strlen(myMenuItems[i].pdata))
            {
                break;
            }

            if (i == (NUM_MENU_ITEMS - 1))
            {
                displayedCredentials = true;
                displayCredentials();
            }
        }
    }
}

#endif

void loop()
{
    WiFiManager_NINA->run();
#if USE_DYNAMIC_PARAMETERS
    displayCredentialsInLoop();
    s_f->update_spot_id(myMenuItems[1].pdata);
#endif
    if (WiFiManager_NINA->configuration_mode)
    {
        s_f->lightup_led_config_portal(true);
    }

    check_status();
    s_f->lightup_leds();
}