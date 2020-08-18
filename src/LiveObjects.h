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
#define JSONMODELNAME "github_sample_MKRNB"
/******************************************************************************
   INCLUDES
 ******************************************************************************/
#include "ArduinoMqttClient.h"
#include <ArduinoJson.h>
#include <time.h>
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

enum Mode
{
  NONE
  ,TLS
  ,BINARY
  ,TXT
  //,DTLS
};

enum LOG_MSGTYPE
{
  INFO,
  WARN,
  ERR,
  TEXT
};

typedef void (*onParameterUpdateCallback)();
typedef void (*onCommandCallback)(const String, String&);

class LiveObjectsBase
{
protected:
  LiveObjectsBase();
  ~LiveObjectsBase(){};
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
       ,value(variable)
       ,type(t)
       ,variableType(vt)
       ,callback(c)
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
  void setProtocol(Protocol p);
  void setMode(Mode s);
  void enableDebug(bool b);
  void setClientID(const String id);

public:
  bool debugEnabled();

public:
  void addTimestamp(time_t timestamp);
  void addLocation(double lat, double lon, float alt);
  virtual void addPowerStatus()=0;
  virtual void addNetworkInfo()=0;
  void clearPayload();

public:
  void addCommand(const String name, onCommandCallback callback);
  virtual void publishMessage(const String& topic, JsonDocument& payload)=0;
  void connect();
  void networkCheck();
  void disconnect();
  void onMQTTmessage(int messageSize);
  void sendData();
  void sendData(const String customPayload);
  void loop();

protected:
  virtual void begin(Protocol, Mode, bool) = 0;
  virtual void connectNetwork() =0;
  virtual void checkNetwork() =0;
  virtual void disconnectNetwork() =0;
  template<typename T,typename ... Args>
  void addToStringPayload(T val, Args ... args);
  void addToStringPayload(){};

protected:
  template<typename T, typename ... Args>
  void outputDebug(LOG_MSGTYPE type,T item, Args&... args);
  void outputDebug(LOG_MSGTYPE type = TEXT){Serial.print('\n');};
private:
  virtual void checkMQTT()=0;
  virtual void connectMQTT()=0;
  virtual void disconnectMQTT()=0;

private:
 /******************************************************************************
   CONFIGURATION MANAGER
 ******************************************************************************/
    virtual void configurationManager(int)=0;
/******************************************************************************
   COMMAND MANAGEMENT
 ******************************************************************************/
    virtual void commandManager()=0;

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
protected:
    template<typename T, typename E, typename ... Args>
    void addToPayload(JsonObject obj, T key, E val, Args ... args);
    void addToPayload(JsonObject obj){};
    template<typename T, typename E, typename R, typename ... Args>
    void addToPayload(T key, E val, R tmp , Args ... args);
/******************************************************************************
   PARAM TYPERS
 ******************************************************************************/
    void paramTyper(const String& name, bool* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, char* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, int* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, int8_t*variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    //void paramTyper(const String& name, int16_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, int32_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, unsigned int* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, uint8_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    //void paramTyper(const String& name, uint16_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, uint32_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, double* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, float* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, String* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);

/******************************************************************************
   POINTER TYPER
 ******************************************************************************/
   void ptrTyper(const LiveObjects_parameter param, const JsonDocument& configIn, JsonDocument& configOut);

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
  unsigned long lastKeepAliveMQTT =  0;
  unsigned long lastKeepAliveNetwork  = 0;

protected:
  String m_sMqttid;
  String m_sPayload;
  uint16_t m_nPort;
  Protocol m_Protocol;
  Mode m_Mode;
  bool m_bInitialMqttConfig;
  bool m_bDebug;
  bool m_bCertLoaded;
  bool m_bInitialized;
  bool m_bSubCMD;
};

template<typename LOtA>
inline void LiveObjectsBase::addParameter(const String name, LOtA &variable) {
  onParameterUpdateCallback ptr = nullptr;
  paramTyper(name, &variable, IMPLICIT, ptr);
}
template<typename LOtB>
inline void LiveObjectsBase::addParameter(const String name, LOtB &variable, LiveObjects_parameterType type) {
  onParameterUpdateCallback ptr = NULL;
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
  if(m_Protocol == MQTT) easyDataPayload[JSONVALUE][label] = value;
  else
  {
    outputDebug(WARN,"SMS protocol active, sending only value, label skipped...");
    addToStringPayload(value);
  }
}

template<typename T>
inline void LiveObjectsBase::addToPayload(T value)
{
  if(m_Protocol == MQTT) outputDebug(ERR, "Odd number of parameters in addToPayload!");
  addToStringPayload(value);
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
      Serial.print(item);
  }
  if(String(item)!=".") outputDebug(TEXT,args...);
}

template<typename T,typename ... Args>
void LiveObjectsBase::addToStringPayload(T val, Args ... args)
{
  if(m_Mode == Mode::BINARY) m_sPayload+=ToHex(val);
  else 
  {
    m_sPayload+=val;
    m_sPayload+=';';
  }
  
  addToStringPayload(args...);
}

extern const String SECRET_LIVEOBJECTS_API_KEY;


#ifdef ARDUINO_ARCH_AVR
#include "Adafruit_FONA.h"
#include <SoftwareSerial.h>
#define FONA_PWRKEY 6
#define FONA_RST 7
#define FONA_TX 10 // Microcontroller RX
#define FONA_RX 11 // Microcontroller TX

