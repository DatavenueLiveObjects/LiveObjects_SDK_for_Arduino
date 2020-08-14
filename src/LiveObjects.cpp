#include "LiveObjects.h"


LiveObjectsBase::LiveObjectsBase()
    :
     m_bDebug(false)
    ,m_bInitialized(false)
    ,m_bInitialMqttConfig(false)
    ,m_bCertLoaded(false)
    ,m_sPayload()
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
// void LiveObjectsBase::paramTyper(const String& name, int16_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
//   if (type == IMPLICIT)
//     addTypedParam(name, variable, INTEGER, T_INT16, callback);
//   else
//     addTypedParam(name, variable, type, T_INT16, callback);
// }
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
// void LiveObjectsBase::paramTyper(const String& name, uint16_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
//   if (type == IMPLICIT)
//     addTypedParam(name, variable, UNSIGNED_INTEGER, T_UINT16, callback);
//   else
//     addTypedParam(name, variable, type, T_UINT16, callback);
// }
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


void LiveObjectsBase::addCommand(const String name, onCommandCallback callback) {
  if(!commands.push(new LiveObjects_command(name, callback))) outputDebug(ERR, "Command ",name," already exists, skipping....");
}

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


void LiveObjectsBase::sendData() {

  easyDataPayload[JSONMODEL] = JSONMODELNAME;
  publishMessage(MQTT_PUBDATA, easyDataPayload);
  easyDataPayload.clear();

}

void LiveObjectsBase::sendData(const String customPayload) {
  StaticJsonDocument<PAYLOAD_DATA_SIZE> payload;
  deserializeJson(payload, customPayload);
  if (!payload.containsKey(JSONMODEL))
    payload[JSONMODEL] = JSONMODELNAME;
  publishMessage(MQTT_PUBDATA, payload);
}


