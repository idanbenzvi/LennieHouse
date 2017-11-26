/*
  Basic PIR Sensor Alert System (Wifi connected ESP8266)
  -----------------------------
  * MQTT published messages
  * listening to incoming messages
  * once motion is detected a message is published

  The Door sensor only alerts the gateway (or message center) of an event using the MQTT protocol. The messages are relayed through the broker and the 
  gateway decides how to act on the message. The message sent expects a return message with "ack" signal based on "1". If no such message is received an attempt to 
  send the message will be done 3 times. 

  Notes:
  * Remember the sensor requires 5v voltage, if using NodeMCU - you might require powering it using an external power source.

  TODO:
  * See if this enough
  * change onto JSON msg format to ease reading by NodeRED etc.
  
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "idanadi";
const char* password = "toytoy123";
const char* mqtt_server = "https://192.168.1.184";
int inputPin = 2;
int pirState = LOW;


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



void pirSensorAlert(){
  int val = digitalRead(inputPin);
  if(val==HIGH){
    if(pirState==LOW){
      Serial.println("Motion Detected");
      //publish MQTT message
      snprintf (msg, 75, "Door PIR MOTION DETECTED", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
    delay(5000); // wait 5 seconds before continuing 
      pirState=HIGH;
    }else{
      if(pirState==HIGH){
          Serial.println("Motion ended");
          pirState=LOW;
        }
      }
    }
  }



void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  //if ((char)payload[0] == '1') {
  //  digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
  //  // but actually the LED is on; this is because
  //  // it is acive low on the ESP-01)
  //} else {
  //  digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  // }

}
void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("Door_PIR", "Door Sensor Online");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  pirSensorAlert();
  client.loop();


  //in case of an event (motion event) - publis a message
  }
