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
// a dále počtu znaků a řádků LCD, zde 20x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

// definice proměnné snímače teploty a vhkosti
HTU21D mujHTU;
//definice globalni struktury teploty a vlhkosti
struct TeplotaVlhkost{
    float teplota;
    float vlhkost;
 };
TeplotaVlhkost aktTeplotaVlhkost;

WiFiClient client;
HADevice device;
HAMqtt mqtt(client, device);
unsigned long lastUpdateAt = 0;

// "myUptime" is unique ID of the sensor. You should define your own ID.
HASensorNumber teplotaSensor("Teplota", HASensorNumber::PrecisionP1);
HASensorNumber vlhkostSensor("Vlhkost", HASensorNumber::PrecisionP1);

void setup() {
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
  //lcd.backlight();
  // vytisknutí hlášky na první řádek
  lcd.print("->dratek.cz<-");
  // nastavení kurzoru na první znak, druhý řádek
  // veškeré číslování je od nuly, poslední znak je tedy 19, 3
  lcd.setCursor ( 0, 1 );
  lcd.print("--------------------");
  lcd.setCursor ( 0, 2);
  lcd.print(" Test LCD pres I2C");
  lcd.setCursor ( 19, 3);
  lcd.print("!");
  delay(2000);
}

void loop() {
    mqtt.loop();  


 if ((millis() - lastUpdateAt) > 5000) { // 1000ms debounce time
       aktTeplotaVlhkost = GetTeplotaVlhkost();
       teplotaSensor.setValue(aktTeplotaVlhkost.teplota);
       vlhkostSensor.setValue(aktTeplotaVlhkost.vlhkost);

       Serial.print("Teplota:");
       Serial.print(aktTeplotaVlhkost.teplota, 1);
       Serial.print("°C");
       Serial.print("   ");
       Serial.print("Vlhkost:");
       Serial.print(aktTeplotaVlhkost.vlhkost, 1);
       Serial.print("%");
       Serial.println();
       lastUpdateAt = millis();
    }

     // nastavení kurzoru na devátý znak, druhý řádek
  lcd.setCursor(8, 1);
  // vytisknutí počtu sekund od začátku programu
  lcd.print(millis() / 1000);
    
}

/********************Funkce pro načtení teploty, vlhkosti, data a času**********************************/
  TeplotaVlhkost GetTeplotaVlhkost(){
  float teplota = mujHTU.readTemperature();
  float vlhkost = mujHTU.readHumidity();
  return {teplota, vlhkost};
  }
/*******************************************************************************************************/