void LiveObjectsBase::loop() {
  if (networkStatus == CONNECTED) {
    networkCheck();
  }
}
void LiveObjectsBase::setProtocol(Protocol p)
{
//TODO
}
void LiveObjectsBase::setMode(Mode s)
{
//TODO
}
void LiveObjectsBase::enableDebug(bool b)
{
  m_bDebug = b;
}
void LiveObjectsBase::setClientID(const String id)
{
  m_sMqttid = id;
}
void LiveObjectsBase::addTimestamp(time_t timestamp)
{
  timestamp-=504921600;
  char bufer[sizeof("2011-10-08T07:07:09Z")];
  strftime(bufer, sizeof(bufer), "%Y-%m-%dT%H:%M:%SZ",gmtime(&timestamp));
  if(m_Protocol == SMS)
  {
    String s = bufer;
    addToStringPayload(s);
  }
  else easyDataPayload["timestamp"]=bufer;
  
    
}
void LiveObjectsBase::addLocation(double lat, double lon, float alt)
{
  if(m_Protocol == SMS) addToStringPayload(lat,lon,alt);
  else addToPayload(easyDataPayload.createNestedObject("location"),"lat",lat,"lon",lon,"alt",alt);
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

#if defined ARDUINO_ARCH_AVR

LiveObjectsAVR::LiveObjectsAVR()
    :
    m_serialFona(FONA_TX, FONA_RX),
    m_Fona()
{}

void LiveObjectsAVR::begin(Protocol p, Mode m, bool debug)
{
  m_Protocol = p;
  m_Mode = m;
  m_bDebug=debug;
  if(m==NONE)m_nPort=1883;
  else m_nPort=8883;


  //Set reset pin to default state
  pinMode(FONA_RST, OUTPUT);
  digitalWrite(FONA_RST, HIGH);
  pinMode(FONA_PWRKEY, OUTPUT);

  //Power on Module
  digitalWrite(FONA_PWRKEY, LOW);
  delay(100); // For SIM7000
  digitalWrite(FONA_PWRKEY, HIGH);

  outputDebug(INFO, "Setting baudrate 9600");
  m_serialFona.begin(9600); // Default SIM7000 shield baud rate
  m_serialFona.println("AT+IPR=9600"); // Set baud rate
  delay(100); // Short pause to let the command run
  m_serialFona.begin(9600);
  if (!m_Fona.begin(m_serialFona)) {
    outputDebug(ERR,"Cannot find FONA");
    while(1); // Don't proceed if it couldn't find the device
  }

  String f;
  switch (m_Fona.type()) {
    case SIM800L:
      f= "SIM800L"; break;
    case SIM800H:
      f= "SIM800H"; break;
    case SIM808_V1:
      f="SIM808 (v1)"; break;
    case SIM808_V2:
      f="SIM808 (v2)"; break;
    case SIM5320A:
      f="SIM5320A (American)"; break;
    case SIM5320E:
      f="SIM5320E (European)"; break;
    case SIM7000A:
      f="SIM7000A (American)"; break;
    case SIM7000C:
      f="SIM7000C (Chinese)"; break;
    case SIM7000E:
      f="SIM7000E (European)"; break;
    case SIM7000G:
      f="SIM7000G (Global)"; break;
    case SIM7500A:
      f="SIM7500A (American)"; break;
    case SIM7500E:
      f="SIM7500E (European)"; break;
    default:
      f="???"; break;
  }
  outputDebug(INFO,"Fona is OK, found model: ",f);
  
  // Print module IMEI number.
  char tab[20];
  while(m_Fona.getIMEI(tab)<1);
  m_sMqttid = tab;
  outputDebug(INFO,"IMEI: ",m_sMqttid);

  m_Fona.setFunctionality(1);
}

void LiveObjectsAVR::checkNetwork()
{
  int n = m_Fona.getNetworkStatus();
  outputDebug(ERR,n);
  if (n == 0) outputDebug(INFO,"Network status: Not registered");
  else if (n == 1) outputDebug(INFO,"Network status: Registered (home)");
  else if (n == 2) outputDebug(INFO,"Network status: Not registered (searching)");
  else if (n == 3) outputDebug(INFO,"Network status: Denied");
  else if (n == 4) outputDebug(INFO,"Network status: Unknown");
  else if (n == 5) outputDebug(INFO,"Network status: Registered roaming");
  if (n!=1 && n!=5)
  {
    delay(2000);
    connectNetwork();
  } 
  
  // if(networkStatus==CONNECTED)
  // {
  //   while(m_Fona.available()) m_sMessage += (char)m_Fona.read();
  //   if(m_sMessage.length()>0)
  //   {
  //     outputDebug(INFO, "Received message: ",m_sMessage);
  //     m_sMessage="";
  //   }
  // }
}

void LiveObjectsAVR::connectNetwork()
{
  outputDebug(INFO, "Connecting to cellular network");
  checkNetwork();
  outputDebug(INFO, "Connected!");
  outputDebug(INFO, "Connecting to internet");
  if (!m_Fona.wirelessConnStatus()) 
  {
    while (!m_Fona.openWirelessConnection(true)) 
    {
      outputDebug(TEXT,".");
      delay(2000); // Retry every 2s
    }
  }
  outputDebug(INFO, "Connected!");
}

void LiveObjectsAVR::checkMQTT()
{
  if(!m_Fona.MQTT_connectionStatus())
    connectMQTT();
}
void LiveObjectsAVR::connectMQTT()
{
  if(!m_Fona.MQTT_connectionStatus())
  {
    m_Fona.MQTT_setParameter("URL", MQTT_BROKER, m_nPort);
    // Set up MQTT username and password if necessary
    m_Fona.MQTT_setParameter("USERNAME", MQTT_USER);
    m_Fona.MQTT_setParameter("PASSWORD", SECRET_LIVEOBJECTS_API_KEY.c_str());
    m_Fona.MQTT_setParameter("CLIENTID",m_sMqttid.c_str());
    //fona.MQTTsetParameter("KEEPTIME", 30); // Time to connect to server, 60s by default
    
    outputDebug(INFO,"Connecting to MQTT broker ", MQTT_BROKER, m_nPort);
    while (! m_Fona.MQTT_connect(true)) {
      outputDebug(TEXT,".");
    }
    outputDebug(INFO, "You are connected to mqtt broker");

    m_Fona.MQTT_subscribe(MQTT_SUBCFG,0);
    m_Fona.MQTT_subscribe(MQTT_SUBCMD,0);
  }
}
void LiveObjectsAVR::disconnectMQTT()
{

}

void LiveObjectsAVR::disconnectNetwork()
{

}

void LiveObjectsAVR::addNetworkInfo()
{

}

void LiveObjectsAVR::addPowerStatus()
{

}

void LiveObjectsAVR::onMQTTmessage(int messageSize)
{

}

void LiveObjectsAVR::publishMessage(const String& topic, JsonDocument& payload)
{
  String s;
  outputDebug(INFO,"Publishing message on topic: ", topic);
  serializeJsonPretty(payload,Serial);
  serializeJson(payload,s);
  while(!m_Fona.MQTT_publish(topic.c_str(),s.c_str(),s.length(),0,0)) outputDebug(WARN,"Failed to send message, retrying...");
}
#elif defined ARDUINO_ARCH_SAMD

void LiveObjectsSAMD::configurationManager(int messageSize) {
  StaticJsonDocument<PAYLOAD_DATA_SIZE> configOut;
  if (messageSize >= 0)
  { // config update received
    StaticJsonDocument<PAYLOAD_DATA_SIZE> configIn;
    deserializeJson(configIn, *m_pMqttclient);

    if(m_bDebug)
    {
    serializeJsonPretty(configIn, Serial);
    outputDebug(TEXT);
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

LiveObjectsSAMD::LiveObjectsSAMD()
    :
    LiveObjectsBase()
    ,m_pClient(nullptr)
    ,m_pMqttclient(nullptr)
{}

LiveObjectsSAMD::~LiveObjectsSAMD()
{
  if(m_pMqttclient != nullptr)delete m_pMqttclient;
  if(m_pClient != nullptr)delete m_pClient;
}

/******************************************************************************
   COMMAND MANAGEMENT
 ******************************************************************************/
void LiveObjectsSAMD::commandManager() {
  StaticJsonDocument<PAYLOAD_DATA_SIZE> cmdIn;
  StaticJsonDocument<PAYLOAD_DATA_SIZE> cmdOut;
  deserializeJson(cmdIn, *m_pMqttclient);

  if(m_bDebug)
  {
  serializeJsonPretty(cmdIn, Serial);
  outputDebug(TEXT);
  }
  for (uint8_t i = 0; i < commands.size(); i++) // only with MQTT or SMS !!
    if (cmdIn[F("req")] == commands[i]->label) {
      cmdOut[JSONCID] = cmdIn[JSONCID];
      String response;
      commands[i]->callback(cmdIn[F("arg")].as<String>(), response);
      if (response.length() != 0)
        cmdOut[F("res")] = serialized(response);
      break;
    }
  publishMessage(MQTT_PUBCMD, cmdOut);
}

/******************************************************************************
   MQTT FUNCTIONS
 ******************************************************************************/

void LiveObjectsSAMD::onMQTTmessage(int messageSize) {
  String topic = m_pMqttclient->messageTopic();
  outputDebug(INFO,"Received a message on topic: ", topic);

  if (topic == MQTT_SUBCFG)
    configurationManager(messageSize);
  else if (topic == MQTT_SUBCMD)
    commandManager();
}

void LiveObjectsSAMD::publishMessage(const String& topic, JsonDocument& payload) {
  outputDebug(INFO,"Publishing message on topic: ", topic);
  if(m_bDebug)
  {
  serializeJsonPretty(payload, Serial);
  outputDebug(TEXT);
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

/******************************************************************************
   LIVE OBJECTS
 ******************************************************************************/

void LiveObjectsSAMD::checkMQTT()
{
  if(!m_pMqttclient->connected())
    connectMQTT();
  m_pMqttclient->poll();
}
void LiveObjectsSAMD::connectMQTT()
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


  while (!m_pMqttclient->connect(MQTT_BROKER, m_nPort)) outputDebug(TEXT,".");
  outputDebug(INFO,"You're connected to the MQTT broker");

  m_pMqttclient->subscribe(MQTT_SUBCFG);
  m_pMqttclient->subscribe(MQTT_SUBCMD);

  m_pMqttclient->poll();

  if (!m_bInitialMqttConfig) {
    configurationManager();
    m_bInitialMqttConfig = true;
  }

}
void LiveObjectsSAMD::disconnectMQTT()
{
  outputDebug(INFO,"Closing MQTT connection...");
  m_pMqttclient->stop();
}

void LiveObjectsSAMD::addPowerStatus()
{
  #ifdef PMIC_PRESENT
  int DATA = readRegister(SYSTEM_STATUS_REGISTER);
  bool bat = (DATA & (1<<0 == 0));
  bool usb = (DATA & ((1<<5)|(1<<4) != 0));
  bool power=(DATA & (1<<2));
  double voltage=0.;
  if(bat) voltage = analogRead(ADC_BATTERY) * (4.3 / 1023.0);
  if(m_Protocol==SMS) addToStringPayload((usb ? 1 : (!bat && power ? 1 : 0, bat ,( bat ? voltage : 0.))));
  else
  {
    addToPayload(easyDataPayload[JSONVALUE].createNestedObject("powerStatus"),
                "external_power",(usb ? "Yes" : !bat && power ? "Yes" : "No")
                ,"battery_connected",(bat ? "Yes" : "No")
                ,"battery_voltage",voltage);
  }
  #endif
}







 /******************************************************************************
   NB CLASS
 ******************************************************************************/
#if defined NBD || defined GSMD
LiveObjectsCellular::LiveObjectsCellular()
  :
   LiveObjectsSAMD()
  ,m_Acces()
  ,m_Scanner()
  ,m_Sms()
  #ifdef GSMD
  ,m_GPRSAcces()
  #endif
{}

LiveObjectsCellular::~LiveObjectsCellular()
{}

void LiveObjectsCellular::begin(Protocol p, Mode s, bool bDebug)
{
  m_Protocol=p;
  m_Mode = s;
  m_bDebug = bDebug;
  if(p==MQTT)
  {
    switch(s)
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
     outputDebug(TEXT,".");
   #elif defined GSMD
    while ((m_Acces.begin(SECRET_PINNUMBER.c_str()) != GSM_READY)
        || (m_GPRSAcces.attachGPRS(SECRET_APN.c_str(), SECRET_APN_USER.c_str(), SECRET_APN_PASS.c_str()) != GPRS_READY)){
      outputDebug(TEXT, ".");}
   #endif
   outputDebug(INFO,"You're connected to the network");

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
  String strength=m_Scanner.getSignalStrength();
  String carrier = m_Scanner.getCurrentCarrier();
  #ifdef NBD
  if(m_Protocol == SMS) addToStringPayload((int)(m_Acces.status() == NB_READY),strength,carrier);
  #elif defined GSMD
  if(m_Protocol == SMS) addToStringPayload(m_Acces.status() == GSM_READY,strength,carrier);
  #endif
  else 
  {
    //JsonObject obj = easyDataPayload[JSONVALUE].createNestedObject("networkInfo");
    String status;
    #ifdef NBD
    status = m_Acces.status() == NB_READY ? "connected":"disconnected";
    #elif defined GSMD
    status = m_Acces.status() == GSM_READY ? "connected":"disconnected";
    #endif
    addToPayload(easyDataPayload[JSONVALUE].createNestedObject("networkInfo"),"connection_status",status,"strength",strength,"carrier",carrier);
  }
}

void LiveObjectsCellular::sendData()
{
  if(m_Protocol == MQTT) LiveObjectsSAMD::sendData();
  else
  {
    if(m_sPayload.length() > 130)
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
    m_Sms.beginSMS(SECRET_SERVER_MSISDN.c_str());
    m_Sms.print(m_sPayload);
    m_Sms.endSMS();
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
   LiveObjectsSAMD()
   ,m_sMac()
   ,m_sIP()
{
}
LiveObjectsWiFi::~LiveObjectsWiFi()
{}



void LiveObjectsWiFi::begin(Protocol p, Mode s, bool bDebug)
{
  m_bDebug = bDebug;
  m_Protocol=p;
  m_Mode = s;
  if(p != MQTT)
  {
    outputDebug(ERR,"Wrong protocol! This board support only MQTT! Stopping....");
    while(true);
  } 
  switch(s)
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


  uint8_t mac[6];
  char buff[10];
  WiFi.macAddress(mac);
  for(int i=0;i<6;++i)
  {
    memset(buff,'\0',10);
    itoa(mac[i],buff,16);
    m_sMac += buff;
    if(i!=5) m_sMac += ':';
  }
  m_sMqttid = m_sMac;
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
    outputDebug(TEXT,".");
    delay(1000);
  }
  IPAddress ip = WiFi.localIP();
  for(int i=0;i<4;++i)
  {
    m_sIP+=ip[i];
    if(i!=3) m_sIP+='.';
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
  //JsonObject obj = easyDataPayload[JSONVALUE].createNestedObject("networkInfo");
  // obj["mac"] = m_sMac;
  // obj["ssid"] = SECRET_SSID;
  // obj["ip"] = m_sIP;
  String tmp;
  tmp = WiFi.RSSI();
  tmp += " dbm";
  //obj["strength"] = tmp;
  addToPayload(easyDataPayload[JSONVALUE].createNestedObject("networkInfo"),"mac",m_sMac,"ssid",SECRET_SSID,"ip",m_sIP,"strength",tmp);

}
#endif

#endif
LiveObjects& lo = LiveObjects::get();
