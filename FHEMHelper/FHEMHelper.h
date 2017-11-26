#ifndef __FHEMHelper__
#define __FHEMHelper__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>


class FHEMHelper {
	
private:
	String m_sESPName;
	const char* m_ssid = "";
	const char* m_password = "";

	double brightness = 50;
	double brightnessPrev = 1;
	int m_WS2812_R = 0;
	int m_WS2812_G = 0;
	int m_WS2812_B = 0;
	int m_WS2812_R_prev = 0;
	int m_WS2812_G_prev = 0;
	int m_WS2812_B_prev = 0;	
	
	void setColor(Adafruit_NeoPixel *strip);
	int getColorDiff(int targetColor, int currentColor);
	void extractRGBFromHex(String sHEX_RGB);
	
public:



	FHEMHelper (String sESPName);

	void setup() ;
	boolean checkConnection(PubSubClient MQTTclient);
	void reconnect(PubSubClient MQTTclient) ;

	void setWS2812RGB(Adafruit_NeoPixel *strip, String sHEX_RGB);
	void setColorFade(Adafruit_NeoPixel *strip, String sHEX_RGB);

	//PubSubClient getMQTTClient() ;
	//WiFiClient getESPClient() ;

};

#endif