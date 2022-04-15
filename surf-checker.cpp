#include "surf-checker.h"

Surf_Checker::Surf_Checker(char *ssid, char *pass)
{
    Serial.begin(9600);
    unsigned long debug_counter = millis();
    while (!Serial)
    {
        // wait for serial port to connect. Needed for native USB port only. If not connected, starts after DEBUG_TIMEOUT ms.
        if (millis() - debug_counter >= DEBUG_TIMEOUT)
        {
            debug = false;
            break;
        }
    }
    if (connect(ssid, pass))
    {
    }
    else
    {
        println("Connection failed, stopping here");
        while (true)
        {
        }
    }
}

Surf_Checker::Surf_Checker()
{
    Serial.begin(9600);
    unsigned long debug_counter = millis();
    while (!Serial)
    {
        // wait for serial port to connect. Needed for native USB port only. If not connected, starts after DEBUG_TIMEOUT ms.
        if (millis() - debug_counter >= DEBUG_TIMEOUT)
        {
            debug = false;
            break;
        }
    }

    FastLED.addLeds<LED_TYPE, LED_PIN_CLOCK, COLOR_ORDER>(leds_clock, NUM_LEDS_CLOCK).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, LED_PIN_SIDES, COLOR_ORDER>(leds_sides, NUM_LEDS_SIDES).setCorrection(TypicalLEDStrip);
    for (int ii = 0; ii < NUM_LEDS_CLOCK; ii++)
    {
        leds_clock[ii] = CRGB::Black;
    }
    for (int ii = 0; ii < NUM_LEDS_SIDES; ii++)
    {
        leds_sides[ii] = CRGB::Black;
    }
    FastLED.setBrightness(BRIGHTNESS);
}

void Surf_Checker::lightup_led_setup(bool y_or_n)
{
    if (y_or_n)
    {
        for (int ii = 0; ii < NUM_DIRECTIONS; ii++)
        {
            leds_clock[2 * ii] = CHSV(ii * 255 / NUM_DIRECTIONS, 255, 255);
            leds_clock[2 * ii + 1] = leds_clock[2 * ii];
        }
        FastLED.show();
        time_since_last_show = millis();
        Serial.println("Lighting up setup LEDs");
    }
    else
    {
        for (int ii = 0; ii < NUM_LEDS_CLOCK; ii++)
        {
            leds_clock[ii] = CRGB::Black;
        }
        FastLED.show();
        time_since_last_show = millis();
        Serial.println("Shutting off setup LEDs");
    }
}

void Surf_Checker::lightup_led_config_portal(bool y_or_n)
{
    if (y_or_n)
    {

        if (millis() - time_since_last_show > 1000 / FRAMES_PER_SECOND)
        {
            fadeToBlackBy(leds_clock, NUM_LEDS_CLOCK, 20);
            int pos = beatsin16(13, 0, NUM_DIRECTIONS);
            leds_clock[(2 * pos) % NUM_LEDS_CLOCK] += CHSV(pos * 255 / NUM_DIRECTIONS, 255, 192);
            leds_clock[(NUM_LEDS_CLOCK + 1 - (2 * pos)) % NUM_LEDS_CLOCK] += CHSV(pos * 255 / NUM_DIRECTIONS, 255, 192);
        }
    }
    else
    {
        for (int ii = 0; ii < NUM_LEDS_CLOCK; ii++)
        {
            leds_clock[ii] = CRGB::Black;
        }
    }
}

void Surf_Checker::lightup_leds()
{
    if (millis() - time_since_last_show > 1000 / FRAMES_PER_SECOND)
    {
        FastLED.show();
        time_since_last_show = millis();
    }
}

void Surf_Checker::print(String s)
{
    if (debug)
    {
        Serial.print(s);
    }
}

void Surf_Checker::println(String s)
{
    if (debug)
    {
        Serial.println(s);
    }
}

void Surf_Checker::println()
{
    if (debug)
    {
        Serial.println();
    }
}

bool Surf_Checker::update_spot_id(String sid)
{
    if (spot_id != sid && sid != "blank")
    {
        println("Previous Spot Id : " + spot_id + "\t\t New Spot Id : " + sid);
        spot_id = sid;
    }
}

