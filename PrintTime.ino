//Serielle Ausgabe der aktuellen Zeit

void printDigits(int digits) {
  // utility for digital clock display: prints preceding colon and leading 0
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void PrintFormattedTime(time_t TimeToPrint) {
  tmElements_t TP;
  breakTime(TimeToPrint, TP);

  Serial.println("");
  if (!NTPGood)
    Serial.println("*** NTP nicht ok ***");
  else
    Serial.println("");

  Serial.print(dayStr(TP.Wday));
  Serial.print(", ");
  printDigits(TP.Day);
  Serial.print(". ");
  Serial.print(monthStr(TP.Month));
  Serial.print(" ");
  Serial.println(year(TimeToPrint));
  printDigits(TP.Hour);
  Serial.print(":");
  printDigits(TP.Minute);
  Serial.print(":");
  printDigits(TP.Second);
  if (LTZ.locIsDST(TimeToPrint))
    Serial.println(", Sommerzeit");
  else
    Serial.println(", Winterzeit");
}

void PrintIPInfo (void) {
  Serial.print(" IP-Adresse: ");
  Serial.println(WiFi.localIP());
  Serial.print("    Gateway: ");
  WiFi.gatewayIP().printTo(Serial);
  Serial.println();
  Serial.print("Subnet-Mask: ");
  WiFi.subnetMask().printTo(Serial);
  Serial.println();
  Serial.print(" DNS-Server: ");
  WiFi.dnsIP().printTo(Serial);
  Serial.println();

}
