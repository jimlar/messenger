// wifi_settings.h should be a file containing the following: 
//
// const char* ssid     = "wifi ssid";
// const char* password = "wifi passwd";
//
// See http://forum.arduino.cc/index.php?topic=37371.0 on where to put the file
//
#include <wifi_settings.h>

// For LED stuff
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

Adafruit_8x16minimatrix matrix = Adafruit_8x16minimatrix();

const int led_disp_width = 16;
const int led_char_width = 6;
int led_scroll_pos = led_disp_width;
String led_text = "";
int led_text_width = 0;
int beeps = 0;

void set_led_text(String text) {
  led_text = text;
  led_text_width = led_text.length() * led_char_width;
  beeps = 0;
}

ESP8266WebServer server ( 80 );


void handleGetRequest() {
  server.send(200, "text/plain", "PUT your message (as a plain body, no encoding)");
}

void handlePostRequest() {
  set_led_text(server.arg("plain"));
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

  // Piezo pin
  pinMode(14, OUTPUT);
  
  //Setup LED  
  matrix.begin(0x70);
  matrix.clear();
  matrix.setTextSize(1);
  matrix.setTextWrap(false);
  matrix.setTextColor(LED_ON);
  matrix.setRotation(1);
  matrix.writeDisplay();
  matrix.setBrightness(5);
  
  //Setup WIFI
  set_led_text(String("Connecting to ") + ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    scroll_once();
  }

  set_led_text(String("PUT message to http://") + WiFi.localIP().toString() + String(":80/"));
  scroll_once();
  
  server.on("/", HTTP_GET, handleGetRequest);
  server.on("/", HTTP_PUT, handlePostRequest);
  server.begin();
}

void play_sound() {
  tone(14, 4000);
  delay(10);
  noTone(14);
}

void draw_message() {
  matrix.clear();
  matrix.setCursor(led_scroll_pos, 0);
  matrix.print(led_text);
  matrix.writeDisplay();
}

void scroll_once() {
  led_scroll_pos = led_disp_width;
  while (led_scroll_pos >= -led_text_width) {
    draw_message();
    led_scroll_pos--;
    delay(15);
  }
}

void loop() {
  server.handleClient();
  if (beeps < 3) {
    play_sound();
    beeps++;
  }
  
  scroll_once();
}
