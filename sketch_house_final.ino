#include <ESP32Servo.h>  
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "matheus"; 
const char* password = "matheus159"; 

AsyncWebServer server(80);

Servo servoBase1;
const int trigPin1 = 19;  
const int echoPin1 = 18;  
const int servoPin1 = 5;  
const int ledPin1 = 4;    

Servo servoBase2;
const int trigPin2 = 22;  
const int echoPin2 = 23;  
const int servoPin2 = 33; 
const int ledPin2 = 2;    

const int ledPin4 = 13;
const int ledPin3 = 12;
const int ledPin5 = 14;
const int ledPin6 = 27;

unsigned long servo1StartTime = 0;
unsigned long servo2StartTime = 0;
bool servo1Active = false;
bool servo2Active = false;
bool commandServo1Active = false; 
bool commandServo2Active = false; 

void setup() {
    Serial.begin(9600);           
    delay(1000);

    servoBase1.attach(servoPin1);   
    servoBase1.write(0);            
    pinMode(trigPin1, OUTPUT);      
    pinMode(echoPin1, INPUT);       
    pinMode(ledPin1, OUTPUT);       

    servoBase2.attach(servoPin2);   
    servoBase2.write(180);  
    pinMode(trigPin2, OUTPUT);      
    pinMode(echoPin2, INPUT);       
    pinMode(ledPin2, OUTPUT);       

    pinMode(ledPin3, OUTPUT);       
    pinMode(ledPin4, OUTPUT);       
    pinMode(ledPin5, OUTPUT);       
    pinMode(ledPin6, OUTPUT);       

    Serial.println("Conectando à rede Wi-Fi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");  
    }

    Serial.println();
    Serial.println("Wi-Fi conectado com sucesso!");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());  

    server.on("/servo1", HTTP_GET, [](AsyncWebServerRequest *request) {
        commandServo1Active = true; 
        servoBase1.write(120);  
        digitalWrite(ledPin1, HIGH);  
        digitalWrite(ledPin3, HIGH);  
        request->send(200, "text/plain", "Servo 1 acionado e permanecerá em 180 graus.");
    });

    server.on("/servo2", HTTP_GET, [](AsyncWebServerRequest *request) {
        commandServo2Active = true; 
        servoBase2.write(60);  
        digitalWrite(ledPin2, HIGH);  
        digitalWrite(ledPin4, HIGH);  
        request->send(200, "text/plain", "Servo 2 acionado e permanecerá em 0 graus.");
    });

    server.on("/servo1off", HTTP_GET, [](AsyncWebServerRequest *request) {
        servoBase1.write(0);  
        digitalWrite(ledPin1, LOW);  
        request->send(200, "text/plain", "Servo 1 retornou ao estado inicial.");
        commandServo1Active = false; 
    });

    server.on("/servo2off", HTTP_GET, [](AsyncWebServerRequest *request) {
        servoBase2.write(180);  
        digitalWrite(ledPin2, LOW);  
        request->send(200, "text/plain", "Servo 2 retornou ao estado inicial.");
        commandServo2Active = false; 
    });

    server.on("/garageon", HTTP_GET, [](AsyncWebServerRequest *request) {
        digitalWrite(ledPin3, HIGH);
        digitalWrite(ledPin4, HIGH);
        request->send(200, "text/plain", "LED Garagem On");
    });

    server.on("/garageoff", HTTP_GET, [](AsyncWebServerRequest *request) {
        digitalWrite(ledPin3, LOW);
        digitalWrite(ledPin4, LOW);
        request->send(200, "text/plain", "LED Garagem Off");
    });

    server.on("/topon", HTTP_GET, [](AsyncWebServerRequest *request) {
        digitalWrite(ledPin5, HIGH);
        digitalWrite(ledPin6, HIGH);
        request->send(200, "text/plain", "LED Superior On");
    });

    server.on("/topoff", HTTP_GET, [](AsyncWebServerRequest *request) {
        digitalWrite(ledPin5, LOW);
        digitalWrite(ledPin6, LOW);
        request->send(200, "text/plain", "LED Superior Off");
    });

    server.begin();
}

