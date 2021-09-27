/*
 * Copyright (C) Orange
 *
 * This software is distributed under the terms and conditions of the 'MIT'
 * license which can be found in the file 'LICENSE.md' in this package distribution
 */
#include "LiveObjectsCellular.h"

#if defined NBD || defined GSMD
LiveObjectsCellular::LiveObjectsCellular()
  :
   LiveObjectsMKR()
  ,m_Acces()
  ,m_Scanner()
  ,m_Sms()
  #ifdef GSMD
  ,m_GPRSAcces()
  #endif
  #ifdef LIVE_OBJECTS_IOT_SAFE
  ,m_IoTSafe(IOT_SAFE_CUSTOM_AID, IOT_SAFE_CUSTOM_AID_LEN)
  #endif
{
  String num = SECRET_SERVER_MSISDN;
  for (int i = 0; i < num.length(); i+=2)
    {
        if ((i + 1) == num.length() && num.length() % 2 == 1)
        {
            num += num[i];
            num[i] = 'F';
        }
        else
        {
        char tmp = num[i + 1];
        num[i + 1] = num[i];
        num[i] = tmp;
        }
    }
    m_sNumber += ToHexT((char)(num.length() - 1));
    m_sNumber += "A1";
    m_sNumber += num;
}

LiveObjectsCellular::~LiveObjectsCellular()
{}

void LiveObjectsCellular::begin(Protocol p, Encoding s, bool bDebug)
{
  LiveObjectsBase::begin(p,s,bDebug);
  if(p==MQTT)
  {
    switch(m_Security)
    {
      #ifdef LIVE_OBJECTS_IOT_SAFE
      case MUTUAL_TLS_WITH_IOT_SAFE:
      #ifdef NBD
      m_pClient = new NBClient();
      #elif defined GSMD
      m_pClient = new GSMClient();
      #endif
      m_pBearSSLClient = new BearSSLClient(*m_pClient, TAs, 1);
      m_pMqttclient = new MqttClient(m_pBearSSLClient);
      m_nPort = 8883;
      break;
      #endif
      case TLS:
      #ifdef NBD
      m_pClient = new NBSSLClient();
      #elif defined GSMD
      m_pClient = new GSMSSLClient();
      #endif
      m_pMqttclient = new MqttClient(m_pClient);
      m_nPort = 8883;
      break;
      case NONE:
      #ifdef NBD
      m_pClient = new NBClient();
      #elif defined GSMD
      m_pClient = new GSMClient();
      #endif
      m_pMqttclient = new MqttClient(m_pClient);
      m_nPort = 1883;
      break;
      default:
      outputDebug(ERR,"Wrong mode! stopping...");
      while(true);
    }
    m_pMqttclient->onMessage(messageCallback);
  }
  else
  {
    if(SECRET_SERVER_MSISDN.length()<3)
    {
      outputDebug(ERR,"SERVER MSISDN is empty! Check arduino_secrets.h! Stopping here....");
      while(true);
    }
  }
  m_bInitialized = true;
}

#ifdef LIVE_OBJECTS_IOT_SAFE
unsigned long LiveObjectsCellular::getTime() {
  outputDebug(INFO,"Getting time from the cellular module...");
  // get the current time from the cellular module
  return m_Acces.getTime();
}

unsigned long LiveObjectsCellular::trampolineGetTime() {
  return LiveObjects::get().getTime();
}
#endif

