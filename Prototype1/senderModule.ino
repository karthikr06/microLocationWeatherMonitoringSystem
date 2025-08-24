#include <ESP8266WiFi.h>
#include <espnow.h>

int num = 0; //Variable to show message number

typedef struct message {
  int nodeId;
  int dataNum;
  int value1;
  int value2;
  int value3;
} message;

//Creating an instance of message
message data; 

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  
  //channel number
  WiFi.channel(1);  

  if (esp_now_init() != 0) {
    Serial.println("ESPNOW initialisation failed!");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  //Setting Broadcast Address: 
  uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  // Set a unique node ID for this sender
  data.nodeId = 2;  // Setting the node ID for this sender module
}

void loop() {
  // Temporary Data
  data.value1 = random(10, 50);
  data.value2 = random(50, 100);
  data.value3 = random(100, 200);
  data.dataNum = num;

  // Send the struct
  esp_now_send(NULL, (uint8_t *) &data, sizeof(data));

  Serial.printf("Node %d sent data %d: %d, %d, %d\n", data.nodeId, data.dataNum, data.value1, data.value2, data.value3);
  delay(5000);
  num++;
  if(num>99) num=0;
}
