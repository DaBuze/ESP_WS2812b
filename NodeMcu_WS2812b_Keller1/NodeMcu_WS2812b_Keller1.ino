#include <Adafruit_NeoPixel.h>
#include <FHEMHelper.h>
FHEMHelper fhem("MCU_KINO_LED_LEFT"); 

#define PIN            D1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      121


Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel *p_strip = &strip;

int delayval = 250; // delay for half a second
int helligkeit = 200;

WiFiClient m_espClient;
PubSubClient m_MQTTclient(m_espClient);
const char* m_mqtt_server = "";  

const char* inTopicKL_EFFECT  = "KELLER/LEFT/LED/EFFECT";
const char* inTopicKL_COLOR   = "KELLER/LEFT/LED/COLOR";


int showType = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  fhem.setup();


  strip.begin();
  strip.show();

  m_MQTTclient.setServer(m_mqtt_server, 1883);
  m_MQTTclient.setCallback(callback);
}

void loop() 
{
  boolean oReconnected = fhem.checkConnection(m_MQTTclient);
  if (true == oReconnected)
  {
    Serial.println("Rejoining Channels. ");
    // rejoin channels
    m_MQTTclient.subscribe(inTopicKL_EFFECT);
    m_MQTTclient.subscribe(inTopicKL_COLOR);

    //send message to FHEM to trigger callback with.
    m_MQTTclient.publish("ESPs","getRGB");
    Serial.println("Rejoining Callback. ");

  }
  m_MQTTclient.loop();

//  startShow(showType);
}



void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");


  String sMessage = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    sMessage += (char)payload[i];
  }
  Serial.println();

  if (strcmp (topic, inTopicKL_EFFECT) == 0 )
  {
      showType++;
      if (showType > 9)
      {
        showType=0;
      }
      Serial.print("switch show to: ");
      Serial.println(showType);
      //startShow(showType);
  }
  else if (strcmp (topic, inTopicKL_COLOR) == 0 )
  {
    Serial.println("set fixed color....");
    fhem.setColorFade(p_strip, sMessage);
    Serial.println("Color was set....");
  }
}


void startShow(int i) {
  switch(i){
    case 0: colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
            break;
    case 1: colorWipe(strip.Color(255, 0, 0), 50);  // Red
            break;
    case 2: colorWipe(strip.Color(0, 255, 0), 50);  // Green
            break;
    case 3: colorWipe(strip.Color(0, 0, 255), 50);  // Blue
            break;
    case 4: theaterChase(strip.Color(127, 127, 127), 50); // White
            break;
    case 5: theaterChase(strip.Color(127,   0,   0), 50); // Red
            break;
    case 6: theaterChase(strip.Color(  0,   0, 127), 50); // Blue
            break;
    case 7: rainbow(20);
            break;
    case 8: rainbowCycle(20);
            break;
    case 9: theaterChaseRainbow(50);
            break;
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
//  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
//  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