void LiveObjectsCellular::connectNetwork()
{
  //Set client id as IMEI
  if (!m_bInitialized)
  {
    outputDebug(WARN,"missing begin() call, calling with default protcol=MQTT, security protcol=TLS, debug=true");
    begin();
  }
  #ifdef NBD
  if(m_Acces.status()!= NB_READY)
  #elif defined GSMD
  if(m_Acces.status()!= GSM_READY)
  #endif
  {
    #ifdef NBD
    NBModem modem;
    #elif defined GSMD
    GSMModem modem;
    #endif
    if(modem.begin())
    {
      #ifdef LIVE_OBJECTS_IOT_SAFE
      if (m_Security == MUTUAL_TLS_WITH_IOT_SAFE)
      {
        // Set a callback to get the current time used to validate the servers certificate
        ArduinoBearSSL.onGetTime(trampolineGetTime);

        // Wait a little before sending command to applet as we're getting strange
        // behavior otherwise (i.e. an empty file is returned)
        delay(2000);

        br_x509_certificate client_certificate = m_IoTSafe.readClientCertificate();
        m_pBearSSLClient->setEccCert(client_certificate);
        m_sMqttid = m_IoTSafe.getClientCertificateCommonName();
        outputDebug(INFO,"m_sMqttid retrieved from client certificate: ", m_sMqttid);
        m_pBearSSLClient->setEccSign(m_IoTSafe.trampolineSign);
      }
      #endif
      if(m_sMqttid.length()==0)
      {
        String imei="";
        for(int i=1;i<=3;i++)
        {
          imei=modem.getIMEI();
          if(imei.length()!=0) break;
          delay(100*i);
        }
        m_sMqttid = imei;
      }
    }
    else
    {
      outputDebug(ERR,"Failed to initialize modem!" );
      while(true);
    }

    outputDebug(INFO,"Connecting to cellular network");
    #ifdef NBD
    while (m_Acces.begin(SECRET_PINNUMBER.c_str(), SECRET_APN.c_str(), SECRET_APN_USER.c_str(), SECRET_APN_PASS.c_str()) != NB_READY)
      outputDebug(TXT,".");
    outputDebug();
    #elif defined GSMD
    while ((m_Acces.begin(SECRET_PINNUMBER.c_str()) != GSM_READY)) outputDebug(TXT, ".");
    if(m_Protocol != SMS) while ((m_GPRSAcces.attachGPRS(SECRET_APN.c_str(), SECRET_APN_USER.c_str(), SECRET_APN_PASS.c_str()) != GPRS_READY)) outputDebug(TXT, ".");
    outputDebug();
    #endif
    if(m_Encoding==BINARY) MODEM.send("AT+CMGF=0");
    delay(200);
    outputDebug(INFO,"You're connected to the network");
  }
  else
  {
      #ifdef NBD
      while (m_Acces.begin(SECRET_PINNUMBER.c_str(), SECRET_APN.c_str(), SECRET_APN_USER.c_str(), SECRET_APN_PASS.c_str()) != NB_READY)
      #elif defined GSMD
      if(m_Protocol==MQTT) while ((m_GPRSAcces.attachGPRS(SECRET_APN.c_str(), SECRET_APN_USER.c_str(), SECRET_APN_PASS.c_str()) != GPRS_READY)) outputDebug(TXT, ".");
      #endif
      delay(100);//outputDebug(INFO, "Sending atcmgf");
      if(m_Encoding==BINARY) MODEM.send("AT+CMGF=0");
      else MODEM.send("AT+CMGF=1");
      delay(200);
  }
  if(m_nPort==8883){
    if (!m_bCertLoaded) {
      int i;
      String response;
      for (i=0; i<(sizeof LO_ROOT_CERT / sizeof *LO_ROOT_CERT); ++i) {
        // get MD5 of LO_ROOT_CERT[i].name certificate
        MODEM.sendf("AT+USECMNG=4,0,\"%s\"", LO_ROOT_CERT[i].name);
        MODEM.waitForResponse(2000, &response);
        int rl = response.length();
        String md5=response.substring(rl-1-32, rl-1); // MD5 has 32-hex digits
        // uploading certificate if necessary
        if (md5!=LO_ROOT_CERT[i].md5) {
          outputDebug(INFO, "Loading Root CA certificate: ", LO_ROOT_CERT[i].name, ", size: " ,LO_ROOT_CERT[i].size, ", md5: " ,LO_ROOT_CERT[i].md5);
          MODEM.sendf("AT+USECMNG=0,0,\"%s\",%d", LO_ROOT_CERT[i].name, LO_ROOT_CERT[i].size);
          if (MODEM.waitForPrompt() != 1) {
            outputDebug(ERR,"Problem loading certificate!\nStopping here.");
            while (1) ;
          }
          else {
            MODEM.write(LO_ROOT_CERT[i].data, LO_ROOT_CERT[i].size);
            int ready;
            while (!MODEM.ready());
            m_bCertLoaded = true;
            outputDebug(INFO,"Certificate loaded");
          }
        }
        else {
          outputDebug(INFO,"Certificate ", LO_ROOT_CERT[i].name, " has been already uploaded");
        }
      }
    }
  }
  m_sModel = m_sMqttid;
}


