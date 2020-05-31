/******************************************************************************
   DEFAULT VALUES
 ******************************************************************************/

#define PARAMETERS_NB_MAX 10
#define COMMANDS_NB_MAX 10
#define PAYLOAD_DEVMGT_SIZE 256
#define PAYLOAD_DATA_SIZE 512

#define KEEP_ALIVE_NETWORK 10000

#define SW_REVISION "1.8.0"

/******************************************************************************
   INCLUDES
 ******************************************************************************/

#include "LiveObjectsCert.h"

/******************************************************************************
   TYPEDEFS
 ******************************************************************************/

enum LiveObjects_networkStatus {
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

typedef void (*onParameterUpdateCallback)();
typedef void (*onCommandCallback)(const String, String&);

struct LiveObjects_parameter {
  const char* label;
  void *value;
  LiveObjects_parameterType type;
  LiveObjects_variableType variableType;
  onParameterUpdateCallback callback;
} parameters[PARAMETERS_NB_MAX];
uint8_t paramNb = 0;

struct LiveObjects_command {
  const char* label;
  onCommandCallback callback;
} commands[COMMANDS_NB_MAX];
uint8_t cmdNb = 0;

/******************************************************************************
   OBJECTS
 ******************************************************************************/

NB nbAccess;
#if defined MQTT_TLS
NBSSLClient nbClient;   // for TLS MQTT connection to Live Objects
#else
NBClient nbClient;      // for plain MQTT connection to Live Objects
#endif
MqttClient mqttClient(nbClient);

/******************************************************************************
   CONSTANTS
 ******************************************************************************/

// Live Objects constants
char mqtt_id[16];
const char mqtt_broker[] = "liveobjects.orange-business.com";
const char mqtt_user[] = "json+device";    // MQTT username for 'device' role
#if defined MQTT_TLS
const uint16_t mqtt_port = 8883;         // TLS MQTT port
#else
const uint16_t mqtt_port = 1883;         // default MQTT port
#endif
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
   VARIABLES
 ******************************************************************************/

bool DigiCert_rootCA_loaded = false;  // controls if the root CA certificate has already been stored in the SARA module
bool initialConfigDone = false;       // controls if the inital config has been sent to Live Objects
unsigned long lastKeepAliveMQTT = 0;
unsigned long lastKeepAliveNetwork = 0;
LiveObjects_networkStatus networkStatus = DISCONNECTED;
StaticJsonDocument<PAYLOAD_DATA_SIZE> easyDataPayload;

void publishMessage(const char* topic, JsonDocument& payload);

/******************************************************************************
   FUNCTION TEMPLATES
 ******************************************************************************/

template<typename LOtA>
void addParameter(const char* name, LOtA &variable) {
  onParameterUpdateCallback ptr = NULL;
  paramTyper(name, &variable, IMPLICIT, ptr);
}
template<typename LOtB>
void addParameter(const char* name, LOtB &variable, LiveObjects_parameterType type) {
  onParameterUpdateCallback ptr = NULL;
  paramTyper(name, &variable, type, ptr);
}
template<typename LOtC>
void addParameter(const char* name, LOtC &variable, onParameterUpdateCallback callback) {
  paramTyper(name, &variable, IMPLICIT, callback);
}
template<typename LOtD>
void addParameter(const char* name, LOtD &variable, onParameterUpdateCallback callback, LiveObjects_parameterType type) {
  paramTyper(name, &variable, type, callback);
}

template<typename LOtE>
void addTypedParam(const char* name, LOtE *variable, LiveObjects_parameterType type, LiveObjects_variableType variableType, onParameterUpdateCallback callback) {
  parameters[paramNb++] = (LiveObjects_parameter) { name, variable, type, variableType, callback };
}

template<typename LOtF>
void updateParameter(const LiveObjects_parameter param, LOtF* ptr, const JsonDocument& configIn, JsonDocument& configOut) {
  if (!configIn.isNull())
    *ptr = configIn[JSONcfg][param.label][JSONcfgValue].as<LOtF>();
  configOut[JSONcfg][param.label][JSONcfgValue] = *ptr;
}

template<typename LOtH>
void addToPayload(const String label, LOtH value) {
  easyDataPayload[JSONvalue][label] = value;
}

/******************************************************************************
   FUNCTIONS DECLARATION
 ******************************************************************************/

void LiveObjects_connect();
void networkCheck();
void LiveObjects_disconnect();
void onMQTTmessage(int messageSize);
void sendData();
void sendData(const String customPayload);
void publishMessage(const char* topic, JsonDocument& payload);
void LiveObjects_loop();

/******************************************************************************
   PARAM TYPERS
 ******************************************************************************/

void paramTyper(const char* name, bool* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, UNSIGNED_INTEGER, T_BOOL, callback);
  else
    addTypedParam(name, variable, type, T_BOOL, callback);
}
void paramTyper(const char* name, char* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, INTEGER, T_CHAR, callback);
  else
    addTypedParam(name, variable, type, T_CHAR, callback);
}
void paramTyper(const char* name, int* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, INTEGER, T_INT, callback);
  else
    addTypedParam(name, variable, type, T_INT, callback);
}
void paramTyper(const char* name, int8_t*variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, INTEGER, T_INT8, callback);
  else
    addTypedParam(name, variable, type, T_INT8, callback);
}
void paramTyper(const char* name, int16_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, INTEGER, T_INT16, callback);
  else
    addTypedParam(name, variable, type, T_INT16, callback);
}
void paramTyper(const char* name, int32_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, INTEGER, T_INT32, callback);
  else
    addTypedParam(name, variable, type, T_INT32, callback);
}
void paramTyper(const char* name, unsigned int* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, UNSIGNED_INTEGER, T_UINT, callback);
  else
    addTypedParam(name, variable, type, T_UINT, callback);
}
void paramTyper(const char* name, uint8_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, UNSIGNED_INTEGER, T_UINT8, callback);
  else
    addTypedParam(name, variable, type, T_UINT8, callback);
}
void paramTyper(const char* name, uint16_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, UNSIGNED_INTEGER, T_UINT16, callback);
  else
    addTypedParam(name, variable, type, T_UINT16, callback);
}
void paramTyper(const char* name, uint32_t* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, UNSIGNED_INTEGER, T_UINT32, callback);
  else
    addTypedParam(name, variable, type, T_UINT32, callback);
}
void paramTyper(const char* name, double* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, DECIMAL, T_DOUBLE, callback);
  else
    addTypedParam(name, variable, type, T_DOUBLE, callback);
}
void paramTyper(const char* name, float* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, DECIMAL, T_FLOAT, callback);
  else
    addTypedParam(name, variable, type, T_FLOAT, callback);
}
void paramTyper(const char* name, String* variable, LiveObjects_parameterType type, onParameterUpdateCallback callback) {
  if (type == IMPLICIT)
    addTypedParam(name, variable, STRING, T_STRING, callback);
  else
    addTypedParam(name, variable, type, T_STRING, callback);
}

