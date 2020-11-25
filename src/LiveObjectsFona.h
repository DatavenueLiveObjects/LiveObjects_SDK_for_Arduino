/*
 * Copyright (C) Orange
 *
 * This software is distributed under the terms and conditions of the 'MIT'
 * license which can be found in the file 'LICENSE.md' in this package distribution
 */
#pragma once

#ifdef ARDUINO_ARCH_AVR
#define FONAD
/******************************************************************************
   DEFAULT VALUES FOR LIVEOBJECTS
 ******************************************************************************/
#define PAYLOAD_DATA_SIZE 200
#define KEEP_ALIVE_NETWORK 1000
#define SW_REVISION "1.8.0"
#define MAX_COMMANDS 10

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

#include <SoftwareSerial.h>
#include <Adafruit_FONA.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_FONA.h"
#include <ArduinoJson.h>
#include <string.h>
typedef long long time_t;
#ifdef ARDUINO_AVR_FEATHER32U4
  #define FONA_RX  9
  #define FONA_TX  8
  #define FONA_RST 4
#endif
//Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, "dev/cmd");
#define SW_REVISION "1.8.0"
#define PAYLOAD_DATA_SIZE 256

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

enum Encoding
{
  BINARY
  ,TEXT
};

typedef void (*onCommandCallback)(const String, String&);
typedef void (*onParameterUpdateCallback)();

class LiveObjectsFona
{
private:
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
// struct LiveObjects_command
//     {
//       LiveObjects_command(String l, onCommandCallback c): label(l), callback(c){}
//       bool operator==(const LiveObjects_command& p){ return label == p.label; }
//       String label;
//       onCommandCallback callback;
//     };

  public:
  static LiveObjectsFona& get()
  {
    static LiveObjectsFona f;
    return f;
  }
private:
  LiveObjectsFona();
  LiveObjectsFona(const LiveObjectsFona&) = delete;
  LiveObjectsFona& operator==(const LiveObjectsFona&) = delete;

public:
  void begin(Protocol p, Encoding e, bool d);
  void loop();
  void connect();
  template<typename T>
  void addToPayload(char* x, T y);
  template<typename T,typename ... Args>
  void addToStringPayload(T val, Args ... args);
  void addToStringPayload(){};
  void sendData();
  void addCommand(char* name, onCommandCallback cb);

  void changeConfiguration(Protocol p,Security s, Encoding mode);
  void setSecurity(Security s);
  void enableDebug(bool b);
  void setDecoder(char* s);
  void setModel(char* s);
  void addTimestamp(time_t timestamp);
  void addLocation(double lat, double lon, double alt);
  void addPowerStatus();
  void addNetworkInfo();

  template<typename LOtA>
  void addParameter(const String name, LOtA &variable){}//Serial.print("[ERROR] This board doesn't support this function");};
  template<typename LOtB>
  void addParameter(const String name, LOtB &variable, LiveObjects_parameterType type){}//Serial.print("[ERROR] This board doesn't support this function");};
  template<typename LOtC>
  void addParameter(const String name, LOtC &variable, onParameterUpdateCallback callback){}//Serial.print("[ERROR] This board doesn't support this function");};
  template<typename LOtD>
  void addParameter(const String name, LOtD &variable, onParameterUpdateCallback callback, LiveObjects_parameterType type){}//Serial.print("[ERROR] This board doesn't support this function");};

private:
  bool FONAconnect();
  void connectMQTT();
  void clearPayload();
  void publishMessage(const char* topic, JsonDocument& payload);
private:
  Adafruit_FONA m_Fona;
  SoftwareSerial m_FonaSerial;
  Adafruit_MQTT_FONA m_FonaMQTT;
  StaticJsonDocument<PAYLOAD_DATA_SIZE> m_Payload;
  Protocol m_Protocol;
  Encoding m_Encoding;
  Security m_Security;
  char m_sClientID[16];
  char m_sModel[16];
  char m_BufferPayload[PAYLOAD_DATA_SIZE];
  uint16_t m_nPort;
  unsigned long lastKeepAliveNetwork;
};

template<typename T>
inline void LiveObjectsFona::addToPayload(char * x, T y) 
{
  if(m_Protocol == MQTT) m_Payload[JSONVALUE][x] = y;
  else addToStringPayload(x, y);
}

template<typename T,typename ... Args>
void LiveObjectsFona::addToStringPayload(T val, Args ... args)
{
  String tmp = "";
  tmp+=val;
  tmp+=';';
  strcat(m_BufferPayload, tmp.c_str());  
  addToStringPayload(args...);
}

extern const char* SECRET_LIVEOBJECTS_API_KEY;
extern const String SECRET_SERVER_MSISDN;
// extern const String SECRET_PINNUMBER;
// extern const String SECRET_APN;
// extern const String SECRET_APN_USER;
// extern const String SECRET_APN_PASS;

typedef LiveObjectsFona LiveObjects;
#endif