void Surf_Checker::build_query()
{
    query[0] = "ip";
    query[1] = "wave?spotId=" + spot_id + "&days=" + nb_days + "&intervalHours=" + String(interval_hours);
    query[2] = "wind?spotId=" + spot_id + "&days=" + nb_days + "&intervalHours=" + String(interval_hours);
    query[3] = "tides?spotId=" + spot_id + "&days=" + nb_days + "&intervalHours=" + String(interval_hours);
    query[4] = "weather?spotId=" + spot_id + "&days=" + nb_days + "&intervalHours=" + String(interval_hours);
}

bool Surf_Checker::connect(char *ssid, char *pass)
{
    // check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE)
    {
        println("Communication with WiFi module failed!");
        // don't continue
        return false;
    }

    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION)
    {
        println("Please upgrade the firmware");
    }

    // attempt to connect to WiFi network:
    while (status != WL_CONNECTED)
    {
        print("Attempting to connect to SSID: ");
        println(ssid);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, pass);

        // wait 10 seconds for connection:
        delay(10000);
    }
    // you're connected now, so print out the status:
    print_wifi_status();
    return true;
}

void Surf_Checker::print_wifi_status()
{
    // print the SSID of the network you're attached to:
    print("SSID: ");
    println(WiFi.SSID());

    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    println("IP Address: " + String(ip));
    // Serial.print("IP Address: ");
    // Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    println("signal strength (RSSI):" + String(rssi) + " dBm");
    // Serial.print("signal strength (RSSI):");
    // Serial.print(rssi);
    // Serial.println(" dBm");
}

bool Surf_Checker::http_request(HttpDataType type)
{
    build_query();
    int type_int = (int)type;
    // if there's a successful connection:
    if (client.connect(host_name[type_int], 80))
    {
        println("connecting to " + String(host_name[type_int]) + "...");
        // send the HTTP GET request:
        client.println("GET " + path_name[type_int] + query[type_int] + " HTTP/1.1");
        client.println("Host: " + String(host_name[type_int]));
        client.println("User-Agent: ArduinoWiFi/1.1");
        client.println("Connection: close");
        client.println();
    }
    else
    {
        // if you couldn't make a connection:
        println("connection failed");
        return false;
    }

    // Check HTTP status
    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));
    // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
    if (strcmp(status + 9, "200 OK") != 0)
    {
        print(F("Unexpected response: "));
        println(status);
        client.stop();
        return false;
    }

    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders))
    {
        println(F("Invalid response"));
        client.stop();
        return false;
    }

    parse_http_response(type);

    // Disconnect
    client.stop();
    return true;
}

