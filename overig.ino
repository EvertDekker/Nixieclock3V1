void blinkledshort(byte blinks) {
  for (int i = 1; i <= blinks; i++) {
    digitalWrite(LEDPIN, HIGH);
    NoErrorLeds(blinks);
    delay(50);
    digitalWrite(LEDPIN, LOW);
    allLedsOff();
    delay(100);
  }
}

void blinkledlong(byte blinks) {
  for (int i = 1; i <= blinks; i++) {
    digitalWrite(LEDPIN, HIGH);
    errorLeds(blinks);
    delay(500);
    digitalWrite(LEDPIN, LOW);
    allLedsOff();
    delay(500);
  }
}

boolean resetsystem(String by) {
//  if (PublishHandle != NULL) {
//    vTaskDelete(PublishHandle);
//  }
  DEBUG_PRINTLN("Rebooting....by: " + by);
  delay(5000);
  ESP.restart();

}


void PrintHex8(uint8_t In) // prints 8-bit data in hex with leading zeroes
{
  static char hex [] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' , 'A', 'B', 'C', 'D', 'E', 'F' };
  Serial.print ( hex[(In >> 4) & 0x0F]);
  Serial.print ( hex[In & 0x0F]);
}

void PrintHex16(uint16_t In) // prints 16-bit data in hex with leading zeroes
{
  static char hex [] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' , 'A', 'B', 'C', 'D', 'E', 'F' };
  Serial.print ( hex[(In >> 12) & 0x000F]);
  Serial.print ( hex[(In >> 8) & 0x000F]);
  Serial.print ( hex[(In >> 4) & 0x000F]);
  Serial.print ( hex[In & 0x000F]);
}
