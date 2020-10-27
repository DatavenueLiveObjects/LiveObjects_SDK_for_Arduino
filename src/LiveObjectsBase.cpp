
#include "LiveObjectsBase.h"
LiveObjectsBase::LiveObjectsBase()
    :
     lastKeepAliveNetwork(5000)
    ,m_sPayload()
    ,m_sDecoder()
    ,m_Security(NONE)
    ,m_bDebug(false)
    ,m_bInitialized(false)
    ,m_bInitialMqttConfig(false)
    ,m_bCertLoaded(false)
    ,m_bSubCMD(false)
{}


/******************************************************************************
   PARAM TYPERS
 ******************************************************************************/

void LiveObjectsBase::paramTyper(const String& name, bool* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, UNSIGNED_INTEGER, T_BOOL, callback);
  else
    addTypedParam(name, variable, type, T_BOOL, callback);
}
void LiveObjectsBase::paramTyper(const String& name, char* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, INTEGER, T_CHAR, callback);
  else
    addTypedParam(name, variable, type, T_CHAR, callback);
}
#if not defined ESP8266 && not defined ESP32 && not defined ARDUINO_AVR_FEATHER32U4
void LiveObjectsBase::paramTyper(const String& name, int* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, INTEGER, T_INT, callback);
  else
    addTypedParam(name, variable, type, T_INT, callback);
}
void LiveObjectsBase::paramTyper(const String& name, unsigned int* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, UNSIGNED_INTEGER, T_UINT, callback);
  else
    addTypedParam(name, variable, type, T_UINT, callback);
}
#endif
void LiveObjectsBase::paramTyper(const String& name, int8_t*variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, INTEGER, T_INT8, callback);
  else
    addTypedParam(name, variable, type, T_INT8, callback);
}
void LiveObjectsBase::paramTyper(const String& name, int16_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, INTEGER, T_INT16, callback);
  else
    addTypedParam(name, variable, type, T_INT16, callback);
}
void LiveObjectsBase::paramTyper(const String& name, int32_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, INTEGER, T_INT32, callback);
  else
    addTypedParam(name, variable, type, T_INT32, callback);
}
void LiveObjectsBase::paramTyper(const String& name, uint8_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, UNSIGNED_INTEGER, T_UINT8, callback);
  else
    addTypedParam(name, variable, type, T_UINT8, callback);
}
void LiveObjectsBase::paramTyper(const String& name, uint16_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, UNSIGNED_INTEGER, T_UINT16, callback);
  else
    addTypedParam(name, variable, type, T_UINT16, callback);
}
void LiveObjectsBase::paramTyper(const String& name, uint32_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, UNSIGNED_INTEGER, T_UINT32, callback);
  else
    addTypedParam(name, variable, type, T_UINT32, callback);
}
void LiveObjectsBase::paramTyper(const String& name, double* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, DECIMAL, T_DOUBLE, callback);
  else
    addTypedParam(name, variable, type, T_DOUBLE, callback);
}
void LiveObjectsBase::paramTyper(const String& name, float* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, DECIMAL, T_FLOAT, callback);
  else
    addTypedParam(name, variable, type, T_FLOAT, callback);
}
void LiveObjectsBase::paramTyper(const String& name, String* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, STRING, T_STRING, callback);
  else
    addTypedParam(name, variable, type, T_STRING, callback);
}
void LiveObjectsBase::ptrTyper(const LiveObjects_parameter param, const JsonDocument& configIn, JsonDocument& configOut) 
{
  switch (param.variableType) {
    case T_BOOL:
      updateParameter(param, (bool*)param.value, configIn, configOut);
      break;
    case T_CHAR:
      updateParameter(param, (char*)param.value, configIn, configOut);
      break;
    #ifndef ESP8266
    case T_INT:
      updateParameter(param, (int*)param.value, configIn, configOut);
      break;
    case T_UINT:
      updateParameter(param, (unsigned int*)param.value, configIn, configOut);
      break;
    #endif
    case T_INT8:
      updateParameter(param, (int8_t*)param.value, configIn, configOut);
      break;
    case T_INT16:
      updateParameter(param, (int16_t*)param.value, configIn, configOut);
      break;
    case T_INT32:
      updateParameter(param, (int32_t*)param.value, configIn, configOut);
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
      *ptr = configIn[JSONCFG][param.label][JSONCFGVALUE].as<String>();
      configOut[JSONCFG][param.label][JSONCFGVALUE] = (*ptr).c_str();
      break;
  }
}


/******************************************************************************
   CONFIGURATION MANAGER
 ******************************************************************************/

