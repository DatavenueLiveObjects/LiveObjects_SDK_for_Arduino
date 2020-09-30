#include "LiveObjectsMKR.h"
#if defined ARDUINO_SAMD_MKRNB1500
#define NBD
#include <MKRNB.h>
#else
#define GSMD
#include <MKRGSM.h>
#endif
class LiveObjectsCellular : public LiveObjectsMKR
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
    void begin(Protocol p=MQTT, Encoding s=TEXT, bool bDebug=true) override;
    void addNetworkInfo() override;
    void sendData();
  private:
    String parseCommand(String inputString);
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
  String m_sNumber;
};

typedef LiveObjectsCellular LiveObjects;

extern const String SECRET_SERVER_MSISDN;
extern const String SECRET_PINNUMBER;
extern const String SECRET_APN;
extern const String SECRET_APN_USER;
extern const String SECRET_APN_PASS;