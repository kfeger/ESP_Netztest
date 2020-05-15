/*
   Hier wird das WLAN verbunden
*/
void WiFiConnect (void) {
  WiFi.mode(WIFI_STA);
  WiFi.hostname(HOSTNAME);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi verbunden");
  PrintIPInfo();
}
