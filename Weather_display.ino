#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal.h>

const char WiFiSSID[] = "2WIRE3442";
const char WiFiPSK[] = "XXXX";

const int LED_PIN = 2;
const int BATT_PIN = A0;

const char serverHost[] = "192.168.1.111";

WiFiClient wifi_client;
PubSubClient client(serverHost, 1883, wifi_client);
LiquidCrystal lcd(D1,D2,D3,D4,D5,D6);

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.println("Message Received");
  //Serial.print(topic);
  //Serial.print(payload);
  lcd.clear();
  for (int i=0; i < 33; i++)
  {
    //Write payload to lcd, but allow for multiline display
    if (i == 16)
    {
      lcd.setCursor(0,1);
    }
    lcd.write(payload[i]);
  }

  Serial.println("Entering sleep");
  client.disconnect();
  delay(100);
  ESP.deepSleep(30 * 60 * 1000000, WAKE_RF_DEFAULT);
  delay(100);
  
}

void initHardware()
{
  Serial.begin(9600);
  lcd.begin(16,2);
  client.setCallback(callback);

  pinMode(BATT_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

void connectWiFi()
{
  byte ledStatus = LOW;

  // Set WiFi mode to station (as opposed to AP or AP_STA)
  WiFi.mode(WIFI_STA);

  // WiFI.begin([ssid], [passkey]) initiates a WiFI connection
  // to the stated [ssid], using the [passkey] as a WPA, WPA2,
  // or WEP passphrase.
  WiFi.begin(WiFiSSID, WiFiPSK);

  int wifi_counter = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_counter < 12)
  {
    // Blink the LED
    digitalWrite(LED_PIN, ledStatus); // Write LED high/low
    ledStatus = (ledStatus == HIGH) ? LOW : HIGH;

    delay(5000); //wait 5 seconds
    
    Serial.println("Not connected");
    lcd.clear();
    lcd.print("WiFi Not");
    lcd.setCursor(0,1);
    lcd.print("Connected");
    wifi_counter += 1;
  }
  //Serial.println("Wifi connected successfully");
  lcd.clear();
  lcd.print("WiFi Connected");
  digitalWrite(LED_PIN, HIGH);
}

int connectToServer()
{
  // LED turns on when we enter, it'll go off when we 
  // successfully post.
  digitalWrite(LED_PIN, LOW);
  int counter = 0;
  while (counter < 3) 
  {
    if (client.connect("ESP8266 Weather"))
    {
      client.subscribe("weather_display/weather");
      Serial.println("Connected to server");
      digitalWrite(LED_PIN, HIGH);
      Serial.println("Sending data");
      publishData();
      return 1;
    }
    counter += 1;
    delay(5000);
  } 
  // If we fail to connect, return 0.
  Serial.println("Failed to connect to server");
  lcd.clear();
  lcd.print("Server Not");
  lcd.setCursor(0,1);
  lcd.print("Connected");
  delay(100);
  return -1;
}

bool publishData()
{
  int battery_level = analogRead(A0);
  float battery_voltage = battery_level*(5.0/1023.0);
  char battery_string[10];
  sprintf(battery_string, "%d", battery_voltage);
  return client.publish("weather_display/voltage", (char*) battery_string);
}

void setup() 
{
  initHardware();
  Serial.println("Hardware init'ed");
  lcd.clear();
  lcd.print("Initialization");
  lcd.setCursor(0,1);
  lcd.print("Completed");
  connectWiFi();
  digitalWrite(LED_PIN, HIGH);
}

void loop() 
{
  if (!client.connected())
  {
    connectToServer();
  }
  client.loop(); 
}

