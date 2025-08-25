#include <WiFi.h>
#include <esp_now.h>

typedef struct message {
  int nodeId;
  int dataNum;
  int value1;
  int value2;
  int value3;
  int value4;
} message;

void onReceive(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  message data;
  memcpy(&data, incomingData, sizeof(data));

  Serial.printf("Node %d Value %d -> %d, %d, %d, %d\n", data.nodeId, data.dataNum, data.value1, data.value2, data.value3, data.value4);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.channel(5);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onReceive);
}

void loop() {}

