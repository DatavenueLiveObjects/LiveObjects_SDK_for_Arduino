#include "LiveObjectsMKR.h"
#if defined ARDUINO_SAMD_MKRWIFI1010 || defined ARDUINO_SAMD_NANO_33_IOT || defined ARDUINO_SAMD_MKRVIDOR4000
#include <WiFiNINA.h>
#else
#include <WiFi101.h>
#endif
class LiveObjectsWiFi : public LiveObjectsMKR
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
    void begin(Protocol p=MQTT, Encoding s=TEXT, bool bDebug=true) override;
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