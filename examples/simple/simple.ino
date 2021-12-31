#include "surf-checker.h"
#include "secrets.h"

Surf_Checker *s_f;
char ssid[] = WIFI_SSID; // your network SSID (name)
char pass[] = WIFI_PASS; // your network password (use for WPA, or use as key for WEP)

void setup()
{
    s_f = new Surf_Checker(ssid, pass);
}

void loop()
{
}