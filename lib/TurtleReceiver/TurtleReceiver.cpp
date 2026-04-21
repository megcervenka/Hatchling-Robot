#include <TurtleReceiver.h>

bool NetController::displayDebugMessages;

NetController::NetController(bool displayDebugMessage){
  displayDebugMessages = displayDebugMessage;

}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

  memcpy(&controllerData, incomingData, sizeof(controllerData));
  if(NetController::getDebug()){
    Serial.println("received controller message");
    Serial.printf("joy1: %.2f %.2f %d  buttons: %d %d %d %d %d %d\n", controllerData.j1x, controllerData.j1y, controllerData.j1z, 
      controllerData.j2x, controllerData.j2y, controllerData.j2z,
      controllerData.butX, controllerData.butY, controllerData.butA, controllerData.butB,
      controllerData.butLt, controllerData.butLb, controllerData.butRt, controllerData.butRb, controllerData.butDpad); // bro this line is a mess there must be a better way
  }

}

void NetController::controllerSetup(){
  // Set device as a Wi-Fi Station
  
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

bool NetController::getDebug(){return displayDebugMessages;}

float NetController::getJoy1X(){return controllerData.j1x;}
float NetController::getJoy1Y(){return controllerData.j1y;}
bool NetController::getJoy1Z(){return controllerData.j1z;}

float NetController::getJoy2X(){return controllerData.j2x;}
float NetController::getJoy2Y(){return controllerData.j2y;}
bool NetController::getJoy2Z(){return controllerData.j2z;}

bool NetController::getA(){return controllerData.butA;}
bool NetController::getB(){return controllerData.butB;}
bool NetController::getX(){return controllerData.butX;}
bool NetController::getY(){return controllerData.butY;}

bool NetController::getLt(){return controllerData.butLt;}
bool NetController::getLb(){return controllerData.butLb;}
bool NetController::getRt(){return controllerData.butRt;}
bool NetController::getRb(){return controllerData.butRb;}

bool NetController::getDpad(){return controllerData.butDpad;}

/**
 * @brief function to print out your mac address. send it to us once u got it. make sure to call Serial.begin() before doin this
 */
void printMacAddress(){
 
  WiFi.mode(WIFI_STA);
  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK) {
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                  baseMac[0], baseMac[1], baseMac[2],
                  baseMac[3], baseMac[4], baseMac[5]);
  } else {
    Serial.println("Failed to read MAC address");
  }
}