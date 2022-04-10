/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"

/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
// Replace with your network credentials
const char* ssid = "";
const char* password = "";

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 184);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
const char back_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>

<script>
  window.history.back();
</script>


</body>
</html>
)=====";

/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
uint8_t gpioIdx=0, gpioNo[]={18, 19};

const char voiceCmd[][2][6]={
  {//on
    {0xE9, 0x96, 0x8B, 0xE7, 0xB4, 0x85}, 
    {0xE9, 0x96, 0x8B, 0xE7, 0xB6, 0xA0}
  },
  
  {//off
    {0xE9, 0x97, 0x9C, 0xE7, 0xB4, 0x85}, 
    {0xE9, 0x97, 0x9C, 0xE7, 0xB6, 0xA0}
  }
};

/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
void parseVoiceCommand(String text){
  
  bool foundCmd=false, isOn=false; 
  
  //*********************************************************************
  for(int i=0; i<2; i++){
    for(int j=0; j<2; j++){
      int k;
      for(k=0; k<6; k++) if(text.charAt(k) != voiceCmd[i][j][k]) break;
      if(k==6) {foundCmd=true; gpioIdx=j; isOn=(i==0); break;}
    }
    if(foundCmd) break;
  }

  //*********************************************************************
  if(foundCmd){
    if(isOn){
      Serial.printf("gpio%0d on!\n", gpioNo[gpioIdx]);
      digitalWrite(gpioNo[gpioIdx], HIGH);    
    }
    else{
      Serial.printf("gpio%0d off!\n", gpioNo[gpioIdx]);
      digitalWrite(gpioNo[gpioIdx], LOW);  
    }
  }
}

/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
void setup(){

  //*********************************************************************
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  //*********************************************************************
  pinMode(gpioNo[0], OUTPUT);
  pinMode(gpioNo[1], OUTPUT);

  //*********************************************************************
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  //*********************************************************************
  // Route for root / web page
  server.on("/stt", HTTP_GET, [] (AsyncWebServerRequest *request) {
    if (request->hasParam("text")) {
      String text=request->getParam("text")->value();
      Serial.print("text="); Serial.println(text);
      parseVoiceCommand(text);
    }

    String s = back_page; //Read HTML contents
    request->send(200, "text/html", s); 
  });
      
  //*********************************************************************
  // Start server
  server.begin();
}

/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
void loop(){
  
}
