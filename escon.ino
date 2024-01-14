#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "Dzulfikar Spot";
const char* password = "binamarga126";
const char* mqtt_server = "broker.emqx.io";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  Serial.println();

  // Koneksi ke WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Menunggu WiFi terkoneksi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }


  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


// Callback function jika ada message yang masuk dari topic yang di subscribe

void callback(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<50> doc;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.println();
  
  // Memproses message berupa json
  DeserializationError error = deserializeJson(doc, msg);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }else{
    // Jika message sukses di proses maka, kita ubah kondisi relay sesuai message yang dikirim
    serializeJson(doc, Serial);
    bool isOn = doc["device"];
    digitalWrite(0, isOn);
  }
}


// Menghubungkan device ke broker dan subscribe ke topik
void reconnect() {
  // Menghubungkan ke broker hingga terhubung
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Membuat random client id agar tidak bentrok
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Percobaan koneksi
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");

      // subscribe ke topik
      client.subscribe("escon2024/123456");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // jika gagal, tunggu 5 detik lalu akan mencoba menghubungkan kembali
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  // Jadikan GPIO 0 sebagai OUTPUT
  pinMode(0, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  // mengunggu pesan 
  client.loop();
}
