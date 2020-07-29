#pragma once
/******************************************************************************
   DEFAULT VALUES
 ******************************************************************************/
#define PARAMETERS_NB_MAX 10
#define COMMANDS_NB_MAX 10
#define PAYLOAD_DEVMGT_SIZE 256
#define PAYLOAD_DATA_SIZE 512
#define KEEP_ALIVE_NETWORK 10000
#define SW_REVISION "1.8.0"

#define MQTT_BROKER "liveobjects.orange-business.com"
#define MQTT_USER "json+device"
#define MQTT_PUBDATA "dev/data"
#define MQTT_SUBCFG "dev/cfg/upd"
#define MQTT_PUBCFG "dev/cfg"
#define MQTT_SUBCMD "dev/cmd"
#define MQTT_PUBCMD "dev/cmd/res"

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
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>
#include <ctime>
#include "arduino_secrets.h"
#include "LiveObjectsCert.h"

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
        const char* label;
        void *value;
        LiveObjects_parameterType type;
        LiveObjects_variableType variableType;
        onParameterUpdateCallback callback;
    };
    struct LiveObjects_command
    {
        const char* label;
        onCommandCallback callback;
    };

/******************************************************************************
   FUNCTIONS
******************************************************************************/

public:
  void setProtocol(Protocol p);
  void setSecurity(Security s);
  void enableDebug(bool b);
  void setClientID(const char* id);

public:
  bool debugEnabled();

public:
  void addTimestamp(time_t timestamp);
  void addLocation(double lat, double lon, float alt);
  virtual void addNetworkInfo()=0;
  void clearPayload();

public:
  void publishMessage(const char* topic, JsonDocument& payload);
  void connect();
  void networkCheck();
  void disconnect();
  void onMQTTmessage(int messageSize);
  void sendData();
  void sendData(const String customPayload);
  void addCommand(const char* name, onCommandCallback callback);
  void loop();

protected:
  virtual void begin(Protocol, Security, bool) =0;
  virtual void connectNetwork() =0;
  virtual void checkNetwork() =0;
  virtual void disconnectNetwork() =0;

protected:
  template<typename T>
  void outputDebug(T buf);

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
    void addParameter(const char* name, LOtA &variable);
    template<typename LOtB>
    void addParameter(const char* name, LOtB &variable, LiveObjects_parameterType type);
    template<typename LOtC>
    void addParameter(const char* name, LOtC &variable, onParameterUpdateCallback callback);
    template<typename LOtD>
    void addParameter(const char* name, LOtD &variable, onParameterUpdateCallback callback, LiveObjects_parameterType type);
    template<typename LOtE>
    void addTypedParam(const char* name, LOtE *variable, LiveObjects_parameterType type, LiveObjects_variableType variableType, onParameterUpdateCallback callback);
    template<typename LOtF>
    void updateParameter(const LiveObjects_parameter param, LOtF* ptr, const JsonDocument& configIn, JsonDocument& configOut);
    template<typename LOtH>
    void addToPayload(const String label, LOtH value);
/******************************************************************************
   OBJECTS
******************************************************************************/
protected:
    LiveObjects_command commands[COMMANDS_NB_MAX];
    LiveObjects_parameter parameters[PARAMETERS_NB_MAX];
    LiveObjects_networkStatus networkStatus = DISCONNECTED;
    StaticJsonDocument<PAYLOAD_DATA_SIZE> easyDataPayload;
/******************************************************************************
   VARIABLES
******************************************************************************/
private:
    uint8_t paramNb =  0;
    uint8_t cmdNb  = 0;
    unsigned long lastKeepAliveMQTT =  0;
    unsigned long lastKeepAliveNetwork  = 0;

protected:
    Client* m_pClient;
    MqttClient *m_pMqttclient;
    String m_sMqttid;
    uint16_t m_nPort;
    bool m_bInitialMqttConfig;
    bool m_bDebug;
    bool m_bCertLoaded;
    bool m_bInitialized;
/******************************************************************************
   PARAM TYPERS
 ******************************************************************************/
    void paramTyper(const char* name, bool* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const char* name, char* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const char* name, int* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const char* name, int8_t*variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const char* name, int16_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const char* name, int32_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const char* name, unsigned int* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const char* name, uint8_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const char* name, uint16_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const char* name, uint32_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const char* name, double* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const char* name, float* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);
    void paramTyper(const char* name, String* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback);

/******************************************************************************
   POINTER TYPER
 ******************************************************************************/
   void ptrTyper(const LiveObjects_parameter param, const JsonDocument& configIn, JsonDocument& configOut);
};

template<typename LOtA>
inline void LiveObjectsBase::addParameter(const char* name, LOtA &variable) {
  onParameterUpdateCallback ptr = NULL;
  paramTyper(name, &variable, IMPLICIT, ptr);
}
template<typename LOtB>
inline void LiveObjectsBase::addParameter(const char* name, LOtB &variable, LiveObjects_parameterType type) {
  onParameterUpdateCallback ptr = NULL;
  paramTyper(name, &variable, type, ptr);
}
template<typename LOtC>
inline void LiveObjectsBase::addParameter(const char* name, LOtC &variable, onParameterUpdateCallback callback) {
  paramTyper(name, &variable, IMPLICIT, callback);
}
template<typename LOtD>
inline void LiveObjectsBase::addParameter(const char* name, LOtD &variable, onParameterUpdateCallback callback, LiveObjects_parameterType type) {
  paramTyper(name, &variable, type, callback);
}

template<typename LOtE>
inline void LiveObjectsBase::addTypedParam(const char* name, LOtE *variable, LiveObjects_parameterType type, LiveObjects_variableType variableType, onParameterUpdateCallback callback) {
  parameters[paramNb++] = (LiveObjects_parameter) { name, variable, type, variableType, callback };
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

template<typename T>
inline void LiveObjectsBase::outputDebug(T buf)
{
  if(m_bDebug) Serial.println(buf);
}


 /******************************************************************************
   GSM BOARDS CLASS
 ******************************************************************************/
#ifdef ARDUINO_SAMD_MKRNB1500
#include <MKRNB.h>

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
    void begin(Protocol p, Security s, bool bDebug) override;
    void addNetworkInfo() override;

  private:
    void connectNetwork() override;
    void checkNetwork() override;
    void disconnectNetwork() override;
    static void messageCallback(int msg);
  private:
  NB m_NBAcces;
};

typedef LiveObjectsGSM LiveObjects;
#endif

/******************************************************************************
  WIFI BOARDS CLASS
******************************************************************************/
#ifdef ARDUINO_SAMD_MKRWIFI1010
#include <WiFiNINA.h>
class LiveObjectsWiFi101 : public LiveObjectsBase
{
  public:
    static LiveObjectsWiFi101& get()
    {
      static LiveObjectsWiFi101 w; return w;
    }

  private:
    LiveObjectsWiFi101();
    ~LiveObjectsWiFi101();
    LiveObjectsWiFi101(const LiveObjectsWiFi101&)  = delete;
    LiveObjectsWiFi101& operator== (const LiveObjectsWiFi101&) =  delete;
  public:
    void begin(Protocol p, Security s, bool bDebug) override;
    void addNetworkInfo() override;
  private:
    void connectNetwork() override;
    void checkNetwork() override;
    void disconnectNetwork() override;
    static void messageCallback(int msg);
  private:
  String m_sSSID;
  String m_sPassword;
};
typedef LiveObjectsWiFi101 LiveObjects;
#endif

#ifndef DO_NOT_DEFINE_LO
extern LiveObjects& lo =  LiveObjects::get();
#endif
