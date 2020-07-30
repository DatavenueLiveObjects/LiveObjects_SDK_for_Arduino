	// Cellular connection cerdentials, used only for GSM boards
const char SECRET_PINNUMBER[] = "";		// unless PIN is deactivated, specify your SIM card PIN number
const char SECRET_APN[] = "";					// specify the APN name (if needed)
const char SECRET_APN_USER[] = ""; 		// specify the username for your APN (if needed)
const char SECRET_APN_PASS[] = "";		// specify the password for your APN (if needed)

// WIFI connection credentials, used only for WiFi boards
const char SECRET_SSID[] = "";		// unless PIN is deactivated, specify your SIM card PIN number
const char SECRET_WIFI_PASS[] = "";					// specify the APN name (if needed)

	// Live Objects credential: paste below your API key (see Configuration > API keys on the portal).
	// You API key must have at least the predefined 'MQTT Device' rights profile
	// (alternatively: 'Device Access' read + write rights if need to customise the rights).
	// Please note that you *must* use a TLS connection (MQTTS) if you grant more rights to the API key.
const char SECRET_LIVEOBJECTS_API_KEY[] = "...";
