#pragma once


/******************************************************************************
   DEFAULT VALUES FOR LIVEOBJECTS
 ******************************************************************************/
#define PAYLOAD_DATA_SIZE 1024
#define KEEP_ALIVE_NETWORK 1000
#define SW_REVISION "1.8.0"


/******************************************************************************
   LiveObjects MQTT constants
 ******************************************************************************/
#define MQTT_BROKER "liveobjects.orange-business.com"
#define MQTT_USER "json+device"
#define MQTT_PUBDATA "dev/data"
#define MQTT_PUBDATA_BINARY "dev/v1/data/binary"
#define MQTT_SUBCFG "dev/cfg/upd"
#define MQTT_PUBCFG "dev/cfg"
#define MQTT_SUBCMD "dev/cmd"
#define MQTT_PUBCMD "dev/cmd/res"

/******************************************************************************
   JSON payload constants
 ******************************************************************************/
#define JSONCID "cid"
#define JSONCFG "cfg"
#define JSONCFGVALUE "v"
#define JSONCFGTYPE "t"
#define JSONMODEL "model"
#define JSONVALUE "value"
#define JSONMODELNAME "Orange_Pollution_Shield"
/******************************************************************************
   INCLUDES
 ******************************************************************************/
#include <Arduino.h>
#include <ArduinoJson.h>
#ifndef ARDUINO_ARCH_AVR
#include <ctime>
#else
typedef long long time_t;
#endif
#include "LiveObjectsCert.h"
#include "Utils.h"

/******************************************************************************
   TYPEDEFS/ENUMS
 ******************************************************************************/

enum LiveObjects_networkStatus
{
  CONNECTED,
  DISCONNECTED
};

enum LiveObjects_parameterType {
  INTEGER,
  UNSIGNED_INTEGER,
  BINR,
  STRING,
  DECIMAL,
  IMPLICIT
};

enum LiveObjects_variableType {
  T_BOOL,
  T_CHAR,
  T_INT,
  T_INT8,
  T_INT16,
  T_INT32,
  T_UINT,
  T_UINT8,
  T_UINT16,
  T_UINT32,
  T_DOUBLE,
  T_FLOAT,
  T_STRING,
};

enum Protocol
{
  MQTT,
  SMS
  //,LORA
};

enum Security
{
  NONE
  ,TLS
  //,DTLS
};

enum Encoding
{
  BINARY
  ,TEXT
};

enum LOG_MSGTYPE
{
  INFO,
  WARN,
  ERR,
  TXT
};

typedef void (*onParameterUpdateCallback)();
typedef void (*onCommandCallback)(const String, String&);

class LiveObjectsBase
{
protected:
  LiveObjectsBase();
  LiveObjectsBase(const LiveObjectsBase&) = delete;
  LiveObjectsBase& operator==(const LiveObjectsBase&) = delete;

/******************************************************************************
   STRUCTS
******************************************************************************/
public:
    struct LiveObjects_parameter
    {
      LiveObjects_parameter(String name, void *variable, LiveObjects_parameterType t, LiveObjects_variableType vt, onParameterUpdateCallback c)
      :
        label(name)
       ,type(t)
       ,variableType(vt)
       ,callback(c)
       ,value(variable)
      {}

