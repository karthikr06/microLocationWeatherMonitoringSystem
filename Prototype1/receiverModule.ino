#include <WiFi.h>
#include <esp_now.h>
#include "esp_wifi.h"

#define ESPNOW_CHANNEL 1 //setting channel 1

typedef struct message {
  int nodeId;
  int dataNum;
  int value1; // Temperature
  int value2; // Humidity
  int value3; // Air Quality
  int value4; // Rainfall
} message;

void onReceive(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  // First, check if the data received has the correct size
  if (len != sizeof(message)) {
      Serial.println("Received packet with incorrect size.");
      return;
  }

  message data;
  memcpy(&data, incomingData, sizeof(data));

  // Print the received data in a format for logger
  Serial.printf("LOG,%d,%d,%d,%d,%d\n",
                data.nodeId, 
                data.value1, 
                data.value2, 
                data.value3, 
                data.value4);
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 ESP-NOW Receiver");

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the receive callback function
  esp_now_register_recv_cb(onReceive);

  Serial.println("ESP-NOW Receiver ready on channel " + String(ESPNOW_CHANNEL));
}

void loop() {}
