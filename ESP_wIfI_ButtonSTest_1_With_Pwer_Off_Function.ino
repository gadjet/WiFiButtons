/*
  Based on a sketch form Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-many-to-one-esp8266-nodemcu/
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
#include<ESP8266WiFi.h>
#include<espnow.h>

uint8_t bridgeAddress1[] = {0xBC, 0xDD, 0xC2, 0x2F, 0xBB, 0x29};   //please update this with the MAC address of your ESP-NOW TO MQTT brigde
//uint8_t bridgeAddress2[] = {0xA8, 0x48, 0xFA, 0xCE, 0x83, 0xD1};   //please update this with the MAC address of your ESP-NOW TO MQTT brigde

// Set your Board ID (ESP32 Sender #1 = BOARD_ID 1, ESP32 Sender #2 = BOARD_ID 2, etc)
#define BOARD_ID 100

const int BUTTON1 = 12;
const int BUTTON2 = 13;
const int BUTTON3 = 14;
const int powerOff = 2; // set to low to turn off LDO

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int id;
  char state[8];
  int vBatt;
} struct_message;

// Create a struct_message called test to store variables to be sent
struct_message myData;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  /* Serial.print("\r\nLast Packet Send Status: ");
    if (sendStatus == 0){
     Serial.println("Delivery success");
    }
    else{
     Serial.println("Delivery fail");
    }
  */
}

void setup() {
  // Init Serial Monitor
  //Serial.begin(115200);
  // initialize the inputs:
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  pinMode(BUTTON3, INPUT);

  // Make the END pin High to keep on the ESP Power
  pinMode(powerOff, OUTPUT);
  digitalWrite(powerOff, HIGH);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    //Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Set ESP-NOW role
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  // Once ESPNow is successfully init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_add_peer(bridgeAddress1, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  //esp_now_add_peer(bridgeAddress2, ESP_NOW_ROLE_SLAVE, 2, NULL, 0);
}

void loop() {
  // Set values to send
  myData.id = BOARD_ID;
  // Read the state of the reed switch and send open or closed
  if (digitalRead(BUTTON1) == HIGH) {
    strcpy(myData.state, "Button1");
    //Serial.println("Button1");
  }
  else if (digitalRead(BUTTON2) == HIGH) {
    strcpy(myData.state, "Button2");
    //Serial.println("Button2");
  }
  else if (digitalRead(BUTTON3) == HIGH) {
    strcpy(myData.state, "Button3");
    //Serial.println("Button3");
  }
  else {
    strcpy(myData.state, "None");
    //Serial.println("None");
  }

  myData.vBatt = (analogRead(A0) * 4.2 * 10 / 1023);
  Serial.println(myData.vBatt);


  // Send message via ESP-NOW
  esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
  //ESP.deepSleep(0);
  delay(100);
  digitalWrite(powerOff, LOW); //Switch off supply
}
