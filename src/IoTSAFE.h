/*
 * Copyright (C) Orange
 *
 * This software is distributed under the terms and conditions of the 'MIT'
 * license which can be found in the file 'LICENSE.md' in this package distribution
 */
#pragma once

#ifdef LIVE_OBJECTS_IOT_SAFE

#include <iot_safe.h>
#include <ArduinoBearSSL.h>

#define IOT_SAFE_MAX_FILE_SIZE 512

class IoTSAFE
{
  public:
    IoTSAFE(const uint8_t *pAID=IOT_SAFE_AID,
      uint8_t nAIDLength=sizeof(IOT_SAFE_AID));
    ~IoTSAFE();
    br_x509_certificate readClientCertificate();
    String getClientCertificateCommonName();
    size_t sign(const br_ec_impl *pImpl, const br_hash_class *pHF,
      const void *pHashValue, const br_ec_private_key *pSk, void *pSig);
    static size_t trampolineSign(const br_ec_impl *pImpl, const br_hash_class *pHF,
      const void *pHashValue, const br_ec_private_key *pSk, void *pSig);
  private:
    iot_safe_error_t init();
    void finish();
  private:
    const uint8_t *m_AID;
    uint8_t m_nAIDLength;
    uint8_t m_nChannel;
    uint8_t m_ClientCertificate[IOT_SAFE_MAX_FILE_SIZE];
};

extern const uint8_t IOT_SAFE_CUSTOM_AID[];
extern const uint8_t IOT_SAFE_CUSTOM_AID_LEN;
extern const uint8_t IOT_SAFE_PRIVATE_KEY_ID[];
extern const uint8_t IOT_SAFE_PRIVATE_KEY_ID_LEN;
extern const uint8_t IOT_SAFE_CLIENT_CERTIFICATE_FILE_ID[];
extern const uint8_t IOT_SAFE_CLIENT_CERTIFICATE_FILE_ID_LEN;

#endif
