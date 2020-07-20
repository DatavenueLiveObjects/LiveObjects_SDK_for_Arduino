#include "LiveObjects.h"

LiveObjects::LiveObjects()
    :
     mqttClient(nbClient)
{}



/******************************************************************************
   PARAM TYPERS
 ******************************************************************************/

void LiveObjects::paramTyper(const char* name, bool* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, UNSIGNED_INTEGER, T_BOOL, callback);
  else
    addTypedParam(name, variable, type, T_BOOL, callback);
}
void LiveObjects::paramTyper(const char* name, char* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, INTEGER, T_CHAR, callback);
  else
    addTypedParam(name, variable, type, T_CHAR, callback);
}
void LiveObjects::paramTyper(const char* name, int* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, INTEGER, T_INT, callback);
  else
    addTypedParam(name, variable, type, T_INT, callback);
}
void LiveObjects::paramTyper(const char* name, int8_t*variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, INTEGER, T_INT8, callback);
  else
    addTypedParam(name, variable, type, T_INT8, callback);
}
void LiveObjects::paramTyper(const char* name, int16_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, INTEGER, T_INT16, callback);
  else
    addTypedParam(name, variable, type, T_INT16, callback);
}
void LiveObjects::paramTyper(const char* name, int32_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, INTEGER, T_INT32, callback);
  else
    addTypedParam(name, variable, type, T_INT32, callback);
}
void LiveObjects::paramTyper(const char* name, unsigned int* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, UNSIGNED_INTEGER, T_UINT, callback);
  else
    addTypedParam(name, variable, type, T_UINT, callback);
}
void LiveObjects::paramTyper(const char* name, uint8_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, UNSIGNED_INTEGER, T_UINT8, callback);
  else
    addTypedParam(name, variable, type, T_UINT8, callback);
}
void LiveObjects::paramTyper(const char* name, uint16_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, UNSIGNED_INTEGER, T_UINT16, callback);
  else
    addTypedParam(name, variable, type, T_UINT16, callback);
}
void LiveObjects::paramTyper(const char* name, uint32_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, UNSIGNED_INTEGER, T_UINT32, callback);
  else
    addTypedParam(name, variable, type, T_UINT32, callback);
}
void LiveObjects::paramTyper(const char* name, double* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, DECIMAL, T_DOUBLE, callback);
  else
    addTypedParam(name, variable, type, T_DOUBLE, callback);
}
void LiveObjects::paramTyper(const char* name, float* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, DECIMAL, T_FLOAT, callback);
  else
    addTypedParam(name, variable, type, T_FLOAT, callback);
}
void LiveObjects::paramTyper(const char* name, String* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, STRING, T_STRING, callback);
  else
    addTypedParam(name, variable, type, T_STRING, callback);
}

void LiveObjects::ptrTyper(const LiveObjects_parameter param, const JsonDocument& configIn, JsonDocument& configOut) {
  switch (param.variableType) {
    case T_BOOL:
      updateParameter(param, (bool*)param.value, configIn, configOut);
      break;
    case T_CHAR:
      updateParameter(param, (char*)param.value, configIn, configOut);
      break;
    case T_INT:
      updateParameter(param, (int*)param.value, configIn, configOut);
      break;
    case T_INT8:
      updateParameter(param, (int8_t*)param.value, configIn, configOut);
      break;
    case T_INT16:
      updateParameter(param, (int16_t*)param.value, configIn, configOut);
      break;
    case T_INT32:
      updateParameter(param, (int32_t*)param.value, configIn, configOut);
      break;
    case T_UINT:
      updateParameter(param, (unsigned int*)param.value, configIn, configOut);
      break;
    case T_UINT8:
      updateParameter(param, (uint8_t*)param.value, configIn, configOut);
      break;
    case T_UINT16:
      updateParameter(param, (uint16_t*)param.value, configIn, configOut);
      break;
    case T_UINT32:
      updateParameter(param, (uint32_t*)param.value, configIn, configOut);
      break;
    case T_DOUBLE:
      updateParameter(param, (double*)param.value, configIn, configOut);
      break;
    case T_FLOAT:
      updateParameter(param, (float*)param.value, configIn, configOut);
      break;
    case T_STRING:
      String* ptr = (String*)param.value;
      if (!configIn.isNull())
      *ptr = configIn[JSONcfg][param.label][JSONcfgValue].as<String>();
      configOut[JSONcfg][param.label][JSONcfgValue] = (*ptr).c_str();
      break;
  }
}


