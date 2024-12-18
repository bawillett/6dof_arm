/*********
  Blake Willett
  This program creates a website with slider inputs for four stepper motors and two servos. 
  The website will communicate with an ESP32 connected via wifi (could alternatively use bluetooth) to control the motors stepper1d on input.
  
  Adapted from the following tutorials by Rui Santos & Sara Santos:
  Servo Control Tutorial: https://RandomNerdTutorials.com/esp32-servo-motor-web-server-arduino-ide/ 
  Stepper Control Tutorial: https://randomnerdtutorials.com/stepper-motor-esp32-web-server/#stepper-web-server-html-form 
*********/

#include <WiFi.h>
#include <ESP32Servo.h>
#include <Stepper.h>

const int stepsPerRev = 200; //NEMA23 steps per revolution

//Motor objects
Servo myservo1;
Servo myservo2;
Stepper stepper1(stepsPerRev, 33, 32);  //stepper1(steps, pul, dir)
Stepper stepper2(stepsPerRev, 13, 12);  //stepper2
Stepper stepper3(stepsPerRev, 14, 27);  //stepper3
Stepper stepper4(stepsPerRev, 26, 25);   //stepper4

int speed = 1000;  //stepper speed in rpm

//Create one pin int for each servo
static const int servoPin1 = 4;  //may need switch
static const int servoPin2 = 19;

//Replace with your network credentials
const char* ssid = "WiFi Name";
const char* password = "Password";

// Set web server port number to 80
WiFiServer server(80);

// Variable to detect whether a new request occurred
bool newRequest = false;

// Variable to store the HTTP request
String header;

