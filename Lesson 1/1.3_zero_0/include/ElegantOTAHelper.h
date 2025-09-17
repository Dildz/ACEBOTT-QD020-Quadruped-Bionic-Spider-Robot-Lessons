// ElegantOTAHelper.h

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESP8266Ping.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>   // set #define ELEGANTOTA_USE_ASYNC_WEBSERVER 0 to 1 in the ElegantOTA.h file

// Replace these with your WiFi network settings
const char* ssid = "TP-LINK 2.5GHz";
const char* password = "Uitenhage1";

// Static IP address configs (change accordingly)
IPAddress staticIP(192, 168, 3, 10);
IPAddress gateway(192, 168, 3, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress DNS(192, 168, 3, 1);

// Asyncronous web server, on port 80
AsyncWebServer server(80);


// Function to initialize Serial monitor, Wi-Fi, Built-In LED, server, and OTA
void setupOTA() {
    Serial.begin(115200);
    delay(100);

    WiFi.mode(WIFI_STA);            // (STA)tion mode
    WiFi.persistent(false);         // do not store Wi-Fi configuration in EEPROM
    WiFi.setAutoReconnect(true);    // reconnect if connection is lost
    WiFi.hostname("ESP8266");       // set a hostname

    WiFi.config(staticIP, gateway, subnet, DNS);  // use static IP
    WiFi.begin(ssid, password);   // connect to Wi-Fi
    Serial.println("");

    // Wait for connection to be established
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    // Initialize the built in LED & set it to high (active low)
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);   // turn on LED (active low)
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Perform a ping test to Google DNS servers
    Serial.println("\nPinging 8.8.8.8 to test connectivity...");
    if (Ping.ping("8.8.8.8")) {
        Serial.println("Ping successful!");
    } else {
        Serial.println("Ping failed, network does not have internet access.");
    }

    // Handle unknown requests
    server.onNotFound([](AsyncWebServerRequest *request){
        request->send(404, "text/plain", "404 - Page Not Found, oops!");
    });

    // Default landing page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        String message = "Hi! I am ESP8266, and look! No wires!!.\n\n";
        message += "Browse to http://";
        message += staticIP.toString();   // convert to string
        message += "/update to update my firmware.";

        request->send(200, "text/plain", message);
    });

    // Setup the server
    ElegantOTA.begin(&server);
    server.begin();
    delay(500);
    Serial.println("\nHTTP server started");
    Serial.print("Open: http://");
    Serial.print(staticIP);
    Serial.println("/update in a browser to update firmware");
}
