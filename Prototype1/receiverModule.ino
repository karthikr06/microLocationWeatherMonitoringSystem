#include <WiFi.h>
#include <esp_now.h>
#include "esp_wifi.h"
#include <LiquidCrystal.h>

#define ESPNOW_CHANNEL 1 

LiquidCrystal lcd(14,27,26,25,33,32);

typedef struct message {
  int nodeId;
  int dataNum;
  int value1; 
  int value2; 
  int value3; 
  int value4; 
} message;

void onReceive(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  if (len != sizeof(message)) {
      Serial.println("Received packet with incorrect size.");
      return;
  }

  message data;
  memcpy(&data, incomingData, sizeof(data));

  Serial.printf("LOG,%d,%d,%d,%d,%d\n",
                data.nodeId, 
                data.value1, 
                data.value2, 
                data.value3, 
                data.value4);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Node: ");
  lcd.print(data.nodeId);

  lcd.setCursor(0, 1);
  lcd.print(data.value1);
  lcd.print(",");
  lcd.print(data.value2);
  lcd.print(",");
  lcd.print(data.value3);
  lcd.print(",");
  lcd.print(data.value4);
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 ESP-NOW Receiver");

  lcd.begin(16,2);
  lcd.print("Waiting for data");
  
  WiFi.mode(WIFI_STA);

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(onReceive);

  Serial.println("ESP-NOW Receiver ready on channel " + String(ESPNOW_CHANNEL));
}

void loop() {}
