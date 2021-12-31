#ifndef Surf_Checker_h
#define Surf_Checker_h

#include <WiFiNINA.h>
#include <ArduinoJson.h>
// #define ARDUINOJSON_USE_DOUBLE 1

class Surf_Checker
{
public:
    WiFiClient client;
    String spot_id = "5842041f4e65fad6a7708bca"; // grande plage Biarritz;
    String nb_days = "1";
    String interval_hours = "3";
    String HOST_NAME[3] = {"worldtimeapi.org", "services.surfline.com", "services.surfline.com"};
    String PATH_NAME[3] = {"/api/", "/kbyg/spots/forecasts/", "/kbyg/spots/forecasts/"};
    String QUERY[3] = {"ip", "wave?spotId=" + spot_id + "&days=" + nb_days + "&intervalHours=" + interval_hours, "wind?spotId=" + spot_id + "&days=" + nb_days + "&intervalHours=" + interval_hours};

    long unixtime = 0;
    int status = WL_IDLE_STATUS;

    Surf_Checker(char *ssid, char *pass);
    void print_wifi_status();
    bool connect(char *ssid, char *pass);
    bool http_request(char *host_name, String path_name, String query_string);
    bool parse_http_response();

private:
};

#endif