      bool operator==(const LiveObjects_parameter& p){ return label == p.label; }
      String label;
      LiveObjects_parameterType type;
      LiveObjects_variableType variableType;
      onParameterUpdateCallback callback;
      void *value;
    };
    struct LiveObjects_command
    {
      LiveObjects_command(String l, onCommandCallback c): label(l), callback(c){}
      bool operator==(const LiveObjects_command& p){ return label == p.label; }
      String label;
      onCommandCallback callback;
    };

/******************************************************************************
   FUNCTIONS
******************************************************************************/

public:
  void changeConfiguration(Protocol p,Security s, Encoding mode);
  void setSecurity(Security s);
  void enableDebug(bool b);
  void setClientID(const String id);
  void setDecoder(String s);

public:
  bool debugEnabled();

public:
  void addTimestamp(time_t timestamp);
  void addLocation(double lat, double lon, double alt);
  virtual void addPowerStatus()=0;
  virtual void addNetworkInfo()=0;
  void clearPayload();

public:
  void addCommand(const String name, onCommandCallback callback);
  void publishMessage(const String& topic, JsonDocument& payload);
  void publishMessage(const String& topic, String& payload);
  void connect();
  void networkCheck();
  void disconnect();
  //void onMQTTmessage(int messageSize);
  void sendData();
  void sendData(const String customPayload);
  void loop();

protected:
  virtual void begin(Protocol p, Encoding e, bool d);
  virtual void connectNetwork() =0;
  virtual void checkNetwork() =0;
  virtual void disconnectNetwork() =0;
  template<typename T,typename ... Args>
  void addToStringPayload(T val, Args ... args);
  void addToStringPayload(){};

protected:
  template<typename T, typename ... Args>
  void outputDebug(LOG_MSGTYPE type,T item, Args&... args);
  void outputDebug(LOG_MSGTYPE type = TXT){Serial.print('\n');};
  void messageDebug(String& topic, JsonDocument& doc, bool r = false);
protected:
  virtual void checkMQTT()=0;
  virtual void connectMQTT()=0;
  virtual void disconnectMQTT()=0;
  virtual void stopMQTT()=0;
  virtual void sendMQTT(String& topic, JsonDocument& doc)=0;
  virtual void sendMQTT(String& topic, String& doc)=0;
  virtual void deserializeMessage(JsonDocument& doc)=0;


protected:
 /******************************************************************************
   CONFIGURATION MANAGER
 ******************************************************************************/
    void configurationManager(String topic,int messageSize =-1);
/******************************************************************************
   COMMAND MANAGEMENT
 ******************************************************************************/
    void commandManager(String topic);
public:
/******************************************************************************
   TEMPLATE FUNCTIONS
******************************************************************************/
    template<typename LOtA>
    void addParameter(const String name, LOtA &variable);
    template<typename LOtB>
    void addParameter(const String name, LOtB &variable, LiveObjects_parameterType type);
    template<typename LOtC>
    void addParameter(const String name, LOtC &variable, onParameterUpdateCallback callback);
    template<typename LOtD>
    void addParameter(const String name, LOtD &variable, onParameterUpdateCallback callback, LiveObjects_parameterType type);
    template<typename LOtE>
    void addTypedParam(const String name, LOtE *variable, LiveObjects_parameterType type, LiveObjects_variableType variableType, onParameterUpdateCallback callback);
    template<typename LOtF>
    void updateParameter(const LiveObjects_parameter param, LOtF* ptr, const JsonDocument& configIn, JsonDocument& configOut);
    template<typename LOtH>
    void addToPayload(const String label, LOtH value);
    template<typename T>
    void addToPayload(T val);
    void addObjectToPayload(String name, JsonObject& obj);
protected:
    template<typename T, typename E, typename ... Args>
    void addToPayload(JsonObject obj, T key, E val, Args ... args);
    void addToPayload(JsonObject obj){};
    template<typename T, typename E, typename R, typename ... Args>
    void addToPayload(T key, E val, R tmp , Args ... args);
/******************************************************************************
   OBJECTS
******************************************************************************/
protected:
    LinkedList<LiveObjects_command> commands;
    LinkedList<LiveObjects_parameter> parameters;
    LiveObjects_networkStatus networkStatus = DISCONNECTED;
    StaticJsonDocument<PAYLOAD_DATA_SIZE> easyDataPayload;
/******************************************************************************
   VARIABLES
******************************************************************************/
private:
    unsigned long lastKeepAliveNetwork;
protected:
    String m_sMqttid;
    String m_sPayload;
    String m_sTopic;
    String m_sDecoder;
    uint16_t m_nPort;
    Protocol m_Protocol;
    Security m_Security;
    Encoding m_Encoding;
    
