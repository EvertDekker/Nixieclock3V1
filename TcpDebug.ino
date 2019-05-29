void init_telnet_debug(){
  #ifdef TELNET
  if (MDNS.begin(hostn)) {
        Serial.println("* MDNS responder started. Hostname -> ");
        Serial.println(hostn);
    }
   MDNS.addService("telnet", "tcp", 23);   // Initialize the telnet server of RemoteDebug
   Debug.begin(hostn); // Initiaze the telnet server
    Debug.setResetCmdEnabled(true); // Enable the reset command
    //Debug.showTime(true); // To show time
  #endif  
}
