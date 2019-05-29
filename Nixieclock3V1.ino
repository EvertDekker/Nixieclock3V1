/*
    Gerneric Esp32 dev Module: Flash Mode: QIO, Flash Frequency: 80MHz
    Flash Size: 4M , Debug Port: Disabled, Debug Level: None,  Reset Method: ck
*/
/*
  1.0 20181105  Initial release
*/

#include <WiFi.h>
#include "time.h"  //cores/esp32/esp32-hal-time.c
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <AsyncMqttClient.h>
#include <Bounce2.h>
#include "FastLED.h"

FASTLED_USING_NAMESPACE


//Settings
#include "NixieClock3.h"

const char* vrs = "1.0"; // Evert Dekker 2018
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSW;
const char* hostn = HOSTNAME;

const char* ntpServer = "pool.ntp.org"; // "69.10.161.7";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

IPAddress ip(MY_IP);
IPAddress gateway(GATEWAY);
IPAddress subnet(NETMASK);
IPAddress dns1(DNS1);
IPAddress dns2(DNS2);

//Set the debug output, only one at the time
//#define TELNET true
//#define NONE true
#define SERIAL true

#ifdef TELNET
#include "RemoteDebug.h"        //https://github.com/JoaoLopesF/RemoteDebug
RemoteDebug Debug;
#define DEBUG_PRINTF(x,...)  Debug.printf(x, ##__VA_ARGS__)
#define DEBUG_PRINTLN(x)     Debug.println(x)
#define DEBUG_PRINT(x)       Debug.print(x)
#elif  NONE
#define DEBUG_PRINTF(x,...)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#elif SERIAL
#define DEBUG_PRINTF(x,...) Serial.printf(x, ##__VA_ARGS__)
#define DEBUG_PRINTLN(x)    Serial.println(x)
#define DEBUG_PRINT(x)      Serial.print(x)
#endif

// Config
#define LEDPIN 2 // Led pin
#define A 4
#define B 5
#define C 18
#define D 19
#define HR_T 21
#define HR 22
#define MIN_T 23
#define MIN 25
#define SEC_T 26
#define SEC 27
#define SW1 33
#define SW2 34

#define DATA_PIN    32
#define LED_TYPE    SK6812
#define COLOR_ORDER GRB
#define NUM_LEDS    6
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          20
#define FRAMES_PER_SECOND  10

// -- The core to run FastLED.show()
#define FASTLED_SHOW_CORE 0

// declaration
void IRAM_ATTR onTimer();

//Instant
hw_timer_t * timer = NULL;  //multiplex timer tubes
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;
static TaskHandle_t FastLEDshowTaskHandle = 0;
static TaskHandle_t userTaskHandle = 0;
Bounce inputswitch1 = Bounce();
Bounce inputswitch2 = Bounce();


//Globals
int nodestatus;
byte displayloop = 1;
struct tm timeinfo;
byte digit[6];
byte digitcount = 1;
boolean operatemode = true;

void setup(void) {
#ifdef SERIAL
  Serial.begin(115200);
#endif
#ifdef TELNET
  Debug.setSerialEnabled(true);
#endif
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(HR_T, OUTPUT);
  pinMode(HR, OUTPUT);
  pinMode(MIN_T, OUTPUT);
  pinMode(MIN, OUTPUT);
  pinMode(SEC_T, OUTPUT);
  pinMode(SEC, OUTPUT);
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  inputswitch1.attach(SW1);
  inputswitch1.interval(20); // interval in ms
  inputswitch2.attach(SW2);
  inputswitch2.interval(20); // interval in ms

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // tell FastLED about the LED strip configuration
  FastLED.setBrightness(BRIGHTNESS);  // set master brightness control
  int core = xPortGetCoreID();
  allLedsOff();
  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(4000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(4000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));
  xTaskCreatePinnedToCore(FastLEDshowTask, "FastLEDshowTask", 2048, NULL, 1, &FastLEDshowTaskHandle, FASTLED_SHOW_CORE);  // -- Create the FastLED show task
 // xTaskCreate(FastLEDshowTask, "FastLEDshowTask", 2048, NULL, 1, &FastLEDshowTaskHandle);

  WiFi.onEvent(WiFiEvent);
  init_mqtt();
  connectToWifi();
  init_ota();
  init_telnet_debug();

  timer = timerBegin(0, 80, true);  // timer 0, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(timer, &onTimer, true); // edge (not level) triggered
  timerAlarmWrite(timer, 2000, true); // 2000 * 1 us = 2ms, autoreload true
  timerAlarmEnable(timer); // enable
  timerStop(timer);
  // xTaskCreate(publishdata, "Publishdata" , 10000, NULL, 4, &PublishHandle);
}


void loop(void) {
  ArduinoOTA.handle();
#ifdef TELNET
  Debug.handle();
#endif
  inputswitch1.update();
  inputswitch2.update();
  printLocalTime();
  setTubeMode();
  vTaskDelay( 1000 / portTICK_PERIOD_MS );
}




void connectToWifi() {
  DEBUG_PRINTLN("Connecting to Wi-Fi...");
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(hostn);
  WiFi.config(ip, gateway, subnet, dns1, dns2);
  WiFi.begin(ssid, password);
}

void connectToMqtt() {
  DEBUG_PRINTLN("Connecting to MQTT...");
  blinkledshort(2); //2 fast blink: Mqtt connected
  vTaskDelay( 1000 / portTICK_PERIOD_MS );
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
  DEBUG_PRINT("[WiFi-event] event: ");
  DEBUG_PRINTLN(event);
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      DEBUG_PRINTLN("WiFi connected");
      DEBUG_PRINTLN("IP address: ");
      DEBUG_PRINTLN(WiFi.localIP());
      blinkledshort(1); //1 fast blink: wifi connected
      vTaskDelay( 1000 / portTICK_PERIOD_MS );
      connectToMqtt();
      //init and get the time
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      printLocalTime();
      timerStart(timer);
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      DEBUG_PRINTLN("WiFi lost connection");
      StopTheTimer();
      blinkledlong(1); //1 long blink: wifi failed
      vTaskDelay( 2000 / portTICK_PERIOD_MS );
      xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
      xTimerStart(wifiReconnectTimer, 0);
      break;
  }
}

void printLocalTime()
{

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    blinkledlong(4);
    return;
  }
  // Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  //  m.tm_year = 2018 - 1900;
  //  tm.tm_mon = 10;
  //  tm.tm_mday = 15;
  //  tm.tm_hour = 14;
  //  tm.tm_min = 10;
  //  tm.tm_sec = 10;

}
