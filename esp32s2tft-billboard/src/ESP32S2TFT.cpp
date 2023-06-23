#include "ustd_platform.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

#include "scheduler.h"
#include "jsonfile.h"
#include "net.h"

// TODO: You need to edit the content of ca_cert.h and insert your own root certificate authority.
#include "ca_cert.h"

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::Net net(LED_BUILTIN);

bool bNetUp = false;

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Size of display of Adafruit ESP32-S2 TFT FeatherWing
#define BMP_WIDTH 240
#define BMP_HEIGHT 135
int ticker = 0;
uint8_t byteBuffer[BMP_WIDTH * BMP_HEIGHT * 2];

bool bInsecure = false;
String server;
uint16_t port;
String url;
long refresh_interval_secs = 600;

void subsMsg(String topic, String msg, String originator) {
    if (topic == "net/network") {
        // network state received:
        JSONVar jsonState = JSON.parse(msg);
        if (JSON.typeof(jsonState) != "object") {
            DBG("indra: Received broken network state " + msg);
            return;
        }
        String state = (const char *)jsonState["state"];
        String hostname = (const char *)jsonState["hostname"];
        String mac = (const char *)jsonState["mac"];
        if (state == "connected") {
#ifdef USE_SERIAL_DBG
            Serial.println("indra: received network connect");
#endif
            if (!bNetUp) {
#ifdef USE_SERIAL_DBG
                Serial.println("indra: net state online, checking indra connection...");
#endif
                bNetUp = true;
            }
        } else {
            bNetUp = false;
            DBG2("indra: net state offline");
        }
    }
}

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
    Serial.println("Starting up...");
#endif  // USE_SERIAL_DBG

    net.begin(&sched);
    ustd::jsonfile jf;
    bInsecure = jf.readBool("billboard/insecure_ssl", false);
    server = jf.readString("billboard/server", "nalanda");
    url = jf.readString("billboard/url", "/ministation/10865");
    port = (uint16_t)jf.readLong("billboard/port", 8289);
    refresh_interval_secs = jf.readLong("billboard/refresh_interval_secs", 600);

    // turn on backlite
    pinMode(TFT_BACKLITE, OUTPUT);
    digitalWrite(TFT_BACKLITE, HIGH);

    // turn on the TFT / I2C power supply
    pinMode(TFT_I2C_POWER, OUTPUT);
    digitalWrite(TFT_I2C_POWER, HIGH);
    delay(10);

    int tId = sched.add(appLoop, "main", 1000000);

    tft.init(135, 240);  // Init ST7789 240x135
    tft.setRotation(3);
    tft.fillScreen(ST77XX_BLACK);
    // subscribe to network state
    sched.subscribe(tId, "net/network", subsMsg);
    sched.publish("net/network/get");
}

void displayBMP() {
    uint16_t *bmpBuffer = (uint16_t *)byteBuffer;
    // Draw the BMP image
    tft.drawRGBBitmap(0, 0, (uint16_t *)bmpBuffer, (uint16_t)BMP_WIDTH, (uint16_t)BMP_HEIGHT);
}

void downloadAndDisplayBMP() {
    WiFiClientSecure client;

#ifdef USE_SERIAL_DBG
    Serial.print("Connecting to ");
    Serial.println(server);
#endif  // USE_SERIAL_DBG
    client.setCACert(root_ca);
    if (bInsecure) {
        client.setInsecure();
    }
    // client.setInsecure();  // skip verification
    if (client.connect(server.c_str(), port)) {
#ifdef USE_SERIAL_DBG
        Serial.println("Connected!");
#endif  // USE_SERIAL_DBG
        // Send the HTTPS GET request
        client.print("GET ");
        client.print(url.c_str());
        client.print(" HTTP/1.1\r\n");
        client.print("Host: ");
        client.print(server.c_str());
        client.print("\r\n");
        client.print("Connection: close\r\n\r\n");

        Serial.println("Request sent, skipping headers");
        // Skip HTTP headers
        bool bmpHeaderFound = false;
        while (client.connected() && !bmpHeaderFound) {
            if (client.available()) {
                String line = client.readStringUntil('\n');
                line.trim();

                if (line == "") {
                    bmpHeaderFound = true;
                }
            }
        }
#ifdef USE_SERIAL_DBG
        Serial.println("Reading BMP image");
#endif  // USE_SERIAL_DBG
        // Read the BMP data into the buffer
        uint32_t bytesRead = 0;
        while (client.connected() && bytesRead < sizeof(byteBuffer)) {
            if (client.available()) {
                // bmpBuffer[
                bytesRead += client.readBytes((uint8_t *)&byteBuffer[bytesRead], (size_t)(BMP_HEIGHT * BMP_WIDTH * 2 - bytesRead));
            }
        }
#ifdef USE_SERIAL_DBG
        Serial.print("BMP image read: ");
        Serial.print(bytesRead);
        Serial.println(" bytes");
#endif  // USE_SERIAL_DBG

        // Disconnect from the server
        client.stop();

        // Display the BMP image on the TFT display
        displayBMP();
    } else {
#ifdef USE_SERIAL_DBG
        Serial.println("Connection failed!");
#endif  // USE_SERIAL_DBG
    }
}

void appLoop() {
    if (bNetUp) {
        --ticker;
        if (ticker <= 0) {
            downloadAndDisplayBMP();
            ticker = refresh_interval_secs;
        }
    }
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}
