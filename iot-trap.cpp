#include <ESP8266WiFi.h>
//------------------------------------------
//WiFi
const char* ssid = "yourSSID";
const char* password = "yourPassword";
//------------------------------------------
//Slack
#define HOST "hooks.slack.com"
#define URL "/services/yourToken"
#define URL1 "/services/yourTokenForDailyUpdate"
String MESSAGE1 = "Your #1 rat trap has been sprung";
String MESSAGE2 = "Your #1 mouse trap has been sprung";
String MESSAGE3 = "Your #2 rat trap has been sprung";
String MESSAGE4 = "Your #2 mouse trap has been sprung";
String message;
String message1;
//------------------------------------------
//IO Pins constants
const int ratTrap1 = D5;
const int mouseTrap1 = D6;
const int ratTrap2 = D7;
const int mouseTrap2 = D1;
//------------------------------------------
//Variables
uint32_t ratTrap1State = 0;
uint32_t ratTrap1LastState;
uint32_t mouseTrap1State = 0;
uint32_t mouseTrap1LastState;
uint32_t ratTrap2State = 0;
uint32_t ratTrap2LastState;
uint32_t mouseTrap2State = 0;
uint32_t mouseTrap2LastState;
uint32_t deepSleepTime = 20e6; //3600e6;  //Change this to change the Deepsleep time (1e6 = 1sec)
uint32_t counter;
//------------------------------

void setup() {
  Serial.begin(74880);
  delay(10);
  while (!Serial) {}
  Serial.println("***************");
  Serial.println("start of setup");
  Serial.println("***************");

  pinMode (ratTrap1, INPUT_PULLUP);
  pinMode (mouseTrap1, INPUT_PULLUP);
  pinMode (ratTrap2, INPUT_PULLUP);
  pinMode (mouseTrap2, INPUT_PULLUP);

  ESP.rtcUserMemoryRead(4, &counter, sizeof(counter));
  Serial.print("RTCMemory just after booting ");
  Serial.println(counter);
  Serial.println("***********");
  ESP.rtcUserMemoryRead(0, &ratTrap1LastState, sizeof(ratTrap1LastState));
  Serial.print("RTCMemory_ratTrap1LastState ");
  Serial.println(ratTrap1LastState);
  ESP.rtcUserMemoryRead(1, &mouseTrap1LastState, sizeof(mouseTrap1LastState));
  Serial.print("RTCMemory_mouseTrap1LastState ");
  Serial.println(mouseTrap1LastState);
  ESP.rtcUserMemoryRead(2, &ratTrap2LastState, sizeof(ratTrap2LastState));
  Serial.print("RTCMemory_ratTrap2LastState ");
  Serial.println(ratTrap2LastState);
  ESP.rtcUserMemoryRead(3, &mouseTrap2LastState, sizeof(mouseTrap2LastState));
  Serial.print("RTCMemory_mouseTrap2LastState ");
  Serial.println(mouseTrap2LastState);
  Serial.println("***********");
}

void loop() {
  Serial.println("***************");
  Serial.println("start of loop");
  Serial.println("***************");
  //------------------------------------------
  //trap check
  ratTrap1State = digitalRead(ratTrap1);
  if (ratTrap1State != ratTrap1LastState && ratTrap1State == 1)
  {
    message = MESSAGE1;
    post();
  }
  ratTrap1LastState = ratTrap1State;
  ESP.rtcUserMemoryWrite(0, &ratTrap1LastState, sizeof(ratTrap1LastState));

  mouseTrap1State = digitalRead(mouseTrap1);
  if (mouseTrap1State != mouseTrap1LastState && mouseTrap1State == 1)
  {
    message = MESSAGE2;
    post();
  }
  mouseTrap1LastState = mouseTrap1State;
  ESP.rtcUserMemoryWrite(1, &mouseTrap1LastState, sizeof(mouseTrap1LastState));

  ratTrap2State = digitalRead(ratTrap2);
  if (ratTrap2State != ratTrap2LastState && ratTrap2State == 1)
  {
    message = MESSAGE3;
    post();
  }
  ratTrap2LastState = ratTrap2State;
  ESP.rtcUserMemoryWrite(2, &ratTrap2LastState, sizeof(ratTrap2LastState));

  mouseTrap2State = digitalRead(mouseTrap2);
  if (mouseTrap2State != mouseTrap2LastState && mouseTrap2State == 1)
  {
    message = MESSAGE4;
    post();
  }
  mouseTrap2LastState = mouseTrap2State;
  ESP.rtcUserMemoryWrite(3, &mouseTrap2LastState, sizeof(mouseTrap2LastState));

  delay(500);
  //------------------------------------------
  //counter
  if (counter >= 24 )
  {
    Serial.println("sending a message to Slack");
    counter = 0;
    ESP.rtcUserMemoryWrite(4, &counter, sizeof(counter));
    post1();
  }
  else
  {
    counter++;
    ESP.rtcUserMemoryWrite(4, &counter, sizeof(counter));
  }

  ESP.rtcUserMemoryRead(4, &counter, sizeof(counter));
  Serial.print("RTCMemory (after if/else statement) ");
  Serial.println(counter);
  Serial.println("***********");
  delay(500);

  //------------------------------------------
  //Deepsleep
  Serial.print("Going into deep sleep for ");
  Serial.print(deepSleepTime / 1000000);
  Serial.print(" seconds");
  ESP.deepSleep(deepSleepTime);
}

