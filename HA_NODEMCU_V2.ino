#include <ESP8266WiFi.h>
#include <ArduinoHA.h>
#include <Wire.h>
#include <SparkFunHTU21D.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>



#define BROKER_ADDR     IPAddress(192,168,0,151)
#define WIFI_SSID       "ZKwifi"
#define WIFI_PASSWORD   "MameRadiVodu85"
#define BROKER_USERNAME     "homeassistant" // replace with your credentials
#define BROKER_PASSWORD     "uiph2zo4guelae2thei7gai0ahleew2xohzie1aighoh5aiveiSoh9Agh0toopei"

// nastavení adresy I2C (0x27 v mém případě),
// a dále počtu znaků a řádků LCD, zde 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

// definice proměnné snímače teploty a vhkosti
HTU21D mujHTU;

float teplota;
float vlhkost;


WiFiClient client;
HADevice device;
HAMqtt mqtt(client, device);
unsigned long lastUpdateAt = 0;

// "myUptime" is unique ID of the sensor. You should define your own ID.
HASensorNumber teplotaSensor("Teplota", HASensorNumber::PrecisionP1);
HASensorNumber vlhkostSensor("Vlhkost", HASensorNumber::PrecisionP1);

//definice tlačítek
int tlacitko1Stav = 0;
int tlacitko2Stav = 0;
int tlacitko3Stav = 0;
int tlacitko4Stav = 0;

void setup() {
    //nastavení GPIO na vstupy
    pinMode(14, INPUT);
    pinMode(12, INPUT);
    pinMode(13, INPUT);
    pinMode(15, INPUT);
  
    //zahájení seriové komunikace
    Serial.begin(9600);
    Serial.println("Starting...");

    // Unique ID must be set!
    byte mac[]={0x01, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
    WiFi.macAddress(mac);
    device.setUniqueId(mac, sizeof(mac));

   
    // connect to wifi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500); // waiting for the connection
    }
    Serial.println();
    Serial.println("Connected to the network");

    // set device's details (optional)
    device.setName("TeplotaVlhkost");
    device.setSoftwareVersion("1.1.0");

  
    // configure sensor (optional) - teplota
    teplotaSensor.setIcon("mdi:home");
    teplotaSensor.setName("Teplota");
    teplotaSensor.setUnitOfMeasurement("°C");
    // configure sensor (optional) - vlhkost
    vlhkostSensor.setIcon("mdi:home");
    vlhkostSensor.setName("Vlhkost");
    vlhkostSensor.setUnitOfMeasurement("%");


    mqtt.begin(BROKER_ADDR, BROKER_USERNAME, BROKER_PASSWORD);

    // zahájení komunikace se snímačem HTU21D
    mujHTU.begin();


  // inicializace LCD
  lcd.begin();
  // zapnutí podsvícení
  lcd.noBacklight();
   
  // nastavení kurzoru na první znak, první řádek
  // veškeré číslování je od nuly, poslední znak je tedy 16, 1
  lcd.setCursor ( 0, 0 );
  lcd.print("--------------------");
  lcd.setCursor ( 0, 1);
  lcd.print(" Inicializace");
  delay(2000);
  lcd.clear();
}

void loop() {
  
  mqtt.loop();  
  // read the state of the pushbutton value:
  tlacitko1Stav = digitalRead(14);//tlačítko DOPRAVA
  tlacitko2Stav = digitalRead(12);//tlačítko DOLEVA
  tlacitko3Stav = digitalRead(13);//tlačítko NAHORU
  tlacitko4Stav = digitalRead(15);//tlačítko DOLU

  if ((millis() - lastUpdateAt) > 1000) { // 1000ms debounce time
    
    teplota = mujHTU.readTemperature();
    vlhkost = mujHTU.readHumidity();

    Serial.println(button1State);
    Serial.println(button2State);
    Serial.println(button3State);
    Serial.println(button4State);
    
    Serial.print("Teplota:");
    Serial.print(teplota, 1);
    Serial.print("°C");
    Serial.print("   ");
    Serial.print("Vlhkost:");
    Serial.print(vlhkost, 1);
    Serial.print("%");
    Serial.println();

    lcd.setCursor ( 0, 0 );
    lcd.print(teplota, 1);
    lcd.setCursor ( 5, 0 );
    lcd.print("stC");
    lcd.setCursor ( 0, 1 );
    lcd.print(vlhkost, 1);
    lcd.setCursor ( 5, 1 );
    lcd.print("%");
    lastUpdateAt = millis();
    }

    
}
