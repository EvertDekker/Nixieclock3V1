void IRAM_ATTR onTimer() {
  setcol(digitcount++);
  if (digitcount == 7) {
    digitcount = 1;
  }
}

void setTubeMode() {
  if (operatemode == true) {
    setTubeTime();
  }
  else
  {
    tubeTest(8);
  }
}

void tubeTest(byte number) {
  for (int i = 1; i < 7; i++) {
    digit[i] = number;
  }
}

void setTubeTime() {
  if (timeinfo.tm_sec == 19 | timeinfo.tm_sec == 20 | timeinfo.tm_sec == 21) //display date
  {
    digit[1] = ((timeinfo.tm_year - 100) % 10);
    digit[2] = ((timeinfo.tm_year - 100) / 10);
    digit[3] = ((timeinfo.tm_mon + 1) % 10);
    digit[4] = ((timeinfo.tm_mon + 1) / 10);
    digit[5] = (timeinfo.tm_mday % 10);
    digit[6] = (timeinfo.tm_mday / 10);

  }
  else
  {
    digit[1] = (timeinfo.tm_sec % 10);
    digit[2] = (timeinfo.tm_sec / 10);
    digit[3] = (timeinfo.tm_min % 10);
    digit[4] = (timeinfo.tm_min / 10);
    digit[5] = (timeinfo.tm_hour % 10);
    digit[6] = (timeinfo.tm_hour / 10);
  }
}

void StopTheTimer() {
  timerStop(timer);
  setcol(0);
}

/*
  Setting the coll. Because the ESP32 doesn't have port's it's not possible to shift the col, every GPIO have to be set individuale
*/
void  setcol(byte col) {
  switch (col) {
    case 0:                             //disable all col's (display off?)
      digitalWrite(SEC, false);
      digitalWrite(SEC_T, false);
      digitalWrite(MIN, false);
      digitalWrite(MIN_T, false);
      digitalWrite(HR, false);
      digitalWrite(HR_T, false);
      break;
    case 1:
      digitalWrite(HR_T, false);      //Previous col reset
      digitalWrite(SEC, true);        //new col set
      setrow(digit[1]);
      break;
    case 2:
      digitalWrite(SEC, false);
      digitalWrite(SEC_T, true);
      setrow(digit[2]);
      break;
    case 3:
      digitalWrite(SEC_T, false);
      digitalWrite(MIN, true);
      setrow(digit[3]);
      break;
    case 4:
      digitalWrite(MIN, false);
      digitalWrite(MIN_T, true);
      setrow(digit[4]);
      break;
    case 5:
      digitalWrite(MIN_T, false);
      digitalWrite(HR, true);
      setrow(digit[5]);
      break;
    case 6:
      digitalWrite(HR, false);
      digitalWrite(HR_T, true);
      setrow(digit[6]);
      break;
    default:
      // statements
      break;
  }
}

/*
  Setting the row. Because the ESP32 doesn't have port's every GPIO have to be set individuale
  BCD according this table http://www.tube-tester.com/sites/nixie/74141-NDT/74141-NDT.htm
*/
void setrow(byte digit) {
  digitalWrite(A, false);
  digitalWrite(B, false);
  digitalWrite(C, false);
  digitalWrite(D, false);
  switch (digit) {
    case 0:
      break;
    case 1:
      digitalWrite(A, true);
      break;
    case 2:
      digitalWrite(B, true);
      break;
    case 3:
      digitalWrite(A, true);
      digitalWrite(B, true);
      break;
    case 4:
      digitalWrite(C, true);
      break;
    case 5:
      digitalWrite(A, true);
      digitalWrite(C, true);
      break;
    case 6:
      digitalWrite(B, true);
      digitalWrite(C, true);
      break;
    case 7:
      digitalWrite(A, true);
      digitalWrite(B, true);
      digitalWrite(C, true);
      break;
    case 8:
      digitalWrite(D, true);
      break;
    case 9:
      digitalWrite(A, true);
      digitalWrite(D, true);
      break;
    default:
      // statements
      break;
  }

}
