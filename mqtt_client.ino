#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <HX711.h>


const char* ssid = "Chamiru";
const char* password = "kickass7";
const char* mqtt_server = "broker.hivemq.com";


HX711 scale;  // Init of library

#define DOUT  D5
#define CLK  D6
float calibration_factor = 106; // this calibration factor is adjusted according to my load cell

//char* name_ = "House_4";
char* address = "deeedse";
char* description_ = "mfnweknfeffkwen";
float lat = 7.465465; 
float lng = 80.765244;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
float weight;

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

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.print((payload[0]));
  Serial.println();

  int final = int(payload[0]);
  //analogWrite(D3, final);
  
  /*if ((char)payload[0] == '1') {
    digitalWrite(D3, LOW);   
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(D3, HIGH);  
  }
 */
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "AvatarRoshan";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("smartbin_test", "hello world");
      // ... and resubscribe
      client.subscribe("smartbin_sub");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(10000);
    }
  }
}

float readADC(){  
  //read weight balue
  //Serial.println("begin");
  scale.begin(DOUT,CLK);
  scale.set_scale(calibration_factor);
  scale.set_offset(-35320);
  weight = scale.get_units(10), 10;
//  Serial.println("end");
  if(weight > 4800){
    return 0;
  }
  else{
    return 1;
  }
}


void setup() {
  //pinMode(D3, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  //pinMode(D4, INPUT); 
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  //int value=analogRead(A0);
  int value = readADC();     //just to test the whether it is reading the ldr output
  snprintf (msg, 150, "{\"name\":\"house_4\",\"lat\":%.6f,\"lng\":%.6f,\"weigh\":%d}",lat,lng,value); // \"address\":%c,
  //snprintf (msg, 200, "{\"name\":\"gh\",\"address\":\"khj\",\"description_\":\"lkj\",\"weigh\": %d}", value); // \"lat\": %.6f, \"lng\": %.6f, 
  
  //snprintf (msg, 200, "name: House3, address: sdfas, description_: sdf, weigh: %d", value);
  client.publish("smartbin", msg);
  Serial.print("Publish message: ");
  Serial.println(msg);
  

  delay(15000);
}