/******************************************************************************
   CONFIGURATION MANAGER
 ******************************************************************************/

void LiveObjects::configurationManager(int messageSize) {
  StaticJsonDocument<PAYLOAD_DEVMGT_SIZE> configOut;
  if (messageSize >= 0) { // config update received
    StaticJsonDocument<PAYLOAD_DEVMGT_SIZE> configIn;
    deserializeJson(configIn, mqttClient);

    serializeJsonPretty(configIn, Serial);
    Serial.print('\n');

    configOut = configIn;
    for (uint8_t i = 0; i < paramNb; i++)
      if (configIn[JSONcfg].containsKey(parameters[i].label)) {
        ptrTyper(parameters[i], configIn, configOut);
        if (parameters[i].callback != NULL)
          parameters[i].callback();
      }
    publishMessage(mqtt_pubcfg, configOut);
  }
  else if (paramNb > 0) { // messageSize==-1, compose & send initial config
    StaticJsonDocument<0> configIn;
    for (uint8_t i = 0; i < paramNb; i++) {
      switch (parameters[i].type) {
        case INTEGER:
          configOut[JSONcfg][parameters[i].label][JSONcfgType] = F("i32");
          break;
        case UNSIGNED_INTEGER:
          configOut[JSONcfg][parameters[i].label][JSONcfgType] = F("u32");
          break;
        case STRING:
          configOut[JSONcfg][parameters[i].label][JSONcfgType] = F("str");
          break;
        case BINARY:
          configOut[JSONcfg][parameters[i].label][JSONcfgType] = F("bin");
          break;
        case DECIMAL:
          configOut[JSONcfg][parameters[i].label][JSONcfgType] = F("f64");
          break;
      }
      ptrTyper(parameters[i], configIn, configOut);
    }
    publishMessage(mqtt_pubcfg, configOut);
  }
}

/******************************************************************************
   COMMAND MANAGEMENT
 ******************************************************************************/

void LiveObjects::addCommand(const char* name, onCommandCallback callback) {
  commands[cmdNb++] = (LiveObjects_command) {
    name, callback
  };
}

void LiveObjects::commandManager() {
  StaticJsonDocument<PAYLOAD_DEVMGT_SIZE> cmdIn;
  StaticJsonDocument<PAYLOAD_DEVMGT_SIZE> cmdOut;
  deserializeJson(cmdIn, mqttClient);

  serializeJsonPretty(cmdIn, Serial);
  Serial.print('\n');

  for (uint8_t i = 0; i < cmdNb; i++) // only with MQTT or SMS !!
    if (cmdIn[F("req")] == commands[i].label) {
      cmdOut[JSONcid] = cmdIn[JSONcid];
      String response;
      commands[i].callback(cmdIn[F("arg")].as<String>(), response);
      if (response.length() != 0)
        cmdOut[F("res")] = serialized(response);
      break;
    }
  publishMessage(mqtt_pubcmd, cmdOut);
}

/******************************************************************************
   CONNECTION MANAGER
 ******************************************************************************/

