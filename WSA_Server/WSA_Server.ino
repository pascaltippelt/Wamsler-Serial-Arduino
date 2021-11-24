#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <elapsedMillis.h>

#define ERR_CODE "ERROR"

#ifndef STASSID
#define STASSID "YOUR-SSID"
#define STAPSK  "YOUR-PASSWORD"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

elapsedMillis blinker = 0;

ESP8266WebServer server(80);

short retrys = 0;

String getValueSave(String request) {

  //clear Buffer
  while (Serial.read() >= 0) {
    ; // do nothing
  }

  //Write request
  Serial.write(0x1b);
  Serial.print(request);
  Serial.print('&');

  //get answere
  String ret =  "";
  Serial.setTimeout(1000);
  ret = Serial.readStringUntil('&');
  /* shall be: 0x1b 0000 0000 &
   * --> length = 10
   */
  if (ret.length() == 10) {
    //got a perfect answere
    return ret;
  } else if (ret.length() > 10) {
    //answere to long
    int das_und = ret.lastIndexOf('&');
    if (das_und > 9) {
      return ret.substring(das_und - 9);
    }
    return ERR_CODE;
  } else {
    //answere to short
    return ERR_CODE;
  }

}

String getStatus() {

  String ret = getValueSave("RD90005F");

  if (ret != ERR_CODE) {
    int sw = ret.substring(1,5).toInt();
    switch (sw)
            {
                case 0:
                    return "aus";
                case 101:
                    return "Anfeuern 1 (Lueften)";
                case 102:
                    return "Anfeuern 2 (Lade Holz)";
                case 103:
                    return "Anfeuern 3 (Feuer an)";
                case 104:
                    return "Anfeuern 4 (Feuer und Ventilator an)";
                case 201:
                    return "ein";
                case 401:
                    return "Schale reinigen";
                case 801:
                    return "Abkuehlen 1 (normal)";
                case 802:
                    return "Abkuehlen 1 (normal)";
                case 1001:
                    return "Abkuehlen 1 (eco)";
                case 1002:
                    return "Abkuehlen 1 (eco)";
                case 1003:
                    return "aus (eco)";
                default:
                    return "Error: " + String(sw);
            }        
    
  } else if (retrys <3) {
    retrys++;
    return getStatus();
  } else {
    return "Error: " + ret;
  }  
}

String getStatusBinary() {
  String ret = getValueSave("RD90005F");

  if (ret != ERR_CODE) {
    int sw = ret.substring(1,3).toInt();  
    switch (sw)
              {
                  case 0:
                      return "false";
                  case 8:
                      return "false";
                  case 10:
                      return "false";
                  case 1:
                      return "true";
                  case 2:
                      return "true";
                  case 4:
                      return "true";
                  default:
                      return "false";
  
              }
  } else if (retrys <3) {
    retrys++;
    return getStatusBinary();
  } else {
    return "false";
  }
}
String getTargetTemp() {
  String tmp_str = getValueSave("RC60005B");

  if (tmp_str != ERR_CODE) {
    return String(strtol(tmp_str.substring(1,5).c_str(),NULL,16));
  } else if (retrys < 3) {
    retrys++;
    return getTargetTemp();
  } else {
    return "-1";
  }
}

String getAmbientTemp() {

  String tmp_str = getValueSave("RD100057");

  if (tmp_str != ERR_CODE) {
    return String((float)strtol(tmp_str.substring(1,5).c_str(),NULL,16)/(float)10.0);
  } else if (retrys < 3) {
    retrys++;
    return getTargetTemp();
  } else {
    return "-1";
  }
}

String getPowerLevel() {
  String tmp_str = getValueSave("RC50005A");

  if (tmp_str != ERR_CODE) {
    return tmp_str.substring(4,5);
  } else if (retrys < 3) {
    retrys++;
    return getTargetTemp();
  } else {
    return "-1";
  }
}


void handleRoot() {
  server.send(200, "text/html", "More Soon ;-)");
}

void handleGet() {
    for (uint8_t i=0; i<server.args(); i++){
      if (server.argName(i)=="s") { server.send(200, "text/plain", getStatus()); }
      if (server.argName(i)=="sb") { server.send(200, "text/plain", getStatusBinary()); }
      if (server.argName(i)=="tt") { server.send(200, "text/plain", getTargetTemp()); }
      if (server.argName(i)=="ct") { server.send(200, "text/plain", getAmbientTemp()); }
      if (server.argName(i)=="pl") { server.send(200, "text/plain", getPowerLevel()); }
    }
}

String generatePowerLevelTelegram(int power)
        {
          if (power < 0) {power = 0;}
          if (power > 5) {power = 5;}
          
          String xx = String(power,HEX);
          while (xx.length() < 2) { xx = "0" + xx; }
          String yy = String(power+88,HEX);
          while (yy.length() < 2) { yy = "0" + yy; }
          return "RF0" + xx + "0" + yy;
        }
        
String generateTemperatureTelegram(int temperature)
        {
            String xx = String(temperature,HEX);
            while (xx.length() < 2) { xx = "0" + xx; }
            String yy = String(temperature+75,HEX);
            while (yy.length() < 2) { yy = "0" + yy; }
            return "RF2" + xx + "0" + yy;
        }

void handleSet() {
    for (uint8_t i=0; i<server.args(); i++){
      if (server.argName(i)=="pl") { 
        server.send(200, "text/plain", getValue(generatePowerLevelTelegram(server.arg(i).toInt())));
        return;
        }
      if (server.argName(i)=="tt") { 
        server.send(200, "text/plain", getValue(generateTemperatureTelegram(server.arg(i).toInt()))); 
        return;
        }
      if (server.argName(i)=="on") { 
        server.send(200, "text/plain", getValue(generatePowerLevelTelegram(4))); 
        return;
        }
      if (server.argName(i)=="off") { 
        server.send(200, "text/plain", getValue(generatePowerLevelTelegram(0))); 
        return;
        }
    }
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(5000);
    ESP.restart();
  }


  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.on("/get", handleGet);
  server.onNotFound(handleNotFound);
  server.begin();
  pinMode(2,OUTPUT);
  digitalWrite(2,0);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

void loop() {
  try {
  server.handleClient();
  } catch (...) {
    server.begin();
  }
  
  retrys = 0;

  if (blinker > 1000) {
    blinker = 0;
    digitalWrite(2,!digitalRead(2));
  }  
}