/******************************************************************************
   POINTER TYPERS
 ******************************************************************************/

void ptrTyperBool(const LiveObjects_parameter param, const JsonDocument& configIn, JsonDocument& configOut) {
  bool* ptr = (bool*)param.value;
  updateParameter(param, ptr, configIn, configOut);
}
void ptrTyperChar(const LiveObjects_parameter param, const JsonDocument& configIn, JsonDocument& configOut) {
  char* ptr = (char*)param.value;
  updateParameter(param, ptr, configIn, configOut);
}
void ptrTyperInt(const LiveObjects_parameter param, const JsonDocument& configIn, JsonDocument& configOut) {
  int* ptr = (int*)param.value;
  updateParameter(param, ptr, configIn, configOut);
}
void ptrTyperInt8(const LiveObjects_parameter param, const JsonDocument& configIn, JsonDocument& configOut) {
  int8_t* ptr = (int8_t*)param.value;
  updateParameter(param, ptr, configIn, configOut);
}
void ptrTyperInt16(const LiveObjects_parameter param, const JsonDocument& configIn, JsonDocument& configOut) {
  int16_t* ptr = (int16_t*)param.value;
  updateParameter(param, ptr, configIn, configOut);
}
void ptrTyperInt32(const LiveObjects_parameter param, const JsonDocument& configIn, JsonDocument& configOut) {
  int32_t* ptr = (int32_t*)param.value;
  updateParameter(param, ptr, configIn, configOut);
}
void ptrTyperUint(const LiveObjects_parameter param, const JsonDocument& configIn, JsonDocument& configOut) {
  unsigned int* ptr = (unsigned int*)param.value;
  updateParameter(param, ptr, configIn, configOut);
}
void ptrTyperUint8(const LiveObjects_parameter param, const JsonDocument& configIn, JsonDocument& configOut) {
  uint8_t* ptr = (uint8_t*)param.value;
  updateParameter(param, ptr, configIn, configOut);
}
void ptrTyperUint16(const LiveObjects_parameter param, const JsonDocument& configIn, JsonDocument& configOut) {
  uint16_t* ptr = (uint16_t*)param.value;
  updateParameter(param, ptr, configIn, configOut);
}
void ptrTyperUint32(const LiveObjects_parameter param, const JsonDocument& configIn, JsonDocument& configOut) {
  uint32_t* ptr = (uint32_t*)param.value;
  updateParameter(param, ptr, configIn, configOut);
}
void ptrTyperDouble(const LiveObjects_parameter param, const JsonDocument& configIn, JsonDocument& configOut) {
  double* ptr = (double*)param.value;
  updateParameter(param, ptr, configIn, configOut);
}
void ptrTyperFloat(const LiveObjects_parameter param, const JsonDocument& configIn, JsonDocument& configOut) {
  float* ptr = (float*)param.value;
  updateParameter(param, ptr, configIn, configOut);
}
void ptrTyperString(const LiveObjects_parameter param, const JsonDocument& configIn, JsonDocument& configOut) {
  String* ptr = (String*)param.value;
  if (!configIn.isNull())
    *ptr = configIn[JSONcfg][param.label][JSONcfgValue].as<String>();
  configOut[JSONcfg][param.label][JSONcfgValue] = (*ptr).c_str();
}

