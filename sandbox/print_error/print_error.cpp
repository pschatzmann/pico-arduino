#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "Arduino.h"
#include "WiFiEspAT.h"

bool active;
TimerAlarmRepeating blink, reset;
WiFiSSLClient client;

bool blink_callback(repeating_timer_t *t)
{
  active = !active; // toggle state
  digitalWrite(LED_BUILTIN, active);
  printf("Blinked!\n");
  return true;
}

bool reset_callback(repeating_timer_t *t)
{
  if (digitalRead(28) == 0)
  {
    reset_usb_boot(0, 0);
  }
  return true;
}

void setup()
{
  stdio_init_all();
  const uint RESET_PIN = 28;
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RESET_PIN, INPUT_PULLUP);
  blink.start(blink_callback, -1000);
  reset.start(reset_callback, -20);
  
  Serial1.begin(115200,SERIAL_8N1);
  WiFi.init(Serial1,2);
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println();
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  // waiting for connection to Wifi network set with the SetupWiFiConnection sketch
  Serial.println("Waiting for connection to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print('.');
  }
  Serial.println();
  Serial.println("Connected to WiFi network.");
}

void loop()
{
}