class LiveObjectsAVR : public LiveObjectsBase
{
public:
    static LiveObjectsAVR& get()
    {
      static LiveObjectsAVR g; return g;
    }
protected:
  LiveObjectsAVR();
  ~LiveObjectsAVR();
  LiveObjectsAVR(const LiveObjectsAVR&) = delete;
  LiveObjectsAVR& operator==(const LiveObjectsAVR&) = delete;
protected:
  void checkMQTT();
  void connectMQTT();
  void disconnectMQTT();


public:
  void addPowerStatus();
  void addNetworkInfo();
  void begin(Protocol p, Mode m , bool debug);
protected:
  void publishMessage(const String& topic, JsonDocument& payload);
  void onMQTTmessage(int messageSize);

protected:
  void connectNetwork();
  void checkNetwork();
  void disconnectNetwork();
  
private:
 /******************************************************************************
   CONFIGURATION MANAGER
 ******************************************************************************/
    void configurationManager(int message =-1){};
/******************************************************************************
   COMMAND MANAGEMENT
 ******************************************************************************/
    void commandManager(){};

  private:
  String m_sMessage;
};
extern SoftwareSerial* m_serialFona;
extern Adafruit_FONA_LTE* m_Fona;

typedef LiveObjectsAVR LiveObjects;
#elif defined ARDUINO_ARCH_SAMD
class LiveObjectsSAMD : public LiveObjectsBase
{
protected:
  LiveObjectsSAMD();
  ~LiveObjectsSAMD();
  LiveObjectsSAMD(const LiveObjectsSAMD&) = delete;
  LiveObjectsSAMD& operator==(const LiveObjectsSAMD&) = delete;


protected:
  void checkMQTT();
  void connectMQTT();
  void disconnectMQTT();


public:
  void addPowerStatus();

protected:
  void publishMessage(const String& topic, JsonDocument& payload);
  void onMQTTmessage(int messageSize);

protected:
  virtual void begin(Protocol, Mode, bool) = 0;
  virtual void connectNetwork() =0;
  virtual void checkNetwork() =0;
  virtual void disconnectNetwork() =0;
  virtual void addNetworkInfo()=0;


private:
 /******************************************************************************
   CONFIGURATION MANAGER
 ******************************************************************************/
    void configurationManager(int message =-1);
/******************************************************************************
   COMMAND MANAGEMENT
 ******************************************************************************/
    void commandManager();

protected:
    Client* m_pClient;
    MqttClient *m_pMqttclient;
};

 /******************************************************************************
   Cellular BOARDS CLASS
 ******************************************************************************/
#if defined ARDUINO_SAMD_MKRNB1500
#define NBD
#include <MKRNB.h>
#elif defined ARDUINO_SAMD_MKRGSM1400
#define GSMD
#include <MKRGSM.h>
#endif
#if defined NBD || defined GSMD
class LiveObjectsCellular : public LiveObjectsSAMD
{
  public:
    static LiveObjectsCellular& get()
    {
      static LiveObjectsCellular g; return g;
    }

  private:
    LiveObjectsCellular();
    ~LiveObjectsCellular();
    LiveObjectsCellular(const LiveObjectsCellular&)  = delete;
    LiveObjectsCellular& operator== (const LiveObjectsCellular&) =  delete;
  public:
    void begin(Protocol p=MQTT, Mode s=TLS, bool bDebug=true) override;
    void addNetworkInfo() override;
    void sendData();
  private:
    void connectNetwork() override;
    void checkNetwork() override;
    void disconnectNetwork() override;
    static void messageCallback(int msg);
  private:
  #ifdef NBD
  NB m_Acces;
  NBScanner m_Scanner;
  NB_SMS m_Sms;
  #endif
  #ifdef GSMD
  GSM m_Acces;
  GPRS m_GPRSAcces;
  GSMScanner m_Scanner;
  GSM_SMS m_Sms;
  #endif
};

typedef LiveObjectsCellular LiveObjects;

extern const String SECRET_SERVER_MSISDN;
extern const String SECRET_PINNUMBER;
extern const String SECRET_APN;
extern const String SECRET_APN_USER;
extern const String SECRET_APN_PASS;
#endif



/******************************************************************************
  WIFI BOARDS CLASS
******************************************************************************/
#if defined ARDUINO_SAMD_MKRWIFI1010 || defined ARDUINO_SAMD_NANO_33_IOT || defined ARDUINO_SAMD_MKRVIDOR4000
#include <WiFiNINA.h>
#define WIFI
#define NINA
#endif
#if ARDUINO_SAMD_MKR1000
#include <WiFi101.h>
#define W101
#define WIFI
#endif
#ifdef WIFI
class LiveObjectsWiFi : public LiveObjectsSAMD
{
  public:
    static LiveObjectsWiFi& get()
    {
      static LiveObjectsWiFi w; return w;
    }

  private:
    LiveObjectsWiFi();
    ~LiveObjectsWiFi();
    LiveObjectsWiFi(const LiveObjectsWiFi&)  = delete;
    LiveObjectsWiFi& operator== (const LiveObjectsWiFi&) =  delete;
  public:
    void begin(Protocol p=MQTT, Mode s=TLS, bool bDebug=true) override;
    void addNetworkInfo() override;
  private:
    void connectNetwork() override;
    void checkNetwork() override;
    void disconnectNetwork() override;
    static void messageCallback(int msg);
  private:
  String m_sMac;
  String m_sIP;
};
extern const String SECRET_SSID;
extern const String SECRET_WIFI_PASS;

typedef LiveObjectsWiFi LiveObjects;
#endif

#if defined ARDUINO_SAMD_MKRWIFI1010 || defined ARDUINO_SAMD_MKRNB1500 || defined ARDUINO_SAMD_MKRGSM1400
#define PMIC_PRESENT
#endif

#endif;

extern LiveObjects& lo;
