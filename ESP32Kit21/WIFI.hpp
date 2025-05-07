//***** CONFIGURACION WIFI *******//
const char* ssid = "IOTNet";
const char* password = "CSC#Jesuitas";
const char* hostname = "esp21";
IPAddress ip(192, 168, 7, 67);
IPAddress gateway(192, 168, 0, 99);
IPAddress subnet(255, 255, 240, 0);
void ConnectWiFi_STA(bool useStaticIP) {
  Serial.println("");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (useStaticIP) {WiFi.config(ip, gateway, subnet);}
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print('.');
  }
  Serial.println("");
  Serial.print("Iniciado STA:\t");
  Serial.println(ssid);
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
}