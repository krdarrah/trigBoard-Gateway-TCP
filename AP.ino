const char* PARAM_MESSAGE = "message";
char monitorMessage[20];
char oldPacketReceived[200];

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void initAP() {
  WiFi.enableAP(true);
  Serial.println(WiFi.softAP("SSID", "password", 1, 1, 8)); //ssid,pw,ch,hid,conn
  //pw at least 8 char!
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);


  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "Gateway Running");
  });

  // Send a GET request to <IP>/trigBoard?message=<message>
  server.on("/trigBoard", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String message;
    char newPacket[100];
    //request->send(200, "text/plain", "OK");

    if (request->hasParam(PARAM_MESSAGE)) {
      message = request->getParam(PARAM_MESSAGE)->value();
      message.toCharArray(newPacket, message.length());
      request->send(200, "text/plain", "OK");

      if (strcmp(oldPacketReceived, newPacket) != 0) {
        duplicateCount = 0;
        strcpy(oldPacketReceived, newPacket);
        //sniff out for -#-
        const char delimiter[] = "-";
        char *pointerToFoundDash = strstr(newPacket, delimiter);//go find dash
        if (pointerToFoundDash != NULL) {//found a dash
          char parsedStrings[3][100];
          char *token =  strtok(newPacket, delimiter);
          strncpy(parsedStrings[0], token, sizeof(parsedStrings[0]));//first one
          token =  strtok(NULL, delimiter);
          strncpy(parsedStrings[1], token, sizeof(parsedStrings[1]));//second half - if exists
          token =  strtok(NULL, delimiter);
          strncpy(parsedStrings[2], token, sizeof(parsedStrings[1]));//last part
          //now going to recombine two strings
          strcat(parsedStrings[0], parsedStrings[2]);
          const char timeStampDelimiter[] = "$$$";//now strip off that $$$ and timestamp
          char *pointerToFoundDash = strstr(parsedStrings[0], timeStampDelimiter);//go find $$$
          if (pointerToFoundDash != NULL) {//found a dash
            //char parsedTimeStrings[2][50];
            char *tokenTime =  strtok(parsedStrings[0], timeStampDelimiter);
            strncpy(parsedStrings[0], tokenTime, sizeof(parsedStrings[0]));//first one
          }
          strcat(parsedStrings[0], "#");
          particle.print(parsedStrings[0]);//goes to particle
          Serial.println(parsedStrings[0]);//What was sent
          sprintf(monitorMessage, "#%s,%li", parsedStrings[1], millis());
          //Serial.printf("Message for Monitor: %s\n", monitorMessage);
        }
        //Serial.println(message);

      }
      else {
        Serial.printf("duplicate #%i\n", duplicateCount);
        duplicateCount++;
        if (duplicateCount > 5) { //means someone is trying to get through and we're not ACKing, something broken
          delay(100);
          ESP.restart();
        }
      }

    }
  });
  // Send a GET request to <IP>/trigBoard?message=<message>
  server.on("/monitor", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String message;

    if (request->hasParam(PARAM_MESSAGE)) {
      message = request->getParam(PARAM_MESSAGE)->value();
    } else {
      message = "No message sent";
    }
    Serial.print("Monitor from IP: ");
    Serial.println(message);
    if (digitalRead(silentButtonPin) == HIGH) {
      //Serial.printf("Sending to Monitor: %s\n", monitorMessage);
      request->send(200, "text/plain", monitorMessage);
    } else {
      message = "silence";
      //Serial.println("Monitor is Silenced");
      request->send(200, "text/plain", message);
    }

  });
  server.onNotFound(notFound);
  server.begin();
}
