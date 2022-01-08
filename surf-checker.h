#ifndef Surf_Checker_h
#define Surf_Checker_h

#include <WiFiNINA_Generic.h>
#include <ArduinoJson.h>
// #define ARDUINOJSON_USE_DOUBLE 1
#include <constants.h>
#include <FastLED.h>

class Surf_Checker
{
public:
    WiFiClient client;
    String spot_id = "5842041f4e65fad6a7708bca"; // grande plage Biarritz;
    String nb_days = "1";
    uint8_t interval_hours = 3;
    char const *host_name[NB_REQUESTS] = {"worldtimeapi.org", "services.surfline.com", "services.surfline.com"};
    String path_name[NB_REQUESTS] = {"/api/", "/kbyg/spots/forecasts/", "/kbyg/spots/forecasts/"};
    String query[NB_REQUESTS] = {"ip", "wave?spotId=" + spot_id + "&days=" + nb_days + "&intervalHours=" + String(interval_hours), "wind?spotId=" + spot_id + "&days=" + nb_days + "&intervalHours=" + String(interval_hours)};
    unsigned long query_timeout = QUERY_TIMEOUT;

    long unixtime = 0;
    int status = WL_IDLE_STATUS;
    float wind_speed = 0;
    double wind_direction = 0;
    float swell_heights[SWELL_NB];
    double swell_directions[SWELL_NB];
    int swell_periods[SWELL_NB];
    bool error = false;

    CRGB leds[NUM_LEDS]; // Stores the LED colors (only status LED for now).

    Surf_Checker(char *ssid, char *pass);
    Surf_Checker();
    void print_wifi_status();
    bool connect(char *ssid, char *pass);
    // bool http_request(char const *host_name, String path_name, String query_string);
    bool http_request(HttpDataType type);
    bool parse_http_response(HttpDataType type);

    bool update_spot_id(String sid);
    void build_query();

    void get_data();
    bool get_data(HttpDataType type);
    bool get_time();
    bool get_wave();
    bool get_wind();

    void display_data();

private:
};

#endif
