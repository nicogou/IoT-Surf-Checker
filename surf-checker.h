#ifndef Surf_Checker_h
#define Surf_Checker_h

#include <WiFiNINA_Generic.h>
#include <ArduinoJson.h>
// #define ARDUINOJSON_USE_DOUBLE 1
#include <constants.h>

class Surf_Checker
{
public:
    WiFiClient client;
    String spot_id = "5842041f4e65fad6a7708bca"; // grande plage Biarritz;
    String nb_days = "1";
    uint8_t interval_hours = 3;
    char const *host_name[3] = {"worldtimeapi.org", "services.surfline.com", "services.surfline.com"};
    String path_name[3] = {"/api/", "/kbyg/spots/forecasts/", "/kbyg/spots/forecasts/"};
    String query[3] = {"ip", "wave?spotId=" + spot_id + "&days=" + nb_days + "&intervalHours=" + String(interval_hours), "wind?spotId=" + spot_id + "&days=" + nb_days + "&intervalHours=" + String(interval_hours)};
    unsigned long query_timeout = 2000;

    long unixtime = 0;
    int status = WL_IDLE_STATUS;
    float wind_speed = 0;
    double wind_direction = 0;
    float swell_heights[6];
    double swell_directions[6];
    int swell_periods[6];

    Surf_Checker(char *ssid, char *pass);
    Surf_Checker();
    void print_wifi_status();
    bool connect(char *ssid, char *pass);
    // bool http_request(char const *host_name, String path_name, String query_string);
    bool http_request(HttpDataType type);
    bool parse_http_response(HttpDataType type);

    bool update_spot_id(String sid);
    void build_query();

    void get_data(HttpDataType type);
    void get_time();
    void get_wave();
    void get_wind();

private:
};

#endif