void loop() {
    handleUltrasonicSensors();  
    delay(100);
    handleSerialCommands();
    delay(100);  
}

void handleSerialCommands() {
    if (Serial.available()) {
        String comando = Serial.readStringUntil('\n'); 

        if (comando == "garageon") {
            digitalWrite(ledPin3, HIGH);
            digitalWrite(ledPin4, HIGH);
            Serial.println("LED Garagem On");

        } else if (comando == "garageoff") {
            digitalWrite(ledPin3, LOW);
            digitalWrite(ledPin4, LOW);
            Serial.println("LED Garagem Off");

        } else if (comando == "topon") {
            digitalWrite(ledPin5, HIGH);
            digitalWrite(ledPin6, HIGH);
            Serial.println("LED Superior On");

        } else if (comando == "topoff") {
            digitalWrite(ledPin5, LOW);
            digitalWrite(ledPin6, LOW);
            Serial.println("LED Superior Off");

        } else if (comando == "servo1") {
            commandServo1Active = true; 
            servoBase1.write(120);  
            digitalWrite(ledPin1, HIGH);  
            digitalWrite(ledPin3, HIGH);  
            Serial.println("Servo 1 acionado e permanecerá em 180 graus.");

        } else if (comando == "servo1off") {
            commandServo1Active = false;  
            servoBase1.write(0);  
            digitalWrite(ledPin1, LOW);  
            Serial.println("Servo 1 retornou ao estado inicial.");

        } else if (comando == "servo2") {
            commandServo2Active = true; 
            servoBase2.write(60);  
            digitalWrite(ledPin2, HIGH);  
            digitalWrite(ledPin4, HIGH);  
            Serial.println("Servo 2 acionado e permanecerá em 0 graus.");

        } else if (comando == "servo2off") {
            commandServo2Active = false;  
            servoBase2.write(180);  
            digitalWrite(ledPin2, LOW);  
            Serial.println("Servo 2 retornou ao estado inicial.");
        }
    }
}

void handleUltrasonicSensors() {
    if (!commandServo1Active) {
        long duration1, distance1;

        digitalWrite(trigPin1, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin1, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin1, LOW);

        // Leitura do eco com um timeout de 30 ms para evitar travamento
        duration1 = pulseIn(echoPin1, HIGH, 30000);

        if (duration1 == 0) {
            distance1 = -1;  
        } else {
            distance1 = (duration1 * 0.034) / 2;
        }

        if (distance1 > 0 && distance1 <= 2 && !servo1Active) {
            servoBase1.write(120);  
            digitalWrite(ledPin1, HIGH);  
            digitalWrite(ledPin3, HIGH);  
            servo1StartTime = millis();  
            servo1Active = true;
            Serial.println("Sensor servo 1 ativou");
        } else if (servo1Active && (millis() - servo1StartTime >= 5000)) {
            servo1Active = false;  
            servoBase1.write(0);  
            digitalWrite(ledPin1, LOW);
            digitalWrite(ledPin3, LOW);
            Serial.println("Sensor servo 1 desativou");
        }
    }
    
    if (!commandServo2Active) {
        long duration2, distance2;

        digitalWrite(trigPin2, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin2, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin2, LOW);

        duration2 = pulseIn(echoPin2, HIGH, 30000);

        if (duration2 == 0) {
            distance2 = -1; 
        } else {
            distance2 = (duration2 * 0.034) / 2;
        }

        if (distance2 > 0 && distance2 <= 2 && !servo2Active) {
            servoBase2.write(60); 
            digitalWrite(ledPin2, HIGH);  
            digitalWrite(ledPin4, HIGH);  
            servo2StartTime = millis();  
            servo2Active = true;
            Serial.println("Sensor servo 2 ativou");
        } else if (servo2Active && (millis() - servo2StartTime >= 5000)) {
            servo2Active = false;  
            servoBase2.write(180);  
            digitalWrite(ledPin2, LOW);
            digitalWrite(ledPin4, LOW);
            Serial.println("Sensor servo 2 desativou");
        }
    }
}



