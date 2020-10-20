#include "LiveObjectsBase.h"
#include <Adafruit_SleepyDog.h>
#include <SoftwareSerial.h>
#include <Adafruit_FONA.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_FONA.h>
#define FONAD

#ifdef ARDUINO_AVR_FEATHER32U4
  #define FONA_RX  9
  #define FONA_TX  8
  #define FONA_RST 4
#endif


#ifdef FONAD
class LiveObjectsFONA : public LiveObjectsBase
{
public:
  static LiveObjectsFONA& get(){
    static LiveObjectsFONA g;
    return g;
  }

  LiveObjectsFONA();
  ~LiveObjectsFONA();

  private:
    void connectNetwork() override;
    void checkNetwork() override;
    void disconnectNetwork() override;

  protected:
    void checkMQTT() override;
    void connectMQTT() override;
    void disconnectMQTT() override;
    void stopMQTT()override;
    void sendMQTT(String& topic, JsonDocument& doc)override;
    void sendMQTT(String& topic, String& doc)override;
    void deserializeMessage(JsonDocument& doc)override;

  public:
    void begin(Protocol p=MQTT, Encoding s=TEXT, bool bDebug=true) override;
    void addPowerStatus()override;
    void addNetworkInfo() override;

    bool connectFONA();

  void sendData();

  private:
  SoftwareSerial m_FonaSerial;
  Adafruit_FONA m_Fona;
  Adafruit_MQTT_FONA* m_FonaMQTT;
  Adafruit_MQTT_Subscribe* m_subCommands;
  Adafruit_MQTT_Subscribe* m_subParameters;
  Adafruit_MQTT_Subscribe* m_subLast;
  String m_sNumber;
};

boolean FONAconnect(const __FlashStringHelper *apn, const __FlashStringHelper *username, const __FlashStringHelper *password);

extern const String SECRET_SERVER_MSISDN;
extern const String SECRET_PINNUMBER;
extern const String SECRET_APN;
extern const String SECRET_APN_USER;
extern const String SECRET_APN_PASS;

typedef LiveObjectsFONA LiveObjects;
#endif