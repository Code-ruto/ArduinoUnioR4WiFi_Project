#include <SPI.h>
#include <WiFiS3.h>
#include "secrets.h"

// Motor pins
int motor1pin1 = 2;
int motor1pin2 = 3;
int motor2pin1 = 4;
int motor2pin2 = 5;

IPAddress subnet(255, 255, 0, 0);
IPAddress gateway(10, 0, 0, 1);
IPAddress local_IP(10, 0, 0, 251);

WiFiServer server(80);  // Set up web server on port 80

void setup() {
  Serial.begin(115200);

  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
  IPAddress local_IP(10, 0, 0, 252);

  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait until the board connects to WiFi
  while (WiFi.status() != WL_CONNECTED) { //what is wl connected
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");

  server.begin();  // Start the server
  Serial.print("Server started at ");
  Serial.println(WiFi.localIP());
}

// Functions for motor movement
void moveForward() {
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
}

void moveBackward() {
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
}

void turnLeft() {
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
}

void turnRight() {
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
}

void stopMotors() {
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);
}

void loop() {
  WiFiClient client = server.available();  // Listen for incoming clients

  if (client) {
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    // Process command from URL
    if (request.indexOf("/FORWARD") != -1) {
      moveForward();
    } else if (request.indexOf("/BACKWARD") != -1) {
      moveBackward();
    } else if (request.indexOf("/LEFT") != -1) {
      turnLeft();
    } else if (request.indexOf("/RIGHT") != -1) {
      turnRight();
    } else if (request.indexOf("/STOP") != -1) {
      stopMotors();
    }

    // Send a response to the client
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<h1>Robot Control</h1>");
    client.println("<p><a href=\"/FORWARD\">Move Forward</a></p>");
    client.println("<p><a href=\"/BACKWARD\">Move Backward</a></p>");
    client.println("<p><a href=\"/LEFT\">Turn Left</a></p>");
    client.println("<p><a href=\"/RIGHT\">Turn Right</a></p>");
    client.println("<p><a href=\"/STOP\">Stop</a></p>");
    client.println("</html>");
    client.stop();
  }
}
