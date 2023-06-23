#include "ustd_platform.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

#include "scheduler.h"
#include "net.h"

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

const char *bmpUrl = "https://nalanda:8289/ministation/10865";
const char *server = "nalanda";
const char *bmpUrlNp = "https://nalanda/ministation/10865";

const char *test_root_ca =
    "-----BEGIN CERTIFICATE-----\n"
    "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"
    "aWcF/lcyXRdXdzh0ib2okfu9RRgpCA=="
    "-----END CERTIFICATE-----\n";

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
            Serial.println("indra: received network connect");
            if (!bNetUp) {
                Serial.println("indra: net state online, checking indra connection...");
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

    Serial.println("Starting display");
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
    // Set the rotation of the display if necessary
    // tft.setRotation(1);  // Uncomment this line for landscape orientation

    // Clear the display
    Serial.println("Clearing the display");
    // tft.fillScreen(ST77XX_WHITE);
    Serial.println("Display cleared");

    uint16_t *bmpBuffer = (uint16_t *)byteBuffer;
    // Draw the BMP image
    tft.drawRGBBitmap(0, 0, (uint16_t *)bmpBuffer, (uint16_t)BMP_WIDTH, (uint16_t)BMP_HEIGHT);

    Serial.println("BMP image displayed");
}

void downloadAndDisplayBMP() {
    WiFiClientSecure client;

    Serial.print("Connecting to ");
    Serial.println(bmpUrl);
    client.setCACert(test_root_ca);
    // client.setInsecure();  // skip verification
    if (client.connect(server, 8289)) {
        Serial.println("Connected!");

        // Send the HTTP GET request
        // client.print("GET /image.bmp HTTP/1.1\r\n");
        // client.print("Host: example.com\r\n");
        // client.print("Connection: close\r\n\r\n");

        // Send the HTTPS GET request
        client.print("GET ");
        client.print("/ministation/10865");  // bmpUrl);
        client.print(" HTTP/1.1\r\n");
        client.print("Host: ");
        client.print("nalanda");  // Replace with the actual host name
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

        Serial.println("Reading BMP image");

        // Read the BMP data into the buffer
        uint32_t bytesRead = 0;
        while (client.connected() && bytesRead < sizeof(byteBuffer)) {
            if (client.available()) {
                // bmpBuffer[
                bytesRead += client.readBytes((uint8_t *)&byteBuffer[bytesRead], (size_t)(BMP_HEIGHT * BMP_WIDTH * 2 - bytesRead));
                Serial.println("Read " + String(bytesRead) + " bytes");
            }
        }
        Serial.print("BMP image read: ");
        Serial.print(bytesRead);
        Serial.println(" bytes");

        // uint16_t *bmpBuffer = (uint16_t *)byteBuffer;
        for (int i = 0; i < 20000; i++) {
            Serial.print("0x");
            Serial.print(byteBuffer[i]);
            Serial.print(" ");
            if (i % 16 == 15) {
                Serial.println();
            }
        }

        // Disconnect from the server
        client.stop();

        // Display the BMP image on the TFT display
        displayBMP();
    } else {
        Serial.println("Connection failed!");
    }
}

void appLoop() {
    if (bNetUp) {
        --ticker;
        if (ticker <= 0) {
            downloadAndDisplayBMP();
            ticker = 100;
        }
    }
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}
