#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 32    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
const int NUM_SLIDERS = 4;
const int analogInputs[NUM_SLIDERS] = {A0, A1, A2, A3};

unsigned long previousMillis = 0; // will store last time poties were updated
const long updateInterval = 10;   // interval at which to update (milliseconds)

unsigned long lastScreenChange = 0;
const int screenTimeout = 3000; /// milliseconds
String prevMessage = "";

int analogSliderValues[NUM_SLIDERS];
void setup()
{
  for (int i = 0; i < NUM_SLIDERS; i++)
  {
    pinMode(analogInputs[i], INPUT);
  }
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.setRotation(2);
  showOnDisplay("VOLUME MIXER 3000", 2);
  delay(2000);
  display.clearDisplay();
  display.display();
  Serial.begin(9600);
}

void loop()
{
  String appVolumes = readSerial();
  int comaIndex = appVolumes.indexOf(',');
  showSliderValues(appVolumes.substring(0, comaIndex), appVolumes.substring(comaIndex + 1));

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= updateInterval)
  {
    // save the last update
    previousMillis = currentMillis;
    updateSliderValues();
    sendSliderValues(); // Actually send data (all the time)
  }

  if (currentMillis - lastScreenChange >= screenTimeout)
  {
    display.clearDisplay();
    display.display();
  }
  // printSliderValues(); // For debug
}

String readSerial()
{
  static String line; // TODO how long an app name is ?
  while (Serial.available() > 0)
  {
    char c = Serial.read();
    if (c == '|')
    {
      line = Serial.readStringUntil('|');
    }
  }
  return line;
}
void updateSliderValues()
{
  for (int i = 0; i < NUM_SLIDERS; i++)
  {
    analogSliderValues[i] = analogRead(analogInputs[i]);
  }
}
void sendSliderValues()
{
  String builtString = String("");

  for (int i = 0; i < NUM_SLIDERS; i++)
  {
    builtString += String((int)analogSliderValues[i]);

    if (i < NUM_SLIDERS - 1)
    {
      builtString += String("|");
    }
  }

  Serial.println(builtString);
}
void showSliderValues(String appName, String volumeP)
{
  showOnDisplay(appName + ":\n" + volumeP, 2);
}
void showOnDisplay(String message, int fontSize)
{
  if (prevMessage == message){
    return;
  }
  prevMessage = message;
  display.clearDisplay();
  display.setTextSize(fontSize); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(message);
  display.display();
  lastScreenChange = millis();
}
void printSliderValues()
{
  for (int i = 0; i < NUM_SLIDERS; i++)
  {
    String printedString = String("Slider #") + String(i + 1) + String(": ") + String(analogSliderValues[i]) + String(" mV");
    Serial.write(printedString.c_str());

    if (i < NUM_SLIDERS - 1)
    {
      Serial.write(" | ");
    }
    else
    {
      Serial.write("\n");
    }
  }
}
