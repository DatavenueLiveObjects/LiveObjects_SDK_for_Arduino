#pragma once

/******************************************************************************
   DEFAULT VALUES
 ******************************************************************************/

#define PARAMETERS_NB_MAX 10
#define COMMANDS_NB_MAX 10
#define PAYLOAD_DEVMGT_SIZE 256
#define PAYLOAD_DATA_SIZE 512
#define MQTT_CLIENT_ID_LENGTH 16
#define KEEP_ALIVE_NETWORK 10000
#define SW_REVISION "1.8.0"


/******************************************************************************
   INCLUDES
 ******************************************************************************/
#include <ArduinoMqttClient.h>
#include <MKRNB.h>
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

class LiveObjects
{
public:
  static LiveObjects& get(Protocol p=MQTT, Security s=TLS, bool bDebug=true)
  {
     static LiveObjects lo(p,s,bDebug);
     return lo;
  }

private:
  LiveObjects(Protocol p, Security s, bool bDebug);
  ~LiveObjects();
  LiveObjects(const LiveObjects&) = delete;
  LiveObjects& operator=(const LiveObjects&) = delete;

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
   CONSTANTS
******************************************************************************/
private:
    // Live Objects constants
    char mqtt_id[MQTT_CLIENT_ID_LENGTH];
    uint16_t mqtt_port=1883;                  // default MQTT port
    const char* mqtt_broker = "liveobjects.orange-business.com";
    const char* mqtt_user = "json+device";    // MQTT username for 'device' role
    const char* mqtt_pubdata = "dev/data";     // topic for publishing data
    const char* mqtt_subcfg = "dev/cfg/upd";   // subscribed topic for configuration updates
    const char* mqtt_pubcfg = "dev/cfg";       // topic used to publish confuguration
    const char* mqtt_subcmd = "dev/cmd";       // subscribed topic for commands
    const char* mqtt_pubcmd = "dev/cmd/res";   // topic used to confirm commands

    // Live Objects' JSON constants
    const char* JSONcid = "cid";
    const char* JSONcfg = "cfg";
    const char* JSONcfgValue = "v";
    const char* JSONcfgType = "t";
    const char* JSONmodel = "model";
    const char* JSONvalue = "value";
    const char* JSONmodelName = "github_sample_MKRNB";

/******************************************************************************
   FUNCTIONS
******************************************************************************/

public:
   void setProtocol(Protocol p);
   void setSecurity(Security s);
   void enableDebug(bool b);
   void setClientID(const char* id);

public:
   void addTimestamp(time_t timestamp);
   void addLocation(double lat, double lon, float alt);
   void addNetworkInfo();
   void clearPayload();
public:
  bool debugEnabled();
public:
    void publishMessage(const char* topic, JsonDocument& payload);
    void connect();
    void networkCheck();
    void disconnect();
    void onm(int message) { onMQTTmessage(message); }
    static void onMQTTmessage(int messageSize);
    void sendData();
    void sendData(const String customPayload);
    void loop();


private:
 /******************************************************************************
   CONFIGURATION MANAGER
 ******************************************************************************/
    void configurationManager(int messageSize = -1);
/******************************************************************************
   COMMAND MANAGEMENT
 ******************************************************************************/
    void addCommand(const char* name, onCommandCallback callback);
    void commandManager();

/******************************************************************************
   PRIVATE FUNCTIONS
 ******************************************************************************/
    template<typename T>
    void outputDebug(T buf);
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
private:
    LiveObjects_command commands[COMMANDS_NB_MAX];
    LiveObjects_parameter parameters[PARAMETERS_NB_MAX];
    LiveObjects_networkStatus networkStatus = DISCONNECTED;
    StaticJsonDocument<PAYLOAD_DATA_SIZE> easyDataPayload;
    NB nbAccess;
    NBClient client;
    NBSSLClient tlsClient;
    MqttClient *mqttClient;
/******************************************************************************
   VARIABLES
******************************************************************************/
private:
    uint8_t paramNb = 0;
    uint8_t cmdNb = 0;
    bool DigiCert_rootCA_loaded = false;  // controls if the root CA certificate has already been stored in the SARA module
    bool initialConfigDone = false;       // controls if the inital config has been sent to Live Objects
    unsigned long lastKeepAliveMQTT = 0;
    unsigned long lastKeepAliveNetwork = 0;
    bool m_bDebug;

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
inline void LiveObjects::addParameter(const char* name, LOtA &variable) {
  onParameterUpdateCallback ptr = NULL;
  paramTyper(name, &variable, IMPLICIT, ptr);
}
template<typename LOtB>
inline void LiveObjects::addParameter(const char* name, LOtB &variable, LiveObjects_parameterType type) {
  onParameterUpdateCallback ptr = NULL;
  paramTyper(name, &variable, type, ptr);
}
template<typename LOtC>
inline void LiveObjects::addParameter(const char* name, LOtC &variable, onParameterUpdateCallback callback) {
  paramTyper(name, &variable, IMPLICIT, callback);
}
template<typename LOtD>
inline void LiveObjects::addParameter(const char* name, LOtD &variable, onParameterUpdateCallback callback, LiveObjects_parameterType type) {
  paramTyper(name, &variable, type, callback);
}

template<typename LOtE>
inline void LiveObjects::addTypedParam(const char* name, LOtE *variable, LiveObjects_parameterType type, LiveObjects_variableType variableType, onParameterUpdateCallback callback) {
  parameters[paramNb++] = (LiveObjects_parameter) { name, variable, type, variableType, callback };
}

template<typename LOtF>
inline void LiveObjects::updateParameter(const LiveObjects_parameter param, LOtF* ptr, const JsonDocument& configIn, JsonDocument& configOut) {
  if (!configIn.isNull())
    *ptr = configIn[JSONcfg][param.label][JSONcfgValue].as<LOtF>();
  configOut[JSONcfg][param.label][JSONcfgValue] = *ptr;
}

template<typename LOtH>
inline void LiveObjects::addToPayload(const String label, LOtH value) {
  easyDataPayload[JSONvalue][label] = value;
}

template<typename T>
inline void LiveObjects::outputDebug(T buf)
{
  if(m_bDebug) Serial.println(buf);
}