void LiveObjectsBase::configurationManager(String topic, int messageSize) {
  StaticJsonDocument<PAYLOAD_DATA_SIZE> configOut;
  if (messageSize >= 0)
  { // config update received
    StaticJsonDocument<PAYLOAD_DATA_SIZE> configIn;
    deserializeMessage(configIn);

    messageDebug(topic,configIn, true);

    configOut=configIn;
    JsonObject obj =  configOut[JSONCFG];
    LiveObjects_parameter tmp("",nullptr,BINR,T_BOOL,nullptr);
    for(auto el : obj)
    { 
      tmp.label = el.key().c_str();
      int index = parameters.find(&tmp);
      if(index > -1)
      {
        LiveObjects_parameter* p = parameters[index];
        ptrTyper(*p, configIn, configOut);
        if (p->callback != nullptr) p->callback();
      }
      else
      {
        configOut["cfg"][el.key()]["v"]="Nan";
        configOut["cfg"][el.key()]["t"]="str";
      }
    }
    publishMessage(MQTT_PUBCFG, configOut);
  }
  else if(parameters.size()>0 && messageSize == -1)
  { // messageSize==-1, compose & send initial config
    StaticJsonDocument<0> configIn;
    for (uint8_t i = 0; i < parameters.size(); i++) {
      switch (parameters[i]->type) {
        case INTEGER:
          configOut[JSONCFG][parameters[i]->label.c_str()][JSONCFGTYPE] = F("i32");
          break;
        case UNSIGNED_INTEGER:
          configOut[JSONCFG][parameters[i]->label.c_str()][JSONCFGTYPE] = F("u32");
          break;
        case STRING:
          configOut[JSONCFG][parameters[i]->label.c_str()][JSONCFGTYPE] = F("str");
          break;
        case BINR:
          configOut[JSONCFG][parameters[i]->label.c_str()][JSONCFGTYPE] = F("bin");
          break;
        case DECIMAL:
          configOut[JSONCFG][parameters[i]->label.c_str()][JSONCFGTYPE] = F("f64");
          break;
        case IMPLICIT:
          break;
      }
      ptrTyper(*parameters[i], configIn, configOut);
    }
    publishMessage(MQTT_PUBCFG, configOut);
  }
}

/******************************************************************************
   COMMAND MANAGEMENT
 ******************************************************************************/

void LiveObjectsBase::addCommand(const String name, onCommandCallback callback) {
  if(!commands.push(new LiveObjects_command(name, callback))) outputDebug(ERR, "Command ",name," already exists, skipping....");
}

void LiveObjectsBase::commandManager(String topic) {
  StaticJsonDocument<PAYLOAD_DATA_SIZE> cmdIn;
  StaticJsonDocument<PAYLOAD_DATA_SIZE> cmdOut;
  deserializeMessage(cmdIn);
  messageDebug(topic, cmdIn);
  cmdOut[JSONCID] = cmdIn[JSONCID]; 
  for (uint8_t i = 0; i < commands.size(); i++) // only with MQTT or SMS !!
    if (cmdIn[F("req")] == commands[i]->label) {
      String response;
      commands[i]->callback(cmdIn[F("arg")].as<String>(), response);
      if (response.length() != 0)
        cmdOut[F("res")] = serialized(response);
      break;
    }
  publishMessage(MQTT_PUBCMD, cmdOut);
}

/******************************************************************************
   CONNECTION MANAGER
 ******************************************************************************/

void LiveObjectsBase::connect()
{
  #ifdef PMIC_PRESENT
  batteryBegin();
  #endif
  connectNetwork();
  if(m_Protocol == MQTT) connectMQTT();
  networkStatus = CONNECTED;
}

void LiveObjectsBase::networkCheck() {
  unsigned long now = millis();
  if (now - lastKeepAliveNetwork > KEEP_ALIVE_NETWORK) {
    checkNetwork();
    if(m_Protocol == MQTT) checkMQTT();
    lastKeepAliveNetwork = now;
  }
}

void LiveObjectsBase::disconnect() {
  if(m_Protocol == MQTT) disconnectMQTT();
  disconnectNetwork();
  outputDebug(INFO,"Offline.");
  networkStatus = DISCONNECTED;
}

/******************************************************************************
   MQTT FUNCTIONS
 ******************************************************************************/

void LiveObjectsBase::sendData() {

  if(m_Protocol == MQTT)
  {
    if(m_Encoding==TEXT)
    {
      easyDataPayload[JSONMODEL] = JSONMODELNAME;
      publishMessage(m_sTopic, easyDataPayload);
      easyDataPayload.clear();
    }
    else publishMessage(m_sTopic+m_sDecoder, m_sPayload);
  }
  else publishMessage(m_sTopic+m_sDecoder,m_sPayload);

  clearPayload();
}


