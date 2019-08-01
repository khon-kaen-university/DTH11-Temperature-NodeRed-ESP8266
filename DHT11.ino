#include <ESP8266WiFi.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <DHT.h>

#define pin D2 // temp outside
#define DHTPIN D1 // temp inside and hump inside
#define DHTTYPE DHT11
OneWire oneWire(pin);

DallasTemperature sensors(&oneWire);
float Celcius = 0;
float Fahrenheit = 0;

const char* ssid = "name_wifi";
const char* pass = "password";
String sensor_id = "xx";
const char *host = "xxxxxx";
const int httpsPort =  "xxxx";
String Link = "/temp0";
const char fingerprint[] PROGMEM = "60 7F F1 02 28 6C 22 61 6A F0 8B 50 66 43 06 C3 D5 B3 67 BC";

DHT dht(DHTPIN, DHTTYPE);
float temp,hump;
String payload;


void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP());

  sensors.begin();
  dht.begin();
}

void loop()
{
  temp = dht.readTemperature(); // temp inside
  hump = dht.readHumidity(); // hum inside
  delay(100);

  try {
    if (temp < -20 || temp > 100) throw(String("Temp Inside Error."));
    if (hump < 0 || hump > 100) throw(String("Hum Inside Error."));
    
    // Temp in
    String sting_tempin = "";
    sting_tempin+=temp;
    Serial.print(" °C  TempIn : ");
    Serial.print(sting_temp);

    //Hum in
    String string_hump="";
    string_hump+=hump;
    Serial.print(" °C , HumIn : ");
    Serial.print(string_hump);
    Serial.print(" %");
    
    payload = "[{\"id\":" + sensor_id + ",\"temp\":" + sting_temp + ",\"hum\":" + string_hump + "}]";

      // show temp at node red
      String Link = "/temp";
      connet_to_node_red(Link);
      
      // show hum at node red
      String Link = "/hum";
      connet_to_node_red(Link);
 

  }
  catch (String err_message) {
    Serial.println("[ERROR] " + err_message);
    ESP.reset();
  }

delay(1000);
}
void connet_to_node_red(String Link){
       WiFiClientSecure httpsClient;
       httpsClient.setFingerprint(fingerprint);
       httpsClient.setTimeout(5000);
       httpsClient.connect(host, httpsPort);
       
       if ((httpsClient.print(String("POST ")
                             + Link + " HTTP/1.1\r\n" +
                             "Host: " + host + "\r\n" +
                             "Content-Type: application/json" + "\r\n" +
                             "Content-Length: 36" + "\r\n\r\n" +
                             payload + "\r\n" +
                             "Connection: close\r\n\r\n")))
      {
        Serial.println("POST-OK!!!");
      }
      else
      {
        Serial.println("POST-Failed!!!");
      }
    }
