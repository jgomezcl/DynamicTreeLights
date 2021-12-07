
/* DynamicTreeLights.ino

    04/12/2021
    Tested on an Attiny84
    The internal clock must be set to 8 MHz
*/

#include <WS2812.h>

/* Sensitivity configuration */
#define BUFSIZE 50
#define UPDATE_PERIOD 20
#define SENS_TRESHOLD 15

/* White color */
#define COLOR_R 255
#define COLOR_G 180
#define COLOR_B 70

/* Pins configuration */
#define LED_NUM 6
#define LED_PIN A3

#define LSENSOR_1 A1
#define LSENSOR_2 A0
#define LSENSOR_3 A2

#define SPONTANEOUS_TRIGGER       // Comment this line to avoid spontaneous blinks
#define PROBABILITY 20            // Probability of a spontaneous blink will be 1 divided by this number
#define SPONTANEOUS_PERIOD 10000  // Spontaneus probability will be checked with this periodicity (in ms)

//#define DEBUG   // To adjust the algorithm on a UNO

typedef struct circularBuffer
{
  int data[BUFSIZE];
  int pos;
  int validData;
} circularBuffer;

circularBuffer buf1, buf2, buf3;
unsigned long lastUpdate = 0;

WS2812 LEDS(LED_NUM);

#ifdef SPONTANEOUS_TRIGGER
uint16_t spontaneousCnt = 0;
#endif

void setup()
{

#ifdef DEBUG
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
#endif

  LEDS.setOutput(LED_PIN);

  setAllLeds(0, 0, 0);
  syncLeds();

  delay(400);

  delayedBlink();
}

void loop()
{
  if (millis() - lastUpdate > UPDATE_PERIOD)
  {
    lastUpdate += UPDATE_PERIOD;

    int newValue1 = analogRead(LSENSOR_1);
    int newValue2 = analogRead(LSENSOR_2);
    int newValue3 = analogRead(LSENSOR_3);

    addToBuffer(newValue1, &buf1);
    addToBuffer(newValue2, &buf2);
    addToBuffer(newValue3, &buf3);

    if (isDataValid(&buf1))
    {

#ifdef DEBUG
      Serial.print(getMean(&buf1));
      Serial.print(",");
      Serial.println(newValue1);
#endif

      if (newValue1 < getMean(&buf1) - SENS_TRESHOLD)
      {
        delayedBlink();
      }
      else if (newValue2 < getMean(&buf2) - SENS_TRESHOLD)
      {
        delayedBlink();
      }
      else if (newValue3 < getMean(&buf3) - SENS_TRESHOLD)
      {
        delayedBlink();
      }
      else
      {
#ifdef SPONTANEOUS_TRIGGER
        spontaneousCnt++;
        if (spontaneousCnt >= SPONTANEOUS_PERIOD / UPDATE_PERIOD)
        {
          spontaneousCnt = 0;
          if (random(0, PROBABILITY) <= 0)
          {
            delayedBlink();
          }
        }
#endif
      }
    }
  }
}

void addToBuffer(int newValue, circularBuffer *buf)
{
  buf->data[buf->pos] = newValue;

  buf->pos++;
  if (buf->pos >= BUFSIZE)
    buf->pos = 0;

  buf->validData++;
  if (buf->validData >= BUFSIZE)
    buf->validData = BUFSIZE;
}

bool isDataValid(circularBuffer *buf)
{
  if (buf->validData < BUFSIZE)
    return false;

  return true;
}

int getMean(circularBuffer *buf)
{
  unsigned long result = 0;
  for (int i = 0; i < BUFSIZE; i++)
  {
    result += buf->data[i];
  }

  result /= BUFSIZE;

  return (int)result;
}

void syncLeds()
{
  LEDS.sync();
  delay(1); // Without this delay consecutive calls to sync() are ignored
}

void delayedBlink()
{
  delay(200);

  int val = 255;

  for (int i = val; i >= 0; i = i / 2 - 1)
  {
    setAllLeds((i * COLOR_R) / 255, (i * COLOR_G) / 255, (i * COLOR_B) / 255);
    syncLeds();
    delay(30);
  }

  setAllLeds(0, 0, 0);
  syncLeds();

  randomPattern2();
  delay(500);

  buf1.validData = 0;
  buf2.validData = 0;
  buf3.validData = 0;
}

void setLed(int pos, int red, int green, int blue)
{
  cRGB color;
  color.r = red;
  color.g = green;
  color.b = blue;

  LEDS.set_crgb_at(pos, color);
}

void setAllLeds(int red, int green, int blue)
{
  for (int i = 0; i < LED_NUM; i++)
  {
    setLed(i, red, green, blue);
  }
}

void startPattern()
{
  int seq[LED_NUM];

  for (int i = 0; i < LED_NUM; i++)
  {
    seq[i] = random(0, LED_NUM);
  }

  for (int j = 0; j < LED_NUM; j++)
  {
    for (int i = 255; i >= 0; i = i / 2 - 1)
    {
      setLed(seq[j], (i * COLOR_R) / 255, (i * COLOR_G) / 255, (i * COLOR_B) / 255);
      syncLeds();
      delay(30);
    }
  }
}

void randomPattern1()
{
  int r, g, b;

  int seq[LED_NUM];

  for (int i = 0; i < LED_NUM; i++)
  {
    seq[i] = random(0, LED_NUM);
  }

  for (int j = 0; j < LED_NUM; j++)
  {
    r = random(0, 256);
    g = random(0, 256);
    b = random(0, 256);

    for (int i = 255; i >= 0; i = i / 2 - 1)
    {
      setLed(seq[j], (i * r) / 255, (i * g) / 255, (i * b) / 255);
      syncLeds();
      delay(30);
    }
  }
}

void randomPattern2()
{
  int r1, g1, b1;
  int r2, g2, b2;

  int seq[LED_NUM];

  seq[0] = random(0, LED_NUM);

  for (int i = 1; i < LED_NUM; i++)
  {
    do
    {
      seq[i] = random(0, LED_NUM);
    } while (seq[i] == seq[i - 1]);
  }

  int decaySpeed;

  for (int j = 0; j < LED_NUM / 2; j++)
  {
    r1 = random(0, 256);
    g1 = random(0, 256);
    b1 = random(0, 256);

    r2 = random(0, 256);
    g2 = random(0, 256);
    b2 = random(0, 256);

    decaySpeed = random(10, 40);

    for (int i = 255; i >= 0; i = i / 2 - 1)
    {
      setLed(seq[j * 2], (i * r1) / 255, (i * g1) / 255, (i * b1) / 255);
      setLed(seq[j * 2 + 1], (i * r2) / 255, (i * g2) / 255, (i * b2) / 255);
      syncLeds();
      delay(decaySpeed);
    }
  }
}
