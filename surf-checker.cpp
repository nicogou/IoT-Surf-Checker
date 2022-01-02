#include "surf-checker.h"

Surf_Checker::Surf_Checker(char *ssid, char *pass)
{
    Serial.begin(9600);
    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    if (connect(ssid, pass))
    {
    }
    else
    {
        Serial.println("Connection failed, stopping here");
        while (true)
        {
        }
    }
}

bool Surf_Checker::connect(char *ssid, char *pass)
{
    // check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE)
    {
        Serial.println("Communication with WiFi module failed!");
        // don't continue
        return false;
    }

    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION)
    {
        Serial.println("Please upgrade the firmware");
    }

    // attempt to connect to WiFi network:
    while (status != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
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
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}

bool Surf_Checker::http_request(HttpDataType type)
{
    int type_int = (int)type;
    // if there's a successful connection:
    if (client.connect(host_name[type_int], 80))
    {
        Serial.println("connecting to " + String(host_name[type_int]) + "...");
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
        Serial.println("connection failed");
        return false;
    }

    // Check HTTP status
    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));
    // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
    if (strcmp(status + 9, "200 OK") != 0)
    {
        Serial.print(F("Unexpected response: "));
        Serial.println(status);
        client.stop();
        return false;
    }

    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders))
    {
        Serial.println(F("Invalid response"));
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
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
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
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            client.stop();
            return false;
        }
        // serializeJson(doc, Serial);

        // Extract values
        for (JsonObject data_wave_item : doc["data"]["wave"].as<JsonArray>())
        {

            long data_wave_item_timestamp = data_wave_item["timestamp"]; // 1640905200, 1640916000, 1640926800, ...

            if (unixtime - data_wave_item_timestamp >= 0 && unixtime - data_wave_item_timestamp < interval_hours * 60 * 60)
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
        Serial.println("Swell");
        Serial.println("Heights\tPeriods\tDirections");
        for (int ii = 0; ii < 6; ii++)
        {
            Serial.print(swell_heights[ii]);
            Serial.print("\t");
            Serial.print(swell_periods[ii]);
            Serial.print("\t\t");
            Serial.print(swell_directions[ii]);
            Serial.println();
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
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            client.stop();
            return false;
        }

        // Extract values
        for (JsonObject data_wind_item : doc["data"]["wind"].as<JsonArray>())
        {

            long data_wind_item_timestamp = data_wind_item["timestamp"]; // 1640905200, 1640916000, 1640926800, ...
            if (unixtime - data_wind_item_timestamp >= 0 && unixtime - data_wind_item_timestamp < interval_hours * 60 * 60)
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
        Serial.println("Wind");
        Serial.println("Speed\tDirection :");
        Serial.print(wind_speed);
        Serial.print("\t");
        Serial.print(wind_direction);
        Serial.println();

        return true;
    }

    return false;
}

void Surf_Checker::get_time()
{
    http_request(TIME);
}

void Surf_Checker::get_wave()
{
    http_request(WAVE);
}

void Surf_Checker::get_wind()
{
    http_request(WIND);
}