bool Surf_Checker::parse_http_response(HttpDataType type)
{
    if (type == TIME)
    {
        StaticJsonDocument<16> filter;
        filter["unixtime"] = true;

        // Allocate the JSON document
        // Use https://arduinojson.org/v6/assistant to compute the capacity.
        const size_t capacity = 48;
        DynamicJsonDocument doc(capacity);

        // Parse JSON object
        DeserializationError error = deserializeJson(doc, client, DeserializationOption::Filter(filter));
        if (error)
        {
            print(F("deserializeJson() failed: "));
            println(error.f_str());
            client.stop();
            return false;
        }

        // Extract values
        unixtime = doc["unixtime"];
        return true;
    }
    else if (type == WAVE)
    {
        // Allocate the JSON document
        // Use https://arduinojson.org/v6/assistant to compute the capacity.
        const size_t capacity = 8192;
        DynamicJsonDocument doc(capacity);

        // Parse JSON object
        // There are four chars before the JSON object. They may correspond to the byte order mark (see https://arduinojson.org/v6/api/misc/deserializationerror/ for details). This loop gets rid of them and enables the correct reading of the JSON.
        for (int ii = 0; ii < 4; ii++)
        {
            client.read();
        }
        DeserializationError error = deserializeJson(doc, client);
        if (error)
        {
            print(F("deserializeJson() failed: "));
            println(error.f_str());
            client.stop();
            return false;
        }
        // serializeJson(doc, Serial);

        // Extract values
        for (JsonObject data_wave_item : doc["data"]["wave"].as<JsonArray>())
        {

            long data_wave_item_timestamp = data_wave_item["timestamp"]; // 1640905200, 1640916000, 1640926800, ...

            if (unixtime - data_wave_item_timestamp >= 0 && unixtime - data_wave_item_timestamp < interval_hours * HOURS_TO_SECONDS)
            {

                // int data_wave_item_utcOffset = data_wave_item["utcOffset"]; // 1, 1, 1, 1, 1, 1, 1, 1

                // JsonObject data_wave_item_surf = data_wave_item["surf"];
                // float data_wave_item_surf_min = data_wave_item_surf["min"];                 // 1.31, 1.32, 1.02, 0.94, 0.94, 0.89, 1, ...
                // float data_wave_item_surf_max = data_wave_item_surf["max"];                 // 1.62, 1.46, 1.32, 1.25, 1.24, 1.19, 1.11, ...
                // int data_wave_item_surf_optimalScore = data_wave_item_surf["optimalScore"]; // 2, 2, 2, 2, 2, 2, 2, 2
                int ii_wave = 0;
                for (JsonObject data_wave_item_swell : data_wave_item["swells"].as<JsonArray>())
                {
                    float data_wave_item_swell_height = data_wave_item_swell["height"]; // 2.0905, 0.47705, 0, 0, 0, 0
                    swell_heights[ii_wave] = data_wave_item_swell_height;

                    int data_wave_item_swell_period = data_wave_item_swell["period"]; // 13, 4, 0, 0, 0, 0
                    swell_periods[ii_wave] = data_wave_item_swell_period;

                    double data_wave_item_swell_direction = data_wave_item_swell["direction"]; // 281.18524, 87.36938, 0, 0, ...
                    swell_directions[ii_wave] = data_wave_item_swell_direction;

                    // double data_wave_item_swell_directionMin = data_wave_item_swell["directionMin"]; // 275.33166500000004, ...
                    // int data_wave_item_swell_optimalScore = data_wave_item_swell["optimalScore"];    // 0, 0, 0, 0, 0, 0
                    ii_wave++;
                }
            }
        }
        // println("Swell");
        // println("Heights\tPeriods\tDirections");
        // for (int ii = 0; ii < SWELL_NB; ii++)
        // {
        //     print(swell_heights[ii]);
        //     print("\t");
        //     print(swell_periods[ii]);
        //     print("\t\t");
        //     print(swell_directions[ii]);
        //     println();
        // }

        float temp_h;
        double temp_d;
        int temp_p;

        // sorting - Descending ORDER
        for (int ii = 0; ii < SWELL_NB; ii++)
        {
            for (int jj = ii + 1; jj < SWELL_NB; jj++)
            {
                if (swell_heights[ii] < swell_heights[jj])
                {
                    temp_h = swell_heights[ii];
                    swell_heights[ii] = swell_heights[jj];
                    swell_heights[jj] = temp_h;

                    temp_d = swell_directions[ii];
                    swell_directions[ii] = swell_directions[jj];
                    swell_directions[jj] = temp_d;

                    temp_p = swell_periods[ii];
                    swell_periods[ii] = swell_periods[jj];
                    swell_periods[jj] = temp_p;
                }
            }
        }

        return true;
    }
    else if (type == WIND)
    {
        // Allocate the JSON document
        // Use https://arduinojson.org/v6/assistant to compute the capacity.
        const size_t capacity = 1536;
        DynamicJsonDocument doc(capacity);

        // Parse JSON object
        // There are four chars before the JSON object. They may correspond to the byte order mark (see https://arduinojson.org/v6/api/misc/deserializationerror/ for details). This loop gets rid of them and enables the correct reading of the JSON.
        for (int ii = 0; ii < 4; ii++)
        {
            client.read();
        }

        DeserializationError error = deserializeJson(doc, client);
        if (error)
        {
            print(F("deserializeJson() failed: "));
            println(error.f_str());
            client.stop();
            return false;
        }

        // Extract values
        for (JsonObject data_wind_item : doc["data"]["wind"].as<JsonArray>())
        {

            long data_wind_item_timestamp = data_wind_item["timestamp"]; // 1640905200, 1640916000, 1640926800, ...
            if (unixtime - data_wind_item_timestamp >= 0 && unixtime - data_wind_item_timestamp < interval_hours * HOURS_TO_SECONDS)
            {

                // int data_wind_item_utcOffset = data_wind_item["utcOffset"]; // 1, 1, 1, 1, 1, 1, 1, 1
                float data_wind_item_speed = data_wind_item["speed"]; // 6.73059, 5.31981, 0.3269, 7.25703, 8.63038, ...
                wind_speed = data_wind_item_speed;
                double data_wind_item_direction = data_wind_item["direction"]; // 172.66387, 189.37292, 94.54296, ...
                wind_direction = data_wind_item_direction;
                // const char *data_wind_item_directionType = data_wind_item["directionType"]; // "Offshore", ...
                // float data_wind_item_gust = data_wind_item["gust"];                         // 6.73059, 5.83638, 2.16221, 7.56043, 13.71469, ...
                // int data_wind_item_optimalScore = data_wind_item["optimalScore"];           // 2, 2, 2, 2, 2, 0, 2, 0
            }
        }

        return true;
    }
    else if (type == TIDES)
    {
        // Allocate the JSON document
        // Use https://arduinojson.org/v6/assistant to compute the capacity.
        const size_t capacity = 3072;
        DynamicJsonDocument doc(capacity);

        // Parse JSON object
        // There are four chars before the JSON object. They may correspond to the byte order mark (see https://arduinojson.org/v6/api/misc/deserializationerror/ for details). This loop gets rid of them and enables the correct reading of the JSON.
        for (int ii = 0; ii < 4; ii++)
        {
            client.read();
        }

        DeserializationError error = deserializeJson(doc, client);
        if (error)
        {
            print(F("deserializeJson() failed: "));
            println(error.f_str());
            client.stop();
            return false;
        }

        long tides_timestamp[MAX_TIDES_NB] = {0, 0, 0, 0};
        const char *tides_type[MAX_TIDES_NB] = {"",
                                                "",
                                                "",
                                                ""};
        int nb_tides = 0;

        // Extract values
        for (JsonObject data_tides_item : doc["data"]["tides"].as<JsonArray>())
        {
            long data_tides_item_timestamp = data_tides_item["timestamp"]; // 1649800800, 1649804400, 1649808000, 1649811600, ...
            // int data_tides_item_utcOffset = data_tides_item["utcOffset"];  // 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, ...
            const char *data_tides_item_type = data_tides_item["type"]; // "NORMAL", "NORMAL", "NORMAL", "NORMAL", "HIGH", ...
            // float data_tides_item_height = data_tides_item["height"];      // 1.89, 2.3, 2.64, 2.8, 2.81, 2.74, 2.46, 2, 1.5, 1.08, ...
            if (strcmp(data_tides_item_type, "HIGH") == 0 || strcmp(data_tides_item_type, "LOW") == 0)
            {
                tides_type[nb_tides] = data_tides_item_type;
                tides_timestamp[nb_tides] = data_tides_item_timestamp;
                nb_tides++;
            }
        }

        /*println("Types\tTimestamps");
        for (int ii = 0; ii < nb_tides; ii++)
        {
            println(String(tides_type[ii]) + " \t" + String(tides_timestamp[ii]));
        }*/
        for (int ii = 0; ii < nb_tides; ii++)
        {
            println(String(tides_timestamp[ii]));
            if (unixtime <= tides_timestamp[ii])
            {
                tide = NUM_LEDS_PANELS - NUM_LEDS_PANELS * (tides_timestamp[ii] - unixtime) / (TIDE_HOURS * HOURS_TO_SECONDS + TIDE_MINUTES * MINUTES_TO_SECONDS + TIDE_SECONDS);
                if (strcmp(tides_type[ii], "HIGH") == 0)
                {
                    next_tide = HIGH_TIDE;
                }
                else
                {
                    next_tide = LOW_TIDE;
                }
                println("Tide\tNext Tide");
                println(String(tide) + "\t" + String(next_tide));
                return true;
            }
        }
        if (unixtime >= tides_timestamp[nb_tides])
        {
            println("map");
            println(String(unixtime - tides_timestamp[nb_tides]));
            println(String(TIDE_HOURS * HOURS_TO_SECONDS + TIDE_MINUTES * MINUTES_TO_SECONDS + TIDE_SECONDS));
            tide = map(unixtime - tides_timestamp[nb_tides], 0, TIDE_HOURS * HOURS_TO_SECONDS + TIDE_MINUTES * MINUTES_TO_SECONDS + TIDE_SECONDS, 0, NUM_LEDS_PANELS);
            // tide = NUM_LEDS_PANELS * (unixtime - tides_timestamp[nb_tides]) / (TIDE_HOURS * HOURS_TO_SECONDS + TIDE_MINUTES * MINUTES_TO_SECONDS + TIDE_SECONDS);
            if (strcmp(tides_type[nb_tides], "HIGH") == 0)
            {
                next_tide = LOW_TIDE;
            }
            else
            {
                next_tide = HIGH_TIDE;
            }
            println("Tide\tNext Tide");
            println(String(tide) + "\t" + String(next_tide));
            return true;
        }
    }
    else if (type == WEATHER)
    {
    }

    return false;
}

