void init_ota() {
  ArduinoOTA.setHostname(hostn);
  ArduinoOTA.setPassword((const char *)OTA_PASSW);

 ArduinoOTA.onStart([]() {
    StopTheTimer;
     timerEnd(timer);
      timer = NULL;
      setcol(0);
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";
     
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    DEBUG_PRINTLN("Start updating " + type);
   
  });

  ArduinoOTA.onEnd([]() {
    DEBUG_PRINTLN("\nOTA End");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    DEBUG_PRINTF("Progress: %u%%\r", (progress / (total / 100)));
    leds[(progress / (total / 100))/19]= CHSV(HUE_ORANGE, 255, 255);
    FastLED.show();
  });

  ArduinoOTA.onError([](ota_error_t error) {
    DEBUG_PRINTF("OTA Error[%u]: ", error);
    blinkledlong(3); //3 long blink: Ota Failed
    if (error == OTA_AUTH_ERROR) DEBUG_PRINTLN("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) DEBUG_PRINTLN("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) DEBUG_PRINTLN("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) DEBUG_PRINTLN("Receive Failed");
    else if (error == OTA_END_ERROR) DEBUG_PRINTLN("End Failed");
  });

  ArduinoOTA.begin();
}  
