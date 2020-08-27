#include "LiveObjects.h"


LiveObjectsBase::LiveObjectsBase()
    :
     m_bDebug(false)
    ,m_bInitialized(false)
    ,m_bInitialMqttConfig(false)
    ,m_bCertLoaded(false)
    ,m_pClient(nullptr)
    ,m_pMqttclient(nullptr)
    ,m_sPayload()
    ,m_bSubCMD(false)
    ,lastKeepAliveNetwork(10000)
    ,m_Security(NONE)
    ,m_sDecoder()
{
}
LiveObjectsBase::~LiveObjectsBase()
{
  if(m_pMqttclient != nullptr)delete m_pMqttclient;
  if(m_pClient != nullptr)delete m_pClient;
}


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
void LiveObjectsBase::paramTyper(const String& name, int* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, INTEGER, T_INT, callback);
  else
    addTypedParam(name, variable, type, T_INT, callback);
}
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
void LiveObjectsBase::paramTyper(const String& name, unsigned int* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, UNSIGNED_INTEGER, T_UINT, callback);
  else
    addTypedParam(name, variable, type, T_UINT, callback);
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
void LiveObjectsBase::ptrTyper(const LiveObjects_parameter param, const JsonDocument& configIn, JsonDocument& configOut) {
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
      *ptr = configIn[JSONCFG][param.label][JSONCFGVALUE].as<String>();
      configOut[JSONCFG][param.label][JSONCFGVALUE] = (*ptr).c_str();
      break;
  }
}


/******************************************************************************
   CONFIGURATION MANAGER
 ******************************************************************************/