void post() {                    //Post to Slack when a trap is sprung.
  Serial.println("***************");
  Serial.println("start of post()");
  Serial.println("***************");

  //------------------------------------------
  //Wifi
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
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  //------------------------------------------
  //Post to Slack
  Serial.println("Connecting to host...");
  WiFiClientSecure client;
  if (!client.connect(HOST, 443)) {
    Serial.println("Connection failed");
    client.stop();
    return;
  }
  Serial.println("Connected to host");

  String request = "";
  request += "POST ";
  request += URL;
  request += " HTTP/1.1\r\n";

  request += "Host: ";
  request += HOST;
  request += "\r\n";

  int len = message.length() + 12;  // JSON wrapper length
  request += "Content-Length: ";
  request += len;
  request += "\r\n";

  request += "Accept: application/json\r\n";
  request += "Connection: close\r\n";
  request += "Content-Type: application/json\r\n";

  request += "\r\n";

  request += "{\"text\": \"";
  request += message;
  request += "\"}";

  Serial.print(request);
  Serial.println();
  client.print(request);

  long timeout = millis() + 5000;
  while (!client.available()) {
    if (millis() > timeout) {
      Serial.println("Request timed out");
      client.stop();
      return;
    }
  }
  Serial.println("Response:");
  while (client.available()) {
    Serial.write(client.read());
  }
  Serial.println();
  Serial.println("Request complete");
}

void post1() {                    //Daily update post to Slack.
  Serial.println("***************");
  Serial.println("start of post1()");
  Serial.println("***************");

  //------------------------------------------
  //Daily message
  String string1 = "Daily check-in...all systems OK. Trap Status: ";
  string1 += "Rat trap 1: ";
  string1 += ratTrap1State;
  string1 += ". Mouse trap 1: ";
  string1 += mouseTrap1State;
  string1 += ". Rat trap 2: ";
  string1 += ratTrap2State;
  string1 += ". Mouse trap 2: ";
  string1 += mouseTrap2State;

  message1 = string1;

  //------------------------------------------
  //Wifi
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
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //------------------------------------------
  //Post to Slack
  Serial.println("Connecting to host...");
  WiFiClientSecure client;
  if (!client.connect(HOST, 443)) {
    Serial.println("Connection failed");
    client.stop();
    return;
  }
  Serial.println("Connected to host");

  String request = "";
  request += "POST ";
  request += URL1;
  request += " HTTP/1.1\r\n";

  request += "Host: ";
  request += HOST;
  request += "\r\n";

  int len = message1.length() + 12;  // JSON wrapper length
  request += "Content-Length: ";
  request += len;
  request += "\r\n";

  request += "Accept: application/json\r\n";
  request += "Connection: close\r\n";
  request += "Content-Type: application/json\r\n";

  request += "\r\n";

  request += "{\"text\": \"";
  request += message1;
  request += "\"}";

  Serial.print(request);
  Serial.println();
  client.print(request);

  long timeout = millis() + 5000;
  while (!client.available()) {
    if (millis() > timeout) {
      Serial.println("Request timed out");
      client.stop();
      return;
    }
  }
  Serial.println("Response:");
  while (client.available()) {
    Serial.write(client.read());
  }
  Serial.println();
  Serial.println("Request complete");
}