void LiveObjects::connect() {
  if (!initialConfigDone) {
    NBModem modem;
    if(modem.begin()) {
      String imei="";
      for(int i=1;i<=3;i++){
        imei=modem.getIMEI();
        if(imei.length()!=0) break;
        delay(100*i);
      }
      strncpy(mqtt_id, imei.c_str(), MQTT_CLIENT_ID_LENGTH);
    }
    else{
      strncpy(mqtt_id, "MKR_1500_NB", MQTT_CLIENT_ID_LENGTH);
    }

  }
  Serial.print("Connecting to cellular network");
  while (nbAccess.begin(SECRET_PINNUMBER, SECRET_APN, SECRET_APN_USER, SECRET_APN_PASS) != NB_READY)
    Serial.print(".");
  Serial.println("\nYou're connected to the network");

  #if defined MQTT_TLS    // only for TLS ; loads DigiCert CA into SARA module
    if (!DigiCert_rootCA_loaded) {
      Serial.println("Loading DigiCert Root CA certificate");
      MODEM.sendf("AT+USECMNG=0,0,\"%s\",%d", LO_ROOT_CERT.name, LO_ROOT_CERT.size);
      if (MODEM.waitForPrompt() != 1) {
        Serial.print("Problem loading certificate!\nStopping here.");
        while (1) ;
      }
      else {
        MODEM.write(LO_ROOT_CERT.data, LO_ROOT_CERT.size);
        int ready;
        while (!MODEM.ready()) ;
        DigiCert_rootCA_loaded = true;
        Serial.println("Certificate loaded");
      }
    }
  #endif

  if (sizeof(SECRET_LIVEOBJECTS_API_KEY) != 33) // check if API key seems correct
  {
    Serial.print("Please check your Live Objects API key (arduino_secrets.h file).\nStopping here.");
    while (1) ;
  }

  if (!initialConfigDone) {
    mqttClient.setId(mqtt_id);
    mqttClient.setUsernamePassword(mqtt_user, SECRET_LIVEOBJECTS_API_KEY);
    mqttClient.onMessage(onMQTTmessage);
  }

  Serial.print("Connecting to MQTT broker '");
  Serial.print(mqtt_broker);
  Serial.print("'");

  while (!mqttClient.connect(mqtt_broker, mqtt_port))
    Serial.print(".");
  Serial.println("\nYou're connected to the MQTT broker");

  networkStatus = CONNECTED;

  if (paramNb > 0)
    mqttClient.subscribe(mqtt_subcfg);
  if (cmdNb > 0)
  mqttClient.subscribe(mqtt_subcmd);

  if (!initialConfigDone) {
    configurationManager();
    initialConfigDone = true;
  }

  mqttClient.poll();
}

void LiveObjects::networkCheck() {
  unsigned long now = millis();
  if (now - lastKeepAliveNetwork > KEEP_ALIVE_NETWORK) {
    if (nbAccess.status() != NB_READY || !mqttClient.connected())
        connect();
    lastKeepAliveNetwork = now;
  }
}

void LiveObjects::disconnect() {
  Serial.println("\nClosing MQTT connection...");
  mqttClient.stop();
  Serial.println("Disconnecting from cellular network...");
  nbAccess.shutdown();
  Serial.println("Offline.\n");
  networkStatus = DISCONNECTED;
}

/******************************************************************************
   MQTT FUNCTIONS
 ******************************************************************************/

void LiveObjects::onMQTTmessage(int messageSize) {
  LiveObjects& lo = LiveObjects::get();
  String topic = lo.mqttClient.messageTopic();
  Serial.print("Received a message on topic '");
  Serial.print(topic);
  Serial.println("':");

  if (topic == lo.mqtt_subcfg)
    lo.configurationManager(messageSize);
  else if (topic == lo.mqtt_subcmd)
    lo.commandManager();
}

void LiveObjects::sendData() {
  easyDataPayload[JSONmodel] = JSONmodelName;
  publishMessage(mqtt_pubdata, easyDataPayload);
  easyDataPayload.clear();
}

void LiveObjects::sendData(const String customPayload) {
  StaticJsonDocument<PAYLOAD_DATA_SIZE> payload;
  deserializeJson(payload, customPayload);
  if (!payload.containsKey(JSONmodel))
    payload[JSONmodel] = JSONmodelName;
  publishMessage(mqtt_pubdata, payload);
}

void LiveObjects::publishMessage(const char* topic, JsonDocument& payload) {
  LiveObjects_networkStatus lastStatus = networkStatus;
  if (networkStatus != CONNECTED)
        connect();

  Serial.print("Publishing message on topic '");
  Serial.print(topic);
  Serial.println("':");
  serializeJsonPretty(payload, Serial);
  Serial.print('\n');
  mqttClient.beginMessage(topic);
  serializeJson(payload, mqttClient);
  mqttClient.endMessage();

  if (lastStatus == DISCONNECTED)
        disconnect();
}

/******************************************************************************
   LIVE OBJECTS
 ******************************************************************************/

void LiveObjects::loop() {
  if (networkStatus == CONNECTED) {
    networkCheck();
    mqttClient.poll();
  }
}