void LiveObjectsBase::sendData(const String customPayload) {
  if(m_Protocol == MQTT && m_Encoding == TEXT)
  {
    StaticJsonDocument<PAYLOAD_DATA_SIZE> payload;
    deserializeJson(payload, customPayload);
    if (!payload.containsKey(JSONMODEL)) payload[JSONMODEL] = JSONMODELNAME;
    publishMessage(m_sTopic, payload);
  }
  else publishMessage(m_sTopic, const_cast<String&>(customPayload));
  clearPayload();
}


void LiveObjectsBase::messageDebug(String& topic, JsonDocument& doc, bool r)
{
  if(!r)outputDebug(INFO,"Publishing message on topic: ", topic);
  else outputDebug(INFO,"Received message on topic: ", topic);
  if(m_bDebug)
  {
  serializeJsonPretty(doc, Serial);
  outputDebug(TXT);
  }
}

void LiveObjectsBase::publishMessage(const String& topic, JsonDocument& payload) {
  messageDebug(const_cast<String&>(topic), payload);

  if( measureJson(payload) >= PAYLOAD_DATA_SIZE )
  {
    outputDebug(ERR,"Message size is to big, aborting send command.");
    return;
  }

  sendMQTT(const_cast<String&>(topic),payload);
}
void LiveObjectsBase::publishMessage(const String& topic, String& payload) {
  outputDebug(INFO,"Publishing message on topic: ", topic);
  outputDebug(INFO, payload);
  if( payload.length() >= PAYLOAD_DATA_SIZE )
  {
    outputDebug(ERR,"Message size is to big, aborting send command.");
    return;
  }
  sendMQTT(const_cast<String&>(topic), payload);
}

/******************************************************************************
   LIVE OBJECTS
 ******************************************************************************/
void LiveObjectsBase::begin(Protocol p, Encoding e, bool d)
{
  m_Protocol = p;
  m_Encoding = e;
  m_bDebug = d;
  if(e==TEXT) m_sTopic = MQTT_PUBDATA;
  else m_sTopic = MQTT_PUBDATA_BINARY;
  if(p==MQTT)
  {
    stopMQTT();
  }
}

void LiveObjectsBase::loop() {
  if (networkStatus == CONNECTED) {
    networkCheck();
    //if(m_Protocol == MQTT) m_pMqttclient->poll();
  }
}
void LiveObjectsBase::changeConfiguration(Protocol p,Security s, Encoding mode)
{
  m_Security=s;
  if(p!= m_Protocol || mode != m_Encoding)
  {
    m_bInitialized = false;
    m_bInitialMqttConfig = false;
    m_bSubCMD=false;
    begin(p,mode,m_bDebug);
    connect();
  }
}
void LiveObjectsBase::setSecurity(Security s)
{
  m_Security = s;
}
void LiveObjectsBase::enableDebug(bool b)
{
  m_bDebug = b;
}
void LiveObjectsBase::setClientID(const String id)
{
  m_sMqttid = id;
}

void LiveObjectsBase::setDecoder(String s)
{
  m_sDecoder = "/";
  m_sDecoder += s;
}

void LiveObjectsBase::addTimestamp(time_t timestamp)
{
  #ifndef ARDUINO_ARCH_AVR
  char bufer[sizeof("2011-10-08T07:07:09Z")];
  strftime(bufer, sizeof(bufer), "%Y-%m-%dT%H:%M:%SZ",gmtime(&timestamp));
  if(m_Protocol == MQTT && m_Encoding==TEXT) easyDataPayload["timestamp"]=bufer;
  else
  {
    String s = bufer;
    addToStringPayload(s);
  }
  #endif
}
void LiveObjectsBase::addLocation(double lat, double lon, double alt)
{
  if(m_Protocol == MQTT && m_Encoding==TEXT) addToPayload(easyDataPayload.createNestedObject("location"),"lat",lat,"lon",lon,"alt",alt);
  else addToStringPayload(lat,lon,alt);
}

void LiveObjectsBase::clearPayload()
{
  easyDataPayload.clear();
  m_sPayload="";
}

void LiveObjectsBase::addObjectToPayload(String name, JsonObject& obj)
{
  JsonObject tmp = easyDataPayload[JSONVALUE].createNestedObject(name);
  for( JsonPair item : obj)
    tmp[item.key().c_str()] = item.value();
}

bool LiveObjectsBase::debugEnabled()
{
  return m_bDebug;
}