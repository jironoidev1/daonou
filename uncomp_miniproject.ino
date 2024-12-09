#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//=================================================================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

const int freq = 5000; // ความถี่ในการสร้างสัญญาณ PWM
const int PWM_A = 0;
const int PWM_B = 1;
const int resolution = 8; // ความละเอียด 0-16 bit

#define in1 2
#define in2 15
#define in3 4
#define in4 16
#define PWMPin1 22
#define PWMPin2 23

Adafruit_SSD1306 OLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String username = "";
String current_time = "";

//=================================================================================================
// WiFi และ MQTT Configuration
const char* ssid = "CE-NextGen";
const char* password = "##CEnextgen2024";
const char* mqtt_broker = "mqtt.cloud.ce.kmitl.ac.th";
const char* mqtt_username = "iot_new_02";
const char* mqtt_password = "iot_new_02";
const int mqtt_port = 1883;
const char* topic_username = "TOPIC_USERNAME";
const char* topic_move = "teslana/moving";
const char* topic_speed = "teslana/speed";

WiFiClient espClient;
PubSubClient client(espClient);

//=================================================================================================

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message: ");
  Serial.println(message);
  Serial.println("-----------------------");

  if (strcmp(topic, topic_username) == 0) {
    username = message;
  } else if (strcmp(topic, topic_move) == 0){
    if(message == "b"){
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      Serial.println("เดินหน้าจ่ะ");
    }

    if(message == "l"){
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);
      Serial.println("เลี้ยวซ้ายจ่ะ");
    }

    if(message == "r"){
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      Serial.println("เลี้ยวซ้ายจ่ะ");
    }

    if(message == "f"){
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      Serial.println("ถอยหลังจ่ะ");

    }
    else{
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);

    }
 } else if (strcmp(topic, topic_speed) == 0){
    ledcWrite(PWM_A, message.toInt());
    ledcWrite(PWM_B, message.toInt());
  }
}

void reconnect() {
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());

    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public MQTT broker connected");
      client.subscribe(topic_username);
      client.subscribe(topic_move);
    } else {
      Serial.print("Failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {

  ledcSetup(PWM_A, freq, resolution);
  ledcSetup(PWM_B, freq, resolution);
  ledcAttachPin(PWMPin1, PWM_A);
  ledcAttachPin(PWMPin2, PWM_B);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  ledcWrite(PWM_A, 255);
  ledcWrite(PWM_B, 255);

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  
  reconnect();
  
  if (!OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
  } else {
    Serial.println("OLED Start Work !!!");
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  unsigned long currentMillis = millis();

  OLED.clearDisplay();
  OLED.setTextColor(WHITE, BLACK);
  OLED.setCursor(0, 0);
  OLED.setTextSize(2);
  OLED.println("Welcome..");
  OLED.setCursor(0, 20);
  OLED.setTextSize(2);
  OLED.println(username);

  OLED.setCursor(0, 40);
  OLED.setTextSize(1);
  OLED.print("time : ");
  OLED.setTextColor(BLACK, WHITE); 
  OLED.println(current_time);
  client.loop();
}