void Surf_Checker::get_data()
{
    if (!get_time())
    {
        error = true;
        return;
    }
    if (!get_wave())
    {
        error = true;
        return;
    }
    if (!get_wind())
    {
        error = true;
        return;
    }
    if (!get_tides())
    {
        error = true;
        return;
    }
    error = false;
}

bool Surf_Checker::get_data(HttpDataType type)
{
    unsigned long counter = millis();
    while (!http_request(type))
    {
        if (millis() - counter >= query_timeout)
        {
            return false;
        }
    }
    return true;
}

bool Surf_Checker::get_time()
{
    return get_data(TIME);
}

bool Surf_Checker::get_wave()
{
    return get_data(WAVE);
}

bool Surf_Checker::get_wind()
{
    return get_data(WIND);
}

bool Surf_Checker::get_tides()
{
    return get_data(TIDES);
}

void Surf_Checker::display_data()
{
    if (!error)
    {
        println("Current time : " + String(unixtime));
        println();
        println("Swells (sorted)");
        println("Heights\tPeriods\tDirections");
        for (int ii = 0; ii < SWELL_NB; ii++)
        {
            println(String(swell_heights[ii]) + "\t" + String(swell_periods[ii]) + "\t\t" + String(swell_directions[ii]));
            // Serial.print(swell_heights[ii]);
            // Serial.print("\t");
            // Serial.print(swell_periods[ii]);
            // Serial.print("\t\t");
            // Serial.print(swell_directions[ii]);
            // Serial.println();
        }
        println();
        println("Wind");
        println("Speed\tDirection :");
        println(String(wind_speed) + "\t" + String(wind_direction));
        // Serial.print(wind_speed);
        // Serial.print("\t");
        // Serial.print(wind_direction);
        // Serial.println();

        println();
        println("Displaying data on LED panel.");

        for (int ii = 0; ii < NUM_LEDS_CLOCK; ii++)
        {
            leds_clock[ii] = CRGB::Black;
        }
        for (int ii = 0; ii < NUM_LEDS_SIDES; ii++)
        {
            leds_sides[ii] = CRGB::Black;
        }
        // Swell
        int dir_swell = (int)round(swell_directions[0] * (double)(NUM_DIRECTIONS - 1) / 360.0);
        int dir_wind = (int)round(wind_direction * (double)(NUM_DIRECTIONS - 1) / 360.0);
        leds_clock[2 * dir_swell + 1] = CHSV(HUE_AQUA, 255, 255);
        leds_clock[2 * dir_wind] = CHSV(HUE_BLUE, 255, 255);

        // Tide
        if (tide >= 0 && tide < 11)
        {
            leds_sides[tide] = CRGB::Red;
        }
        else
        {
            println("Something wrong with the tide calculations...");
        }
        leds_sides[next_tide] = CRGB::Green;
    }
    else
    {
        println("An error occured while retrieving data.");
    }
}