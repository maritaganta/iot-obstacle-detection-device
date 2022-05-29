#include <ESP8266WiFi.h>

const char* ssid     = "your-ssid";   //Chnage it with your ssid
const char* password = "your-password";   ////Chnage it with your passsword


const char* host = "api.thingspeak.com";
/*
 * Test Accouunt Credentials
String channelID = "1671094";     //Your Channel ID
String writeAPI = "M52QET61I8N6R9WH";   //Your Channel Write API
*/
String channelID = "1675318";     //Your Channel ID
String writeAPI = "149H3BK7E1GACNJD";   //Your Channel Write API

const int trigPin = 12;   //D6
const int echoPin = 14;   //D5

//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;


bool LAST_SENT_STATUS = 0;
bool OBSTACLE_DETECTION_STATUS = 0;

void setup() {

  Serial.begin(115200); // Starts the serial communication
  pinMode(trigPin, OUTPUT); // Setting the trigPin as an Output
  pinMode(echoPin, INPUT); // Setting the echoPin as an Input

  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
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

void loop() {

  obstacle_detection ();

  if (OBSTACLE_DETECTION_STATUS == 1)
  {
    post_data();
    delay(10000);
  }
}

void obstacle_detection ()
{
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance
  distanceCm = duration * SOUND_VELOCITY / 2;

  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;

  // Prints the distance on the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.print("Distance (inch): ");
  Serial.println(distanceInch);

  delay(1000);
  if (distanceCm > 0 && distanceCm <= 10)
  {
    Serial.println("Obstacle detected");
    OBSTACLE_DETECTION_STATUS = 1;
  }
  else if (distanceCm == 0 || distanceCm >= 11)
  {
    Serial.println("No Obstacle");
    if (LAST_SENT_STATUS == 1)
    {
      Serial.println("Obstacle CLEARED");
      delay(5000);
      post_data();
    }
    OBSTACLE_DETECTION_STATUS = 0;
  }
}

void post_data ()
{
  Serial.println("Posting data on ThingSpeak Server on this Channel ID: " + channelID);

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
  String url = "/update?api_key=" + writeAPI;
  String content = "field1=" + String(OBSTACLE_DETECTION_STATUS) + "&field2=" + String(distanceCm);      //USING POST METHOD
  Serial.print("Requesting URL: ");
  Serial.println(url);
  Serial.print("with content: ");
  Serial.println(content);

  // This will send the request to the server
  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + "content-length: " + content.length() + "\r\n" + "content-type: application/x-www-form-urlencoded" + "\r\n\r\n" + content
               //"Connection: close\r\n\r\n"
              );
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.println(line);
  }

  LAST_SENT_STATUS = OBSTACLE_DETECTION_STATUS;
  Serial.println("LAST_SENT_STATUS: " + String(LAST_SENT_STATUS));
  OBSTACLE_DETECTION_STATUS = 0;
  Serial.println();
  Serial.println("closing connection");
}
