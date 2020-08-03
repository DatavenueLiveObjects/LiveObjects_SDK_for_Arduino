#pragma once
/******************************************************************************
   DEFAULT VALUES FOR LIVEOBJECTS
 ******************************************************************************/
#define PARAMETERS_NB_MAX 10
#define PAYLOAD_DEVMGT_SIZE 1024
#define PAYLOAD_DATA_SIZE 512
#define KEEP_ALIVE_NETWORK 10000
#define SW_REVISION "1.8.0"

/******************************************************************************
   PMIC constants
 ******************************************************************************/
#define PMIC_ADDRESS 0x6B
#define SYSTEM_STATUS_REGISTER 0x08
#define PMIC_VERSION_REGISTER 0x0A

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
#include <ctime>
#include <Wire.h>
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
  BINARY,
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
  //,SMS
  //,LORA
};

enum Security
{
  NONE
  ,TLS
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
  ~LiveObjectsBase();
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
        String label;
        onCommandCallback callback;
    };

/******************************************************************************
   FUNCTIONS
******************************************************************************/

public:
  void setProtocol(Protocol p);
  void setSecurity(Security s);
  void enableDebug(bool b);
  void setClientID(const String id);

public:
  bool debugEnabled();

public:
  void addTimestamp(time_t timestamp);
  void addLocation(double lat, double lon, float alt);
  void addPowerStatus();
  virtual void addNetworkInfo()=0;
  void clearPayload();

public:
  void addCommand(const String name, onCommandCallback callback);
  void publishMessage(const String& topic, JsonDocument& payload);
  void connect();
  void networkCheck();
  void disconnect();
  void onMQTTmessage(int messageSize);
  void sendData();
  void sendData(const String customPayload);
  void loop();

protected:
  virtual void begin(Protocol, Security, bool) =0;
  virtual void connectNetwork() =0;
  virtual void checkNetwork() =0;
  virtual void disconnectNetwork() =0;

protected:
  template<typename T, typename ... Args>
  void outputDebug(MSGTYPE type,T item, Args&... args);
  void outputDebug(MSGTYPE type){Serial.print('\n');};
  int readRegister(byte address);

private:
  void checkMQTT();
  void connectMQTT();
  void disconnectMQTT();


private:
 /******************************************************************************
   CONFIGURATION MANAGER
 ******************************************************************************/
    void configurationManager(int messageSize =-1);
/******************************************************************************
   COMMAND MANAGEMENT
 ******************************************************************************/
    void commandManager();
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
    Client* m_pClient;
    MqttClient *m_pMqttclient;
    TwoWire* m_pWire;
    String m_sMqttid;
    uint16_t m_nPort;
    bool m_bInitialMqttConfig;
    bool m_bDebug;
    bool m_bCertLoaded;
    bool m_bInitialized;
/******************************************************************************
   PARAM TYPERS
 ******************************************************************************/
    void paramTyper(const String& name, bool* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, char* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, int* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, int8_t*variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, int16_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, int32_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const String& name, unsigned int* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
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
  parameters.push(new LiveObjects_parameter(name, variable, type, variableType, callback));
}

template<typename LOtF>
inline void LiveObjectsBase::updateParameter(const LiveObjects_parameter param, LOtF* ptr, const JsonDocument& configIn, JsonDocument& configOut) {
  if (!configIn.isNull())
    *ptr = configIn[JSONCFG][param.label][JSONCFGVALUE].as<LOtF>();
  configOut[JSONCFG][param.label][JSONCFGVALUE] = *ptr;
}

template<typename LOtH>
inline void LiveObjectsBase::addToPayload(const String label, LOtH value) {
  easyDataPayload[JSONVALUE][label] = value;
}

template<typename T, typename ... Args>
inline void LiveObjectsBase::outputDebug(MSGTYPE type,T item, Args&... args)
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
  outputDebug(TEXT,args...);
}

extern const String SECRET_LIVEOBJECTS_API_KEY;

 /******************************************************************************
   NB LTE BOARDS CLASS
 ******************************************************************************/
#ifdef ARDUINO_SAMD_MKRNB1500
#include <MKRNB.h>

class LiveObjectsNB : public LiveObjectsBase
{
  public:
    static LiveObjectsNB& get()
    {
      static LiveObjectsNB g; return g;
    }

  private:
    LiveObjectsNB();
    ~LiveObjectsNB();
    LiveObjectsNB(const LiveObjectsNB&)  = delete;
    LiveObjectsNB& operator== (const LiveObjectsNB&) =  delete;
  public:
    void begin(Protocol p=MQTT, Security s=TLS, bool bDebug=true) override;
    void addNetworkInfo() override;

  private:
    void connectNetwork() override;
    void checkNetwork() override;
    void disconnectNetwork() override;
    static void messageCallback(int msg);
  private:
  NB m_NBAcces;
  NBScanner m_NBScanner;
};
<<<<<<< HEAD

typedef LiveObjectsNB LiveObjects;
#endif


/******************************************************************************
  GSM LTE BOARDS CLASS
******************************************************************************/
#ifdef ARDUINO_SAMD_MKRGSM1400
#include <MKRGSM.h>

class LiveObjectsGSM : public LiveObjectsBase
{
 public:
   static LiveObjectsGSM& get()
   {
     static LiveObjectsGSM g; return g;
   }

 private:
   LiveObjectsGSM();
   ~LiveObjectsGSM();
   LiveObjectsGSM(const LiveObjectsGSM&)  = delete;
   LiveObjectsGSM& operator== (const LiveObjectsGSM&) =  delete;
 public:
   void begin(Protocol p=MQTT, Security s=TLS, bool bDebug=true) override;
   void addNetworkInfo() override;

 private:
   void connectNetwork() override;
   void checkNetwork() override;
   void disconnectNetwork() override;
   static void messageCallback(int msg);
 private:
 GSM m_GSMAcces;
 GPRS m_GPRSAcces;
 GSMScanner m_GSMScanner;
};

=======
extern const String SECRET_PINNUMBER;
extern const String SECRET_APN;
extern const String SECRET_APN_USER;
extern const String SECRET_APN_PASS;
>>>>>>> Extracted arduino_secrets.h file into each of examples
typedef LiveObjectsGSM LiveObjects;
#endif

/******************************************************************************
  WIFI BOARDS CLASS
******************************************************************************/
#if defined ARDUINO_SAMD_MKRWIFI1010 || ARDUINO_SAMD_NANO_33_IOT
#include <WiFiNINA.h>
#define WIFI
#endif
#ifdef ARDUINO_SAMD_MKR1000
#include <WiFi101.h>
#define WIFI
#endif
#ifdef WIFI
class LiveObjectsWiFi : public LiveObjectsBase
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
    void begin(Protocol p=MQTT, Security s=TLS, bool bDebug=true) override;
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

extern LiveObjects& lo;
