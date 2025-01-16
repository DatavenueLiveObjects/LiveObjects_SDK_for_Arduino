	// Cellular connection cerdentials, used only for GSM boards
extern const String SECRET_PINNUMBER = "";		// unless PIN is deactivated, specify your SIM card PIN number
extern const String SECRET_APN = "";					// specify the APN name (if needed)
extern const String SECRET_APN_USER = ""; 		// specify the username for your APN (if needed)
extern const String SECRET_APN_PASS = "";		// specify the password for your APN (if needed)
extern const String SECRET_SERVER_MSISDN = "";  // specify the number of server(gate)

// WIFI connection credentials, used only for WiFi boards
extern const String SECRET_SSID = "";		// unless PIN is deactivated, specify your SIM card PIN number
extern const String SECRET_WIFI_PASS = "";					// specify the APN name (if needed)

	// Live Objects credential: paste below your API key (see Configuration > API keys on the portal).
	// You API key must have at least the predefined 'MQTT Device' rights profile
	// (alternatively: 'Device Access' read + write rights if need to customise the rights).
	// Please note that you *must* use a TLS connection (MQTTS) if you grant more rights to the API key.
extern const String SECRET_LIVEOBJECTS_API_KEY = "";
