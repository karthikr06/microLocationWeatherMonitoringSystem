#include <ESP8266WiFi.h>
#include <espnow.h>
#include <DFRobot_DHT11.h>

int nodeId = 2;
DFRobot_DHT11 DHT;

#define analogPin A0
#define mq2Read D5 // MUX Switch for MQ 2 Sensor
#define waterRead D6 // MUX Switch for Water level sensor
#define DHT11_PIN D1

int num = 0; //msg number for reference

int mq2Val = 0;
int waterVal = 0;

typedef struct message {
  int nodeId;
  int dataNum;
  int value1;
  int value2;
  int value3;
  int value4;
} message;

message data; 
//creating object here


void setup() {
  pinMode(analogPin, INPUT);
  pinMode(mq2Read, OUTPUT);
  pinMode(waterRead, OUTPUT);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  

  WiFi.channel(1);  

  if (esp_now_init() != 0) {
    Serial.println("ESPNOW initialisation failed!");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 5, NULL, 0);

  data.nodeId = nodeId; 
}

void loop() {
  DHT.read(DHT11_PIN);
  digitalWrite(mq2Read, HIGH); digitalWrite(waterRead, LOW);
  mq2Val = analogRead(analogPin);
  delay(100);
  digitalWrite(mq2Read, LOW); digitalWrite(waterRead, HIGH);
  waterVal = analogRead(analogPin);
  delay(100);

  //Sensor Calibration
  if (mq2Val<50){
    mq2Val = 0;
  }
  else if(mq2Val<1024){
    mq2Val += 150;
  }
  int mq2Calib = (100-((mq2Val*100)/1024));

  //Sensor Calibration
  int waterCalib = map((int)waterVal, 300, 800, 0, 40);
  if (waterCalib < 0) waterCalib = 0;
  if (waterCalib > 40) waterCalib = 40;

  data.value1 = DHT.temperature;
  data.value2 = DHT.humidity;
  data.value3 = mq2Calib;
  data.value4 = waterCalib;
  data.dataNum = num;


  esp_now_send(NULL, (uint8_t *) &data, sizeof(data));

  Serial.printf("LOG,%d,%d,%d,%d,%d\n", data.nodeId, data.value1, data.value2, data.value3, data.value4);
  delay(5000);
  num++;
  if(num>99) num=0;
}
