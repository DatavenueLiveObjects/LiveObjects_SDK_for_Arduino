#pragma once

/******************************************************************************
   DEFAULT VALUES FOR LIVEOBJECTS
 ******************************************************************************/
#define PAYLOAD_DATA_SIZE 256
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
#define JSONMODELNAME "Orange_Pollution_Shield"


#include <Adafruit_SleepyDog.h>
#include <SoftwareSerial.h>
#include <Adafruit_FONA.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_FONA.h"
#include <ArduinoJson.h>

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

enum Encoding
{
  BINARY
  ,TEXT
};

typedef void (*onCommandCallback)(const String, String&);
class LiveObjectsFona
{
private:
struct LiveObjects_command
    {
      LiveObjects_command(String l, onCommandCallback c): label(l), callback(c){}
      bool operator==(const LiveObjects_command& p){ return label == p.label; }
      String label;
      onCommandCallback callback;
    };

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
  void sendData();
  void addCommand(char* name, onCommandCallback cb);
private:
  bool FONAconnect();
  void connectMQTT();
  void clearPayload();
  void publishMessage(const char* topic, JsonDocument& payload);
private:
  Adafruit_FONA m_Fona;
  SoftwareSerial m_FonaSerial;
  Adafruit_MQTT_FONA m_FonaMQTT;
  LiveObjects_command* m_Commands[MAX_COMMANDS];
  StaticJsonDocument<PAYLOAD_DATA_SIZE> m_Payload;
  Protocol m_Protocol;
  Encoding m_Encoding;
  char m_sClientID[16];
  char m_BufferPayload[PAYLOAD_DATA_SIZE];
  uint16_t m_nPort;
  int m_nSubs;
};

template<typename T>
inline void LiveObjectsFona::addToPayload(char * x, T y) 
{
  m_Payload[JSONVALUE][x] = y;
}

extern const char* SECRET_LIVEOBJECTS_API_KEY;

typedef LiveObjectsFona LiveObjects;