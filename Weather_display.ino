#include <ESP8266WiFi.h>
#include <LiquidCrystal.h>

const char WiFiSSID[] = "2WIRE3442";
const char WiFiPSK[] = "gentlepotato743";

const int LED_PIN = 2;

const char serverHost[] = "192.168.1.111";
const unsigned long readRate = 5000; //seconds 600 sec
unsigned long lastRead = 0;

char readInLine1[32] = "";
char readInLine2[32] = "";

int mcMode = 0;

LiquidCrystal lcd(D6,D1,D2,D3,D4,D5);

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
  while(!readFromServer())
  {
    Serial.println("Attempting Server Read");
    delay(10*1000);
  }
  /*if (lastRead + readRate <= millis())
  {
    if (readFromServer())
      lastRead = millis();
  }*/
  Serial.println("Entering sleep");
  delay(100);
  ESP.deepSleep(15 * 60 * 1000000, WAKE_RF_DEFAULT);
  delay(100);
}

void connectWiFi()
{
  byte ledStatus = LOW;
  mcMode = 1;

  // Set WiFi mode to station (as opposed to AP or AP_STA)
  WiFi.mode(WIFI_STA);

  // WiFI.begin([ssid], [passkey]) initiates a WiFI connection
  // to the stated [ssid], using the [passkey] as a WPA, WPA2,
  // or WEP passphrase.
  WiFi.begin(WiFiSSID, WiFiPSK);

  // Use the WiFi.status() function to check if the ESP8266
  // is connected to a WiFi network.
  while (WiFi.status() != WL_CONNECTED)
  {
    // Blink the LED
    digitalWrite(LED_PIN, ledStatus); // Write LED high/low
    ledStatus = (ledStatus == HIGH) ? LOW : HIGH;

    // Delays allow the ESP8266 to perform critical tasks
    // defined outside of the sketch. These tasks include
    // setting up, and maintaining, a WiFi connection.
    delay(5000); //wait 5 seconds
    // Potentially infinite loops are generally dangerous.
    // Add delays -- allowing the processor to perform other
    // tasks -- wherever possible.
    Serial.println("Not connected");
    lcd.clear();
    lcd.print("WiFi Not");
    lcd.setCursor(0,1);
    lcd.print("Connected");
  }
  //Serial.println("Wifi connected successfully");
  lcd.clear();
  lcd.print("WiFi Connected");
  mcMode = 4;
}

void initHardware()
{
  Serial.begin(9600);
  lcd.begin(16,2);
  pinMode(LED_PIN, OUTPUT);
}

int readFromServer()
{
  // LED turns on when we enter, it'll go off when we 
  // successfully post.
  digitalWrite(LED_PIN, LOW);

  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(serverHost, httpPort)) 
  {
    // If we fail to connect, return 0.
    Serial.println("Failed to connect to server");
    lcd.clear();
    lcd.print("Server Not");
    lcd.setCursor(0,1);
    lcd.print("Connected1");
    mcMode = 3;
    delay(100);
    return 0;
  }
  Serial.println("Connected to server");
  // If we successfully connected, print our Phant post:
  //client.print(phant.post());
  client.println("GET /weather_data");

  Serial.println("Send request");

  delay(1000);

//  char readInLine1[32] = "";
//  char readInLine2[32] = "";
  int i = 0;
  while (client.available())
  {
    if (i < 16)
      readInLine1[i] = client.read();
    else
      readInLine2[i-16] = client.read();
    i += 1;
  }
//  String line1(readInLine1);
//  String line2(readInLine2);
  Serial.println(readInLine1);
  Serial.println(readInLine2);
  lcd.clear();
  lcd.print(readInLine1);
  lcd.setCursor(0,1);
  lcd.print(readInLine2);
  
  // Before we exit, turn the LED off.
  digitalWrite(LED_PIN, HIGH);
  mcMode = 3;

  return 1; // Return success
}