void ptrTyperInit(const LiveObjects_parameter param, const JsonDocument& configIn, JsonDocument& configOut) {
  switch (param.variableType) {
    case T_BOOL:
      ptrTyperBool(param, configIn, configOut);
      break;
    case T_CHAR:
      ptrTyperChar(param, configIn, configOut);
      break;
    case T_INT:
      ptrTyperInt(param, configIn, configOut);
      break;
    case T_INT8:
      ptrTyperInt8(param, configIn, configOut);
      break;
    case T_INT16:
      ptrTyperInt16(param, configIn, configOut);
      break;
    case T_INT32:
      ptrTyperInt32(param, configIn, configOut);
      break;
    case T_UINT:
      ptrTyperUint(param, configIn, configOut);
      break;
    case T_UINT8:
      ptrTyperUint8(param, configIn, configOut);
      break;
    case T_UINT16:
      ptrTyperUint16(param, configIn, configOut);
      break;
    case T_UINT32:
      ptrTyperUint32(param, configIn, configOut);
      break;
    case T_DOUBLE:
      ptrTyperDouble(param, configIn, configOut);
      break;
    case T_FLOAT:
      ptrTyperFloat(param, configIn, configOut);
      break;
    case T_STRING:
      ptrTyperString(param, configIn, configOut);
      break;
  }
}

/******************************************************************************
   CONFIGURATION MANAGER
 ******************************************************************************/

void configurationManager(int messageSize = -1) {
  StaticJsonDocument<PAYLOAD_DEVMGT_SIZE> configOut;
  if (messageSize >= 0) { // config update received
    StaticJsonDocument<PAYLOAD_DEVMGT_SIZE> configIn;
    deserializeJson(configIn, mqttClient);

    serializeJsonPretty(configIn, Serial);
    Serial.print('\n');

    configOut = configIn;
    for (uint8_t i = 0; i < paramNb; i++)
      if (configIn[JSONcfg].containsKey(parameters[i].label)) {
        ptrTyperInit(parameters[i], configIn, configOut);
        if (parameters[i].callback != NULL)
          parameters[i].callback();
      }
    publishMessage(mqtt_pubcfg, configOut);
  }
  else { // messageSize==-1, compose & send initial config
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
      ptrTyperInit(parameters[i], configIn, configOut);
    }
    publishMessage(mqtt_pubcfg, configOut);
  }
}

/******************************************************************************
   COMMAND MANAGEMENT
 ******************************************************************************/

void addCommand(const char* name, onCommandCallback callback) {
  commands[cmdNb++] = (LiveObjects_command) {
    name, callback
  };
}

void commandManager() {
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

void LiveObjects_connect() {
  if (!initialConfigDone) {
    NBModem modem;
    if(modem.begin()) {
      String imei = modem.getIMEI();
      strncpy(mqtt_id, imei.c_str(), 16);
    }
    else
      strncpy(mqtt_id, "MKR1500NB", 16);
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

  mqttClient.subscribe(mqtt_subcfg);
  mqttClient.subscribe(mqtt_subcmd);
  
  if (!initialConfigDone) {
    configurationManager();
    initialConfigDone = true;
  }

  mqttClient.poll();
}

void networkCheck() {
  unsigned long now = millis();
  if (now - lastKeepAliveNetwork > KEEP_ALIVE_NETWORK)
    if (nbAccess.status() != NB_READY || !mqttClient.connected())
      LiveObjects_connect();
}

void LiveObjects_disconnect() {
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

void onMQTTmessage(int messageSize) {
  String topic = mqttClient.messageTopic();

  Serial.print("Received a message on topic '");
  Serial.print(topic);
  Serial.println("':");

  if (topic == mqtt_subcfg)
    configurationManager(messageSize);
  else if (topic == mqtt_subcmd)
    commandManager();
}

void sendData() {
  easyDataPayload[JSONmodel] = JSONmodelName;
  publishMessage(mqtt_pubdata, easyDataPayload);
  easyDataPayload.clear();
}

void sendData(const String customPayload) {
  StaticJsonDocument<PAYLOAD_DATA_SIZE> payload;
  deserializeJson(payload, customPayload);
  if (!payload.containsKey(JSONmodel))
    payload[JSONmodel] = JSONmodelName;
  publishMessage(mqtt_pubdata, payload);
}

void publishMessage(const char* topic, JsonDocument& payload) {
  LiveObjects_networkStatus lastStatus = networkStatus;
  if (networkStatus != CONNECTED)
    LiveObjects_connect();

  Serial.print("Publishing message on topic '");
  Serial.print(topic);
  Serial.println("':");
  serializeJsonPretty(payload, Serial);
  Serial.print('\n');
  mqttClient.beginMessage(topic);
  serializeJson(payload, mqttClient);
  mqttClient.endMessage();

  if (lastStatus == DISCONNECTED)
    LiveObjects_disconnect();
}

/******************************************************************************
   LIVE OBJECTS
 ******************************************************************************/

void LiveObjects_loop() {
  if (networkStatus == CONNECTED) {
    networkCheck();
    mqttClient.poll();
  }
}