    bool m_bDebug;
    bool m_bInitialized;
    bool m_bInitialMqttConfig;
    bool m_bCertLoaded;
    bool m_bSubCMD;
/******************************************************************************
   PARAM TYPERS
 ******************************************************************************/
    void paramTyper(const String& name, bool* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, char* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    #if not defined ESP8266 && not defined ESP32 && not defined ARDUINO_AVR_FEATHER32U4
    void paramTyper(const String& name, int* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, unsigned int* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    #endif
    void paramTyper(const String& name, int8_t*variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, int32_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, int16_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, uint8_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, uint16_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, uint32_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, double* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, float* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, String* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);

/******************************************************************************
   POINTER TYPER
 ******************************************************************************/
   void ptrTyper(const LiveObjects_parameter param, const JsonDocument& configIn, JsonDocument& configOut);
};

template<typename LOtA>
inline void LiveObjectsBase::addParameter(const String name, LOtA &variable) {
  onParameterUpdateCallback ptr = nullptr;
  paramTyper(name, &variable, IMPLICIT, ptr);
}
template<typename LOtB>
inline void LiveObjectsBase::addParameter(const String name, LOtB &variable, LiveObjects_parameterType type) {
  onParameterUpdateCallback ptr = nullptr;
  paramTyper(name, &variable, type, ptr);
}
template<typename LOtC>
inline void LiveObjectsBase::addParameter(const String name, LOtC &variable, onParameterUpdateCallback callback) {
  paramTyper(name, &variable, IMPLICIT, callback);
}
template<typename LOtD>
inline void LiveObjectsBase::addParameter(const String name, LOtD &variable, onParameterUpdateCallback callback, LiveObjects_parameterType type) {
  paramTyper(name, &variable, type, callback);
}

template<typename LOtE>
inline void LiveObjectsBase::addTypedParam(const String name, LOtE *variable, LiveObjects_parameterType type, LiveObjects_variableType variableType, onParameterUpdateCallback callback) {
  if(!parameters.push(new LiveObjects_parameter(name, variable, type, variableType, callback))) outputDebug(ERR, "Parameter ",name," already exists, skipping...");
}

template<typename LOtF>
inline void LiveObjectsBase::updateParameter(const LiveObjects_parameter param, LOtF* ptr, const JsonDocument& configIn, JsonDocument& configOut) {
  if (!configIn.isNull())
    *ptr = configIn[JSONCFG][param.label][JSONCFGVALUE].as<LOtF>();
  configOut[JSONCFG][param.label][JSONCFGVALUE] = *ptr;
}

template<typename LOtH>
inline void LiveObjectsBase::addToPayload(const String label, LOtH value) {
  if(m_Protocol == MQTT)
  {
    if(m_Encoding == TEXT) easyDataPayload[JSONVALUE][label] = value;
    else 
    {
      outputDebug(WARN,"BINARY Encoding active, adding only value, label skipped...");
      addToStringPayload(value);
    }
  }
  else
  {
    addToStringPayload(value);
  }
}

template<typename T>
inline void LiveObjectsBase::addToPayload(T value)
{
  if(m_Protocol == MQTT)
  {
    if(m_Encoding == TEXT) outputDebug(WARN,"Cannot add value without label to JsonPayload, skipping...");
    else addToStringPayload(value);
  }
  else
  {
    addToStringPayload(value);
  }
}

template<typename T, typename E, typename R, typename ... Args>
inline void LiveObjectsBase::addToPayload(T key, E val,R tmp, Args ... args)
{
  if(m_Protocol == MQTT) addToPayload(key,val);
  addToPayload(tmp, args...); 
}
template<typename T, typename E, typename ... Args>
inline void LiveObjectsBase::addToPayload(JsonObject obj, T key, E val, Args ... args)
{
  obj[key]=val;
  addToPayload(obj,args...);
}

template<typename T, typename ... Args>
inline void LiveObjectsBase::outputDebug(LOG_MSGTYPE type,T item, Args&... args)
{
  switch(type)
  {
    case INFO:
      if(m_bDebug)
      {
        Serial.print("[INFO] ");
        Serial.print(item);
      }
      break;
    case WARN:
      if(m_bDebug)
      {
        Serial.print("[WARNING] ");
        Serial.print(item);
      }
      break;
    case ERR:
      Serial.print("[ERROR] ");
      Serial.print(item);
      break;
    default:
      if(m_bDebug) Serial.print(item);
  }
  if(String(item)!=".") outputDebug(TXT,args...);
}

template<typename T,typename ... Args>
void LiveObjectsBase::addToStringPayload(T val, Args ... args)
{
  if(m_Encoding == BINARY) m_sPayload+=ToHex(val);
  else 
  {
    m_sPayload+=val;
    m_sPayload+=';';
  }
  
  addToStringPayload(args...);
}

extern const String SECRET_LIVEOBJECTS_API_KEY;