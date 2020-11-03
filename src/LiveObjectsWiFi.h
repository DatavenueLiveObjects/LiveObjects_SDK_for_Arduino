#if defined ARDUINO_SAMD_MKRWIFI1010 || defined ARDUINO_SAMD_NANO_33_IOT || defined ARDUINO_SAMD_MKRVIDOR4000
  #ifdef ARDUINO_SAMD_MKRVIDOR4000
    #include <VidorPeripherals.h>
  #endif
  #include "LiveObjectsMKR.h"
  #define WIFID
  #include <WiFiNINA.h>
#elif defined ARDUINO_SAMD_MKR1000 || ADAFRUIT_FEATHER_M0
  #include "LiveObjectsMKR.h"
  #define WIFID
  #include <WiFi101.h>
#endif

#ifdef WIFID
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
#endif