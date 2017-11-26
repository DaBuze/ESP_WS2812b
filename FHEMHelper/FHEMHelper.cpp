#include "Arduino.h"
#include "FHEMHelper.h"





FHEMHelper::FHEMHelper (String sESPName)
{
	m_sESPName = sESPName;
}


void FHEMHelper::setup() 
{

  delay(50);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(m_ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(m_ssid, m_password);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  delay(500);
  Serial.println("Setup MQTT Client");

}

boolean FHEMHelper::checkConnection(PubSubClient MQTTclient)
{
	boolean oReconnected = false;
  if (!MQTTclient.connected()) 
  {
	  oReconnected = true;
    reconnect(MQTTclient);
  }
//  m_MQTTclient.loop();	
  return(oReconnected);
}

void FHEMHelper::reconnect(PubSubClient MQTTclient) 
{
  // Loop until we're reconnected
  while (!MQTTclient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect

    delay(500);

    // Create a random client ID
    String clientId = m_sESPName;
    clientId += String(random(0xffff), HEX);
    Serial.println("use client ID");
    Serial.println(clientId);
    
    if (MQTTclient.connect(clientId.c_str())) {
      Serial.println("connected");
      // ... and resubscribe
 //     m_MQTTclient.subscribe("KELLER/RELAIS/R1");


      //send message to FHEM to trigger callback with.
//      MQTTclient.publish("ESPs","getRGB");      
    } else {
      Serial.print("failed, rc=");
      Serial.print(MQTTclient.state());
      Serial.println(" try again in 1,5 seconds");
      // Wait 1,5 seconds before retrying
      for(int i = 0; i<1500; i++){
        delay(1);
      }
    }
  }
}

void FHEMHelper::extractRGBFromHex(String sHEX_RGB)
{
	
    String sR = "0x" + sHEX_RGB.substring(0,2); 
    String sG = "0x" + sHEX_RGB.substring(2,4);
    String sB = "0x" + sHEX_RGB.substring(4,6);
    Serial.println();
    Serial.print("Rot: ");
    Serial.print(sR);
    Serial.print(" Grün: ");
    Serial.print(sG);
    Serial.print(" Blau: ");
    Serial.print(sB);    
    Serial.println();


    int number = (int) strtol( &sHEX_RGB[0], NULL, 16);
    
    // Split them up into r, g, b values
    m_WS2812_R = number >> 16;
    m_WS2812_G = number >> 8 & 0xFF;
    m_WS2812_B = number & 0xFF;
	
    Serial.println();
    Serial.print("Rot: ");
    Serial.print(m_WS2812_R);
    Serial.print(" Grün: ");
    Serial.print(m_WS2812_G);
    Serial.print(" Blau: ");
    Serial.print(m_WS2812_B);    
    Serial.println();	
    	
}
	

void FHEMHelper::setWS2812RGB(Adafruit_NeoPixel *strip, String sHEX_RGB)
{
	extractRGBFromHex(sHEX_RGB);

    setColor(strip);
	
	m_WS2812_R_prev = m_WS2812_R;
	m_WS2812_G_prev = m_WS2812_G;
	m_WS2812_B_prev = m_WS2812_B;
	
	Serial.println("setWS2812RGB is done");
}


void FHEMHelper::setColor(Adafruit_NeoPixel *strip)
{

  Serial.println();
  Serial.println("analogWrite...red:");
  Serial.println(m_WS2812_R);
  Serial.println("analogWrite...green:");
  Serial.println(m_WS2812_G);
  Serial.println("analogWrite...blue:");
  Serial.println(m_WS2812_B);  

  
  for(uint16_t i=0; i<strip->numPixels(); i++) 
  {
    strip->setPixelColor(i, strip->Color(m_WS2812_R, m_WS2812_G, m_WS2812_B));
  } 
  strip->show();
  
  Serial.println("setColor is done");
}



void FHEMHelper::setColorFade(Adafruit_NeoPixel *strip, String sHEX_RGB)
{
	extractRGBFromHex(sHEX_RGB);
	
	boolean oColorFadeComplete = false;

	do
	{
		// calculare diff to target color based on _prev.
		m_WS2812_R_prev = m_WS2812_R_prev + getColorDiff(m_WS2812_R, m_WS2812_R_prev);
		m_WS2812_G_prev = m_WS2812_G_prev + getColorDiff(m_WS2812_G, m_WS2812_G_prev);
		m_WS2812_B_prev = m_WS2812_B_prev + getColorDiff(m_WS2812_B, m_WS2812_B_prev);

		// check if target color is ready
		if (m_WS2812_R_prev == m_WS2812_R && m_WS2812_G_prev == m_WS2812_G && m_WS2812_B_prev == m_WS2812_B)
		{
			oColorFadeComplete = true;
		}

		for(uint16_t i=0; i<strip->numPixels(); i++) 
		{
			strip->setPixelColor(i, strip->Color(m_WS2812_R_prev, m_WS2812_G_prev, m_WS2812_B_prev));
		} 
		strip->show();

		delay(5);

	}while (false == oColorFadeComplete);

}


int FHEMHelper::getColorDiff(int targetColor, int currentColor)
{

  if (currentColor > targetColor)
  {
      // eins abziehen
      return (-1);
  }
  else if (currentColor < targetColor)
  {
    // eins aufadieren
    return (1);
  }
  else if (currentColor == targetColor)
  {
    // passt genau 0
    return (0);
  }
  else
  {
    // darf nicht vorkommen!
    Serial.println("ERROR: getColorDiff ");
    Serial.println("targetColor");
    Serial.println(targetColor);
    Serial.println("currentColor");
    Serial.println(currentColor);
  }

  
}



