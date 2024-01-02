#include <TM1637Display.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <AsyncElegantOTA.h>

// DISPLAY SETUP
#define CLK D5
#define DIO D7
TM1637Display display(CLK, DIO);

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "Rykalovi - patro";   // <-- Enter your Wifi-SSID
const char* password = "zmrzlina42";  // <-- Enter your Wifi-Password

const char* mqtt_server = "192.168.1.142";  // <-- Enter the IP of your MQTT-Server
const unsigned int mqtt_port = 1883; 
const char* mqtt_user =   "deano"; 
const char* mqtt_pass =   "Starbucks8."; // <-- Enter the Password of your MQTT-Server
String clientId = "alarmoDisplay";
char demand = 'w';
int isDisplaying = 0;

AsyncWebServer server(80);

void callback(char* topic, byte* payload, unsigned int length) {
    if (strcmp(topic,"alarmoDisplay") == 0) {
      char demand_str[length + 1];
      strncpy (demand_str, (char*)payload, length);
      demand_str[length] = '\0';
      Serial.println(demand_str);
      
      // u = up, d = down, s = stop, p = program, w = wait
      
      if (strcmp(demand_str,"1") == 0) {
        demand = '1';
      } else if (strcmp(demand_str,"2") == 0) {
        demand = '2';
      } else if (strcmp(demand_str,"3") == 0) {
        demand = '3';
      } else if (strcmp(demand_str,"4") == 0) {
        demand = '4';
      } 
      Serial.println(demand_str);
      Serial.println(demand);
    }
}



void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
      Serial.println("connected. ");
      client.subscribe("alarmoDisplay");
      client.subscribe("alarmoDisplay/Feedback");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 3 seconds");
      delay(3000);
    }
  }
}


int delay_fast = 80;
int delay_slow = 180;

byte cara0[] = { 0, 0, 0, 0 };
byte cara1[] = { 0b00110000, 0, 0, 0 };
byte cara2[] = { 0b00110110, 0, 0, 0 };
byte cara3[] = { 0b00000110, 0b00110000, 0, 0 };
byte cara4[] = { 0, 0b00110110, 0, 0 };
byte cara5[] = { 0, 0b00000110, 0b00110000, 0 };
byte cara6[] = { 0, 0, 0b00110110, 0 };
byte cara7[] = { 0, 0, 0b00000110, 0b00110000 };
byte cara8[] = { 0, 0, 0, 0b00110110 };
byte cara9[] = { 0, 0, 0, 0b00000110};

byte kolo0[] = { 0b00100000, 0, 0, 0b00000100 };
byte kolo1[] = { 0b00010000, 0, 0, 0b00000010 };
byte kolo2[] = { 0b00001000, 0, 0, 0b00000001 };
byte kolo3[] = { 0, 0b00001000, 0b00000001, 0 };
byte kolo4[] = { 0, 0b00000001, 0b00001000, 0 };
byte kolo5[] = { 0b00000001, 0, 0, 0b00001000 };
byte kolo6[] = { 0b00100000, 0, 0, 0b00000100 };
byte kolo7[] = { 0b00010000, 0, 0, 0b00000010 };
byte kolo8[] = { 0b00001000, 0, 0, 0b00000001 };
byte kolo9[] = { 0, 0b00001000, 0b00000001, 0 };
byte kolo10[] = { 0, 0b00000001, 0b00001000, 0 };
byte kolo11[] = { 0b00000001, 0, 0, 0b00001000 };



byte* caraArrays[] = {  cara0,cara1, cara2, cara3, cara4, cara5, cara6, cara7, cara8, cara9,cara0};
byte* koloArrays[] = {  kolo0,kolo1, kolo2, kolo3, kolo4, kolo5, kolo6, kolo7, kolo8, kolo9,kolo10,kolo11};


void display_regime(int rezim) {
  
  if (rezim == 1) { // Odchod - Exit delay - 3min
    display.clear();
      for (int i = 0; i < 11; i++){
      display.setSegments(caraArrays[i]);
      delay(delay_fast);
      }
  }

  if (rezim == 2) { // Triggered - Entry delay - 2min
    display.clear();
      for (int i = 10; i >= 0; i--){
      display.setSegments(caraArrays[i]);
      delay(delay_fast);
      }
  }
  if (rezim == 3) { // Armed - Kolečko doleva
    display.clear();
      for (int i = 0; i < 12; i++){
      display.setSegments(koloArrays[i]);
      delay(delay_slow);
      }
      isDisplaying = 0;
      demand = 0;
      display.clear();
  }

  if (rezim == 4) { // DISArmed - Kolečko doprava
    display.clear();
      for (int i = 11; i >= 0; i--){
      display.setSegments(koloArrays[i]);
      delay(delay_slow);
      }
      isDisplaying = 0;
      demand = 0;
      display.clear();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println(" ");
  Serial.println("Starting alarmoDisplay");
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  Serial.println(WiFi.localIP());


  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
  request->send(200, "text/plain", "Hi! I am a Wemos D1 mini.");
  });

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();

  display.setBrightness(1);  // Set the display brightness (0-7)
  display.clear();
}
  

void loop() {
    AsyncElegantOTA.loop();

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (demand == '1' || demand == '2' || demand == '3' || demand == '4') {

//    char serie = (char)Serial.read();
    char serie = (char)demand;
    if(serie == '1') {
      Serial.println("1");
      isDisplaying = 1;
      
      
    }
    else if(serie == '2') {
      Serial.println("2");
      isDisplaying = 2;

    }
    else if(serie == '3') {
      Serial.println("3");
      isDisplaying = 3;

    }
    else if(serie == '4') {
      Serial.println("4");
      isDisplaying = 4;
    }
    
  }

  display_regime(isDisplaying);
    
    
}