/// Includes
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// Uncomment one of the following lines to define the role of this ESP32 device before compiling.
#define SENDER
#define RECEIVER

// RECEIVER MAC Address
constexpr uint8_t broadcastAddress[] = {0x78, 0xee, 0x4c, 0x01, 0x6a, 0x10};

esp_now_peer_info_t peerInfo;

// Datapackage for sending 
struct DataPackage
{
	int number1{2};
  int number2{42};
};

DataPackage data;

#ifdef SENDER
// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
#endif

#ifdef RECEIVER
/// @brief Callback function that will be executed when data is received
/// @param mac 
/// @param incomingData 
/// @param len 
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
#endif

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  #ifdef SENDER
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Transmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    //Serial.println("Failed to add peer");
    return;
  }
  #endif
}

void loop() {
  #ifdef SENDER
  data.number1++;
  data.number2++;
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &data, sizeof(data));
   
  if (result == ESP_OK) {
    //Serial.println("Sent with success");
  }
  else {
    //Serial.println("Error sending the data");
  }
  #endif

  #ifdef RECEIVER
  // Once ESPNow is successfully Init, we will register for recv CB to
    // get recv packer info
    esp_now_register_recv_cb(OnDataRecv);
  #endif
  delay(10); // Delay to avoid flooding the receiver
}

#ifdef RECEIVER
// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&data, incomingData, sizeof(data));
    Serial.print("Read This: ");
    Serial.print(data.number1);
    Serial.print(" | Number: ");
    Serial.print(data.number2);
    Serial.print("      \r");
}
#endif