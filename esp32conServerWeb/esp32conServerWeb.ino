#include <WiFi.h>
#include <WebSocketClient.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h> 

// ===== CONFIGURACIÓN WIFI =====
const char* nombreRed = "Redmi Note 12S";
const char* contraseñaRed = "Gsv12041010";

// ===== CONFIGURACIÓN WEBSOCKET =====
const char* servidor = "192.168.126.52";
const int puerto = 3000;
const char* ruta = "/";

// ===== PINES DE LEDS =====
const int pinLed1 = 13;

const int pinLed2 = 12;

// ===== CONFIGURACIÓN SERVOS =====
const int pinServo1 = 27;  // Servo 1: rango 90° a 180°
const int pinServo2 = 26;  // Servo 2: rango 90° a 0°
Servo servoMotor1;  
Servo servoMotor2;  

// Posiciones iniciales de los servos (ambos en 90°)
int posicionServo1 = 90;
int posicionServo2 = 180;

// ===== CONFIGURACIÓN MOTORES =====
const int ENA = 19;    // Enable Motor 1
const int IN1 = 18;    // Input 1
const int IN2 = 5;     // Input 2
const int IN3 = 4;     // Input 3
const int IN4 = 2;     // Input 4
const int ENB = 15;    // Enable Motor 2

// Velocidades para los motores (0-255)
const int velocidadMotores = 170;  // 100% de potencia

// Objetos para WiFi y WebSocket
WiFiClient clienteWiFi;
WebSocketClient socketWeb(clienteWiFi, servidor, puerto);

void setup() {
    Serial.begin(115200);
    
    // Configurar pines
    pinMode(pinLed1, OUTPUT);
    pinMode(pinLed2, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENB, OUTPUT);
    
    // Inicializar motores apagados
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    
    // Configurar timers para los servos
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    
    // Configurar y conectar servos permanentemente
    servoMotor1.setPeriodHertz(50);
    servoMotor2.setPeriodHertz(50);
    servoMotor1.attach(pinServo1, 500, 2500);
    servoMotor2.attach(pinServo2, 500, 2500);
    
    servoMotor1.write(110);
    servoMotor2.write(70);
    posicionServo1 = 90;  // Aseguramos que las variables también estén en 90
    posicionServo2 = 180;
    delay(1000);  // Damos más tiempo para que alcancen la posición


    // Conectar WiFi
    WiFi.begin(nombreRed, contraseñaRed);
    
    Serial.println("Conectando al WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("¡Conectado al WiFi!");
    
    if (socketWeb.begin(ruta)) {
        Serial.println("Conectado al WebSocket");
    } else {
        Serial.println("Error al conectar al WebSocket");
    }
}

// ===== CONTROL DE MOTORES =====
void controlMotores(String comando) {
    if (comando == "motor-adelante") {
        analogWrite(ENA, velocidadMotores);
        analogWrite(ENB, velocidadMotores);
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
    }
    else if (comando == "motor-atras") {
        analogWrite(ENA, velocidadMotores);
        analogWrite(ENB, velocidadMotores);
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
    }
    else if (comando == "motor-aderecha") {
        analogWrite(ENA, velocidadMotores);
        analogWrite(ENB, velocidadMotores);
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
    }
    else if (comando == "motor-aizquierda") {
        analogWrite(ENA, velocidadMotores);
        analogWrite(ENB, velocidadMotores);
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
    }
    else if (comando == "motor-dadelante" || comando == "motor-datras") {
        analogWrite(ENA, 0);
        analogWrite(ENB, 0);
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
    }
    else if (comando == "motor-dderecha") {
        analogWrite(ENA, 0);
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
    }
    else if (comando == "motor-dizquierda") {
        analogWrite(ENB, 0);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
    }
}

// ===== CONTROL DE LEDS =====
void controlLeds(String comando) {
    if (comando == "led-on") {
        digitalWrite(pinLed1, HIGH);
        digitalWrite(pinLed2, HIGH);
    } 
    else if (comando == "led-off") {
        digitalWrite(pinLed1, LOW);
        digitalWrite(pinLed2, LOW);
    }
}

// ===== CONTROL DE SERVOS =====
void controlServo(String comando) {
    if (comando == "subirServo") {
        Serial.println("=== Subiendo Servos ===");
        Serial.print("Posición anterior - Servo 27: ");
        Serial.print(posicionServo1);
        Serial.print("°, Servo 26: ");
        Serial.print(posicionServo2);
        Serial.println("°");
        
        // Incrementar posiciones en 20 grados
        if (posicionServo1 < 180) {
            posicionServo1 = min(180, posicionServo1 + 20);
        }
        if (posicionServo2 > 0) {
            posicionServo2 = max(0, posicionServo2 - 20);
        }
        
        // Mover a nueva posición
        servoMotor1.write(posicionServo1);
        servoMotor2.write(posicionServo2);
        
        Serial.print("Nueva posición   - Servo 27: ");
        Serial.print(posicionServo1);
        Serial.print("°, Servo 26: ");
        Serial.print(posicionServo2);
        Serial.println("°");
        Serial.println("==================");
    }
    else if (comando == "bajarServo") {
        Serial.println("=== Bajando Servos ===");
        Serial.print("Posición anterior - Servo 27: ");
        Serial.print(posicionServo1);
        Serial.print("°, Servo 26: ");
        Serial.print(posicionServo2);
        Serial.println("°");
        
        // Decrementar posiciones en 20 grados
        if (posicionServo1 > 90) {
            posicionServo1 = max(90, posicionServo1 - 20);
        }
        if (posicionServo2 < 90) {
            posicionServo2 = min(90, posicionServo2 + 20);
        }
        
        // Mover a nueva posición
        servoMotor1.write(posicionServo1);
        servoMotor2.write(posicionServo2);
        
        Serial.print("Nueva posición   - Servo 27: ");
        Serial.print(posicionServo1);
        Serial.print("°, Servo 26: ");
        Serial.print(posicionServo2);
        Serial.println("°");
        Serial.println("==================");
    }
}
void loop() {
    socketWeb.parseMessage();
    
    if (socketWeb.available()) {
        String mensaje = socketWeb.readString();
        Serial.println("Mensaje recibido -> " + mensaje);
        
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, mensaje);
        
        if (error) {
            Serial.print("Error parseando JSON: ");
            Serial.println(error.c_str());
            return;
        }
        
        String comando = doc["data"]["texto"].as<String>();
        Serial.println("Comando extraído: " + comando);
        
        controlLeds(comando);
        controlServo(comando);
        controlMotores(comando);
    }
}