void LiveObjectsCellular::checkNetwork()
{
  #ifdef NBD
  if(m_Acces.status()!= NB_READY)
    connectNetwork();
  #elif defined GSMD
  if(m_Acces.status()!= GSM_READY)
    connectNetwork();
  #endif
  if(m_Protocol == SMS)
  {
    String msg;
    if(m_Sms.available())
    {
      int c =0;
      while(true)
      {
        c = m_Sms.read();
        if(c==-1)break;
        msg+=(char)c;
      }
      String s;
      if(m_Encoding==TEXT) s = msg;
      else
      {
        s = parseCommand(msg);
        s = from7bit(s);
      }
      int spaceIndex = s.indexOf(' ');
      String cmd_name = s.substring(0,spaceIndex);
      s = s.substring(spaceIndex+1);
      outputDebug(INFO,"Received command: ",cmd_name);
      LiveObjects_command cmd(cmd_name,nullptr);
      int index = commands.find(&cmd);
      if(index >= 0 )
      {
        outputDebug(INFO,"Found command");
        commands[index]->callback(s,msg);
      }
      else outputDebug(INFO,"Unknown command");
      m_Sms.flush();
    }
  }
}

String LiveObjectsCellular::parseCommand(String inputString)
{
  String len; len += inputString[0]; len += inputString[1];
  int index = 2;
  index += 2 * strtol(len.c_str(), nullptr, 16);
  index += 2;
  len = ""; len += inputString[index]; index++; len += inputString[index]; index++;
  index += 2;
  index += (int)ceil(strtol(len.c_str(), nullptr, 16)/2.)*2;
  index += 2 * 9;
  len = ""; len += inputString[index]; index++; len += inputString[index]; index++;
  String retVal;
  retVal = inputString.substring(index);
  //outputDebug(INFO, "PARSED: ", retVal);
  return retVal;
}


void LiveObjectsCellular::disconnectNetwork()
{
  outputDebug(INFO,"Disconnecting from cellular network...");
  m_Acces.shutdown();
}

void LiveObjectsCellular::messageCallback(int msg)
{
  LiveObjects::get().onMQTTmessage(msg);
};

void LiveObjectsCellular::addNetworkInfo()
{
  String strength= m_Scanner.getSignalStrength();
  String carrier = m_Scanner.getCurrentCarrier();
  if(m_Protocol == MQTT && m_Encoding==TEXT)
  {
    bool status;
    #ifdef NBD
    status = m_Acces.status() == NB_READY;
    #elif defined GSMD
    status = m_Acces.status() == GSM_READY;
    #endif
    addToPayload(easyDataPayload[JSONVALUE].createNestedObject("networkInfo"),"connection_status",status,"strength",strength,"carrier",carrier);
  }
  #ifdef NBD
  else addToStringPayload((m_Acces.status() == NB_READY),strength,carrier);
  #elif defined GSMD
  else addToStringPayload(m_Acces.status() == GSM_READY,strength,carrier);
  #endif
}

void LiveObjectsCellular::sendData()
{
  if(m_Protocol == MQTT) LiveObjectsBase::sendData();
  else
  {
    if(m_sPayload.length() > 160)
    {
      outputDebug(ERR,"Payload to big, skipping sending...");
      return;
    }
    else if (m_sPayload.length() < 1)
    {
      outputDebug(WARN,"Payload is empty, skipping...");
      return;
    }
    if(m_Encoding==TEXT)
    {
      outputDebug(INFO,"Publishing message: ", m_sPayload);
      if(m_Sms.beginSMS(SECRET_SERVER_MSISDN.c_str())!=1) outputDebug(ERR,"Error occured while sending SMS");
      m_Sms.print(m_sPayload);
      if(m_Sms.endSMS()!=1) outputDebug(ERR,"Error occured while sending SMS");
    }
    else
    {
      outputDebug(INFO,"Payload before conversion: ",m_sPayload);
      String msg;
      static String fixedStart = "001100";
      static String fixedInfo = "0000FF";
      msg+=fixedStart;
      msg+=m_sNumber;
      msg+= fixedInfo;
      msg+=ToHex((char)m_sPayload.length());
      msg+=to7bit(m_sPayload);
      int msgSize = (msg.length()-2)/2;
      msg+='\x1A';
      outputDebug(INFO,"Message len: ",msgSize);
      outputDebug(INFO,"Publishing message: ", msg);
      MODEM.sendf("AT+CMGS=%d\r", msgSize);
      delay(100);
      MODEM.sendf(msg.c_str());

    }
    m_sPayload="";
  }
}

#endif