void LiveObjectsBase::configurationManager(int messageSize) {
  StaticJsonDocument<PAYLOAD_DATA_SIZE> configOut;
  if (messageSize >= 0)
  { // config update received
    StaticJsonDocument<PAYLOAD_DATA_SIZE> configIn;
    deserializeJson(configIn, *m_pMqttclient);

    if(m_bDebug)
    {
    serializeJsonPretty(configIn, Serial);
    outputDebug(TXT);
    }
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
        if (p->callback != NULL) p->callback();
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

void LiveObjectsBase::commandManager() {
  StaticJsonDocument<PAYLOAD_DATA_SIZE> cmdIn;
  StaticJsonDocument<PAYLOAD_DATA_SIZE> cmdOut;
  deserializeJson(cmdIn, *m_pMqttclient);
  cmdOut[JSONCID] = cmdIn[JSONCID]; 
  if(m_bDebug)
  {
  serializeJsonPretty(cmdIn, Serial);
  outputDebug(TXT);
  }
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

void LiveObjectsBase::onMQTTmessage(int messageSize) {
  String topic = m_pMqttclient->messageTopic();
  outputDebug(INFO,"Received a message on topic: ", topic);

  if (topic == MQTT_SUBCFG)
    configurationManager(messageSize);
  else if (topic == MQTT_SUBCMD)
    commandManager();
}

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

void LiveObjectsBase::publishMessage(const String& topic, JsonDocument& payload) {
  outputDebug(INFO,"Publishing message on topic: ", topic);
  if(m_bDebug)
  {
  serializeJsonPretty(payload, Serial);
  outputDebug(TXT);
  }
  if( measureJson(payload) >= PAYLOAD_DATA_SIZE )
  {
    outputDebug(ERR,"Message size is to big, aborting send command.");
    return;
  }

  m_pMqttclient->beginMessage(topic);
  serializeJson(payload, *m_pMqttclient);
  m_pMqttclient->endMessage();
}
void LiveObjectsBase::publishMessage(const String& topic, String& payload) {
  outputDebug(INFO,"Publishing message on topic: ", topic);
  outputDebug(INFO, payload);
  if( payload.length() >= PAYLOAD_DATA_SIZE )
  {
    outputDebug(ERR,"Message size is to big, aborting send command.");
    return;
  }
  m_pMqttclient->beginMessage(topic);
  for(int i=0;i<m_sPayload.length();i+=2)
    m_pMqttclient->write(hexBinary(m_sPayload[i],m_sPayload[i+1]));
  m_pMqttclient->endMessage();
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
    if(m_pMqttclient!= nullptr)
    {
      m_pMqttclient->stop();
      delete m_pMqttclient;
      m_pMqttclient=nullptr;
    }
  }
}

void LiveObjectsBase::loop() {
  if (networkStatus == CONNECTED) {
    networkCheck();
    if(m_Protocol == MQTT) m_pMqttclient->poll();
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
  char bufer[sizeof("2011-10-08T07:07:09Z")];
  strftime(bufer, sizeof(bufer), "%Y-%m-%dT%H:%M:%SZ",gmtime(&timestamp));
  if(m_Protocol == MQTT && m_Encoding==TEXT) easyDataPayload["timestamp"]=bufer;
  else
  {
    String s = bufer;
    addToStringPayload(s);
  }
  
    
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

bool LiveObjectsBase::debugEnabled()
{
  return m_bDebug;
}

void LiveObjectsBase::checkMQTT()
{
  if(!m_bSubCMD) if(commands.size()>0) m_pMqttclient->subscribe(MQTT_SUBCMD);
  if(!m_pMqttclient->connected())
    connectMQTT();
}
void LiveObjectsBase::connectMQTT()
{
  if (SECRET_LIVEOBJECTS_API_KEY.length() != 32) // check if API key seems correct
  {
    outputDebug(ERR,"Please check your Live Objects API key (arduino_secrets.h file).\nStopping here.");
    while (1) ;
  }

  if (!m_bInitialMqttConfig) {
    m_pMqttclient->setTxPayloadSize(PAYLOAD_DATA_SIZE);
    m_pMqttclient->setId(m_sMqttid.c_str());
    m_pMqttclient->setUsernamePassword(MQTT_USER, SECRET_LIVEOBJECTS_API_KEY);
  }

  outputDebug(INFO,"Connecting to MQTT broker ",MQTT_BROKER,":",m_nPort);


  while (!m_pMqttclient->connect(MQTT_BROKER, m_nPort)) outputDebug(TXT,".");
  outputDebug(INFO,"You're connected to the MQTT broker");

  if(parameters.size()>0)m_pMqttclient->subscribe(MQTT_SUBCFG);
  if(!m_bSubCMD && commands.size()>0) m_pMqttclient->subscribe(MQTT_SUBCMD);

  m_pMqttclient->poll();

  if (!m_bInitialMqttConfig) {
    configurationManager();
    m_bInitialMqttConfig = true;
  }

}
void LiveObjectsBase::disconnectMQTT()
{
  outputDebug(INFO,"Closing MQTT connection...");
  m_pMqttclient->stop();
}

void LiveObjectsBase::addPowerStatus()
{
  #ifdef PMIC_PRESENT
  byte DATA = readRegister(SYSTEM_STATUS_REGISTER);
  bool charging = (DATA & ((1<<5)|(1<<4)))!=0;
  bool bat = (DATA & (1<<0)) == 0;
  bool power=(DATA & (1<<2));
  double voltage=0.;
  // outputDebug(INFO,  DATA&0b11000000u);
  // outputDebug(INFO,  DATA);
  // outputDebug(INFO,  power);
  if(bat) voltage = analogRead(ADC_BATTERY) * (4.3 / 1023.0);
  if(m_Protocol == MQTT && m_Encoding==TEXT)
  {
    addToPayload(easyDataPayload[JSONVALUE].createNestedObject("powerStatus"),
                 "external_power",(charging? true : ((!bat && power)?true:power))
                ,"battery_connected",bat 
                ,"battery_charging", charging
                ,"battery_voltage",voltage);
  }
  else addToStringPayload((charging ? true : (!bat && power ? true : power)), bat ,charging, ( bat ? voltage : 0.));
  #endif
}







 /******************************************************************************
   Cellular CLASS
 ******************************************************************************/
#if defined NBD || defined GSMD
LiveObjectsCellular::LiveObjectsCellular()
  :
   LiveObjectsBase()
  ,m_Acces()
  ,m_Scanner()
  ,m_Sms()
  #ifdef GSMD
  ,m_GPRSAcces()
  #endif
{}

LiveObjectsCellular::~LiveObjectsCellular()
{}

void LiveObjectsCellular::begin(Protocol p, Encoding s, bool bDebug)
{
  LiveObjectsBase::begin(p,s,bDebug);
  if(p==MQTT)
  {
    switch(m_Security)
    {
      case TLS:
      #ifdef NBD
      m_pClient = new NBSSLClient();
      #elif defined GSMD
      m_pClient = new GSMSSLClient();
      #endif
      m_pMqttclient = new MqttClient(m_pClient);
      m_nPort = 8883;
      break;
      case NONE:
      #ifdef NBD
      m_pClient = new NBClient();
      #elif defined GSMD
      m_pClient = new GSMClient();
      #endif
      m_pMqttclient = new MqttClient(m_pClient);
      m_nPort = 1883;
      break;
      default:
      outputDebug(ERR,"Wrong mode! stopping...");
      while(true);
    }
    m_pMqttclient->onMessage(messageCallback);
  }
  else 
  {
    if(SECRET_SERVER_MSISDN.length()<3)
    {
      outputDebug(ERR,"SERVER MSISDN is empty! Check arduino_secrets.h! Stopping here....");
      while(true);
    }
  }
  m_bInitialized = true;
}
void LiveObjectsCellular::connectNetwork()
{
  //Set client id as IMEI
  if (!m_bInitialized)
  {
    outputDebug(WARN,"missing begin() call, calling with default protcol=MQTT, security protcol=TLS, debug=true");
    begin();
  }
  #ifdef NBD
  if(m_Acces.status()!= NB_READY)
  #elif defined GSMD
  if(m_Acces.status()!= GSM_READY)
  #endif
  {
    #ifdef NBD
    NBModem modem;
    #elif defined GSMD
    GSMModem modem;
    #endif
    if(modem.begin())
    {
      if(m_sMqttid.length()==0)
      {
        String imei="";
        for(int i=1;i<=3;i++)
        {
          imei=modem.getIMEI();
          if(imei.length()!=0) break;
          delay(100*i);
        }
        m_sMqttid = imei;
      }
    }
    else
    {
      outputDebug(ERR,"Failed to initialize modem!" );
      while(true);
    }

    outputDebug(INFO,"Connecting to cellular network");
    #ifdef NBD
    while (m_Acces.begin(SECRET_PINNUMBER.c_str(), SECRET_APN.c_str(), SECRET_APN_USER.c_str(), SECRET_APN_PASS.c_str()) != NB_READY)
      outputDebug(TXT,".");
    outputDebug();
    #elif defined GSMD
    while ((m_Acces.begin(SECRET_PINNUMBER.c_str()) != GSM_READY)) outputDebug(TXT, ".");
    while ((m_GPRSAcces.attachGPRS(SECRET_APN.c_str(), SECRET_APN_USER.c_str(), SECRET_APN_PASS.c_str()) != GPRS_READY)) outputDebug(TXT, ".");
    outputDebug();
    #endif
    outputDebug(INFO,"You're connected to the network");
  }
  if(m_nPort==8883){
    if (!m_bCertLoaded) {
      outputDebug(INFO,"Loading DigiCert Root CA certificate");
      MODEM.sendf("AT+USECMNG=0,0,\"%s\",%d", LO_ROOT_CERT.name, LO_ROOT_CERT.size);
      if (MODEM.waitForPrompt() != 1) {
        outputDebug(ERR,"Problem loading certificate!\nStopping here.");
        while (1) ;
      }
      else {
        MODEM.write(LO_ROOT_CERT.data, LO_ROOT_CERT.size);
        int ready;
        while (!MODEM.ready());
        m_bCertLoaded = true;
        outputDebug(INFO,"Certificate loaded");
      }
    }
  }
}


void LiveObjectsCellular::checkNetwork()
{
  #ifdef NBD
  if(m_Acces.status()!= NB_READY)
    connectNetwork();
  #elif defined GSMD
  if(m_Acces.status()!= GSM_READY)
    connectNetwork();
  #endif
  if(m_Protocol == SMS)
  {
    String msg;
    if(m_Sms.available())
    {
      int c =0;
      while(true)
      {
        c = m_Sms.read();
        if(c==-1)break;
        msg+=(char)c;
      }
      outputDebug(INFO,"Received command: ",msg);
      LiveObjects_command cmd(msg,nullptr);
      int index = commands.find(&cmd);
      if(index >= 0 ) commands[index]->callback("",msg);
      else outputDebug(INFO,"Unknown command");
    }
  }
}


void LiveObjectsCellular::disconnectNetwork()
{
  outputDebug(INFO,"Disconnecting from cellular network...");
  m_Acces.shutdown();
}

void LiveObjectsCellular::messageCallback(int msg)
{
  LiveObjects::get().onMQTTmessage(msg);
};

void LiveObjectsCellular::addNetworkInfo()
{
  String strength= m_Scanner.getSignalStrength();
  String carrier = m_Scanner.getCurrentCarrier();
  if(m_Protocol == MQTT && m_Encoding==TEXT) 
  {
    bool status;
    #ifdef NBD
    status = m_Acces.status() == NB_READY;
    #elif defined GSMD
    status = m_Acces.status() == GSM_READY;
    #endif
    addToPayload(easyDataPayload[JSONVALUE].createNestedObject("networkInfo"),"connection_status",status,"strength",strength,"carrier",carrier);
  }
  #ifdef NBD
  else addToStringPayload((m_Acces.status() == NB_READY),strength,carrier);
  #elif defined GSMD
  else addToStringPayload(m_Acces.status() == GSM_READY,strength,carrier);
  #endif
}

void LiveObjectsCellular::sendData()
{
  if(m_Protocol == MQTT) LiveObjectsBase::sendData();
  else
  {
    if(m_sPayload.length() > 160)
    {
      outputDebug(ERR,"Payload to big, skipping sending...");
      return;
    }
    else if (m_sPayload.length() < 1)
    {
      outputDebug(WARN,"Payload is empty, skipping...");
      return;
    }
    outputDebug(INFO,"Publishing message: ", m_sPayload);
    if(m_Sms.beginSMS(SECRET_SERVER_MSISDN.c_str())!=1) outputDebug(ERR,"Error occured while sending SMS");
    m_Sms.print(m_sPayload);
    if(m_Sms.endSMS()!=1) outputDebug(ERR,"Error occured while sending SMS");
    m_sPayload="";
  }
}

#endif

 /******************************************************************************
   WiFi CLASS
 ******************************************************************************/
#ifdef WIFI
LiveObjectsWiFi::LiveObjectsWiFi()
  :
   LiveObjectsBase()
   ,m_sMac()
   ,m_sIP()
{
}
LiveObjectsWiFi::~LiveObjectsWiFi()
{}



void LiveObjectsWiFi::begin(Protocol p, Encoding s, bool bDebug)
{
  LiveObjectsBase::begin(p,s,bDebug);
  if(p != MQTT)
  {
    outputDebug(ERR,"Wrong protocol! This board support only MQTT! Stopping....");
    while(true);
  }
  switch(m_Security)
  {
    case TLS:
    #ifdef ARDUINO_SAMD_MKR1000
      outputDebug(ERR,"TLS NOT COMPATIBLE, STOPPING...");
      while(true);
    #endif
    m_pClient = new WiFiSSLClient();
    m_pMqttclient = new MqttClient(m_pClient);
    m_nPort = 8883;
    break;
    case NONE:
    m_pClient = new WiFiClient();
    m_pMqttclient = new MqttClient(m_pClient);
    m_nPort = 1883;
    break;
    default:
    outputDebug(ERR,"Wrong mode! Stopping...");
    while(true);
  }
  m_bInitialized = true;
  m_pMqttclient->onMessage(messageCallback);
}

void LiveObjectsWiFi::connectNetwork()
{
  if(!m_bInitialized)
  {
    outputDebug(WARN,"missing begin() call, calling with default protcol=MQTT, security protcol=TLS, debug=true");
    begin();
  }
  if (WiFi.status() == WL_NO_SHIELD) {
    outputDebug(ERR,"Communication with WiFi module failed! Stopping here...");
    // don't continue
    while (true);
  }

  if(String(SECRET_SSID).length()<1)
  {
    outputDebug(ERR,"SECRET_SSID is empty, check arduino_secrets.h, Stopping here...");
    while(true);
  }
  String s = WiFi.firmwareVersion();
  outputDebug(INFO,"WiFi firmware version ",s);

  outputDebug(INFO,"Attempting to connect to SSID: ", SECRET_SSID);

  while (WiFi.begin(SECRET_SSID.c_str(), SECRET_WIFI_PASS.c_str()) != WL_CONNECTED) {
    // failed, retry
    outputDebug(TXT,".");
    delay(1000);
  }
   outputDebug();
  IPAddress ip = WiFi.localIP();
  for(int i=0;i<4;++i)
  {
    m_sIP+=ip[i];
    if(i!=3) m_sIP+='.';
  }

   uint8_t mac[6];
  char buff[10];
  WiFi.macAddress(mac);

  for(int i=5;i>=0;--i)
  {
    memset(buff,'\0',10);
    itoa(mac[i],buff,16);
    if(mac[i]<17)
    {
      m_sMac+="0";
      m_sMqttid+="0";
    }
    for(int j=0;j<strlen(buff);++j)
    {
      m_sMac += (char)toupper(buff[j]);
      m_sMqttid += (char)toupper(buff[j]);
    }
  if(i!=0) m_sMac += ':';
  }
}
void LiveObjectsWiFi::checkNetwork()
{
  if(WiFi.status()== WL_DISCONNECTED)
    connectNetwork();
}
void LiveObjectsWiFi::disconnectNetwork()
{
  outputDebug(INFO,"Disconnecting WiFi");
  WiFi.disconnect();
}
void LiveObjectsWiFi::messageCallback(int msg)
{
  LiveObjects::get().onMQTTmessage(msg);
}

void LiveObjectsWiFi::addNetworkInfo()
{
  String tmp;
  tmp = WiFi.RSSI();
  tmp += " dbm";
  if(m_Protocol == MQTT && m_Encoding==TEXT)  addToPayload(easyDataPayload[JSONVALUE].createNestedObject("networkInfo"),"mac",m_sMac,"ssid",SECRET_SSID,"ip",m_sIP,"strength",tmp);
  else addToPayload(m_sMac,SECRET_SSID,m_sIP,tmp);
}

#endif
LiveObjects& lo = LiveObjects::get();