// Decode HTTP GET value
String valueString = String(0);  //default = 0 for all sliders
int pos1 = 0;                    //stepper1
int pos2 = 0;                    //stepper2
int pos3 = 0;                    //stepper3
int pos4 = 0;                    //stepper4
int pos5 = 0;                    //left
int pos6 = 0;                    //right

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);

  //Attach each servo to its pin
  myservo1.attach(servoPin1);  // attaches the servo on the servoPin to the servo object
  myservo2.attach(servoPin2);  // attaches the servo on the servoPin to the servo object

  //Servo set speed
  stepper1.setSpeed(speed);
  stepper2.setSpeed(speed);
  stepper3.setSpeed(speed);
  stepper4.setSpeed(speed);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();  // Listen for incoming clients

  if (client) {  // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");                                             // print a message out in the serial port
    String currentLine = "";                                                   // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {  // if there's bytes to read from the client,
        char c = client.read();  // read a byte, then
        header += c;
        if (c == '\n') {  // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto;}");
            client.println(".slider { width: 720px; }</style>");
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.7.1/jquery.min.js\"></script>");  //jquery.ajax allows interacting w updating website
            client.println("</head><body><h1>ESP32 with Servo and Stepper</h1>");

            //HTML+JavaScript blocks for each motor
            //stepper1 (NOTE: Currently max/min of slider input is |1000| steps for all steppers)
            client.println("<p>Position: <span id=\"stepPos1\"></span></p>");
            client.println("<input type=\"range\" min=\"-1000\" max=\"1000\" class=\"slider\" id=\"step1\" onchange=\"stepper1(this.value)\" value=\"" + valueString + "\"/>");

            client.println("<script>var num1 = document.getElementById(\"step1\");");
            client.println("var pos1 = document.getElementById(\"stepPos1\"); step1.innerHTML = num1.value;");
            client.println("num1.oninput = function() { num1.value = this.value; pos1.innerHTML = this.value; }");
            client.println("$.ajaxSetup({timeout:1000}); function stepper1(pos) { ");
            client.println("$.get(\"/?step1=\" + pos + \"&\"); {Connection: close};}</script>");

            //stepper2
            client.println("<p>Position: <span id=\"stepPos2\"></span></p>");
            client.println("<input type=\"range\" min=\"-1000\" max=\"1000\" class=\"slider\" id=\"step2\" onchange=\"stepper2(this.value)\" value=\"" + valueString + "\"/>");

            client.println("<script>var num2 = document.getElementById(\"step2\");");
            client.println("var pos2 = document.getElementById(\"stepPos2\"); step2.innerHTML = num2.value;");
            client.println("num2.oninput = function() { num2.value = this.value; pos2.innerHTML = this.value; }");
            client.println("$.ajaxSetup({timeout:1000}); function stepper2(pos) { ");
            client.println("$.get(\"/?step2=\" + pos + \"&\"); {Connection: close};}</script>");

            //stepper3
            client.println("<p>Position: <span id=\"stepPos3\"></span></p>");
            client.println("<input type=\"range\" min=\"-1000\" max=\"1000\" class=\"slider\" id=\"step3\" onchange=\"stepper3(this.value)\" value=\"" + valueString + "\"/>");

            client.println("<script>var num3 = document.getElementById(\"step3\");");
            client.println("var pos3 = document.getElementById(\"stepPos3\"); step3.innerHTML = num3.value;");
            client.println("num3.oninput = function() { num3.value = this.value; pos3.innerHTML = this.value; }");
            client.println("$.ajaxSetup({timeout:1000}); function stepper3(pos) { ");
            client.println("$.get(\"/?step3=\" + pos + \"&\"); {Connection: close};}</script>");

            //stepper4
            client.println("<p>Position: <span id=\"stepPos4\"></span></p>");
            client.println("<input type=\"range\" min=\"-1000\" max=\"1000\" class=\"slider\" id=\"step4\" onchange=\"stepper4(this.value)\" value=\"" + valueString + "\"/>");

            client.println("<script>var num4 = document.getElementById(\"step4\");");
            client.println("var pos4 = document.getElementById(\"stepPos4\"); step4.innerHTML = num4.value;");
            client.println("num4.oninput = function() { num4.value = this.value; pos4.innerHTML = this.value; }");
            client.println("$.ajaxSetup({timeout:1000}); function stepper4(pos) { ");
            client.println("$.get(\"/?step4=\" + pos + \"&\"); {Connection: close};}</script>");


            //servo1
            client.println("<p>Position: <span id=\"servoPos1\"></span></p>");
            client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider1\" onchange=\"servo1(this.value)\" value=\"" + valueString + "\"/>");

            client.println("<script>var slider1 = document.getElementById(\"servoSlider1\");");
            client.println("var servoP1 = document.getElementById(\"servoPos1\"); servoP1.innerHTML = slider1.value;");
            client.println("slider1.oninput = function() { slider1.value = this.value; servoP1.innerHTML = this.value; }");
            client.println("$.ajaxSetup({timeout:1000}); function servo1(pos) { ");
            client.println("$.get(\"/?value1=\" + pos + \"&\"); {Connection: close};}</script>");

            //servo2
            client.println("<p>Position: <span id=\"servoPos2\"></span></p>");
            client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider2\" onchange=\"servo2(this.value)\" value=\"" + valueString + "\"/>");

            client.println("<script>var slider2 = document.getElementById(\"servoSlider2\");");
            client.println("var servoP2 = document.getElementById(\"servoPos2\"); servoP2.innerHTML = slider2.value;");
            client.println("slider2.oninput = function() { slider2.value = this.value; servoP2.innerHTML = this.value; }");
            client.println("$.ajaxSetup({timeout:1000}); function servo2(pos) { ");
            client.println("$.get(\"/?value2=\" + pos + \"&\"); {Connection: close};}</script>");


            client.println("</body></html>");

            //The string header contains all the data in the URL sent by the client after the IP address.

            //Create conditional blocks for each motor
            //stepper1
            if (header.indexOf("step1=") >= 0) {
              pos1 = header.indexOf("step1=");
              pos5 = header.indexOf("&", pos1);
              //The added int is the length f the variable + "=", so that the substring starts at the numerical value
              valueString = header.substring(pos1 + 6, pos5);

              // Spin the stepper
              stepper1.step(valueString.toInt());
              //delay(1000); //1s
            }

            //stepper2
            if (header.indexOf("step2=") >= 0) {
              pos2 = header.indexOf("step2=");
              pos3 = header.indexOf("&", pos2);
              valueString = header.substring(pos2 + 6, pos3);

              // Spin the stepper
              stepper2.step(valueString.toInt());
              //delay(1000); //1s
            }

            //stepper3
            if (header.indexOf("step3=") >= 0) {
              pos3 = header.indexOf("step3=");
              pos4 = header.indexOf("&", pos3);
              valueString = header.substring(pos3 + 6, pos4);

              // Spin the stepper
              stepper3.step(valueString.toInt());
              //delay(1000); //1s
            }

            //stepper4
            if (header.indexOf("step4=") >= 0) {
              pos4 = header.indexOf("step4=");
              pos5 = header.indexOf("&", pos4);
              valueString = header.substring(pos4 + 6, pos5);

              // Spin the stepper
              stepper4.step(valueString.toInt());
              //delay(1000); //1s
            }

            //servo1
            if (header.indexOf("value1=") >= 0) {
              pos5 = header.indexOf("value1=");
              pos6 = header.indexOf("&", pos5);
              valueString = header.substring(pos5 + 7, pos6);

              //stepper4ate the servo
              myservo1.write(valueString.toInt());
            }

            //servo2
            if (header.indexOf("value2=") >= 0) {

              pos5 = header.indexOf("value2=");
              pos6 = header.indexOf("&", pos5);
              valueString = header.substring(pos5 + 7, pos6);

              //stepper4ate the servo
              myservo2.write(valueString.toInt());
            }

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else {  // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}