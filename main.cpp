#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// ---------- WiFi ----------
const char* ssid = "ESP32-Robot";
const char* password = "12345678";

// ---------- Motor Pins ----------
#define IN1 26
#define IN2 27
#define IN3 14
#define IN4 12
#define ENA 25
#define ENB 33

// ---------- Ultrasonic ----------
#define TRIG 5
#define ECHO 18

// ---------- Objects ----------
WebServer server(80);
Adafruit_MPU6050 mpu;

// ---------- Variables ----------
String motionState = "STOP";
float distanceCM = 0;
float ax, ay, az;

// ---------- HTML Dashboard ----------
String webpage() {
  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>Grain Weevil Robot</title>
<style>
body{font-family:Arial;text-align:center;background:#111;color:white}
button{padding:15px;margin:8px;font-size:18px;width:140px}
.card{border:1px solid #444;padding:15px;margin:10px}
</style>
</head>
<body>

<h2>Grain Weevil Robot – ESP32</h2>

<div class="card">
<button onclick="send('F')">FORWARD</button><br>
<button onclick="send('L')">LEFT</button>
<button onclick="send('S')">STOP</button>
<button onclick="send('R')">RIGHT</button><br>
<button onclick="send('B')">BACKWARD</button>
</div>

<div class="card">
<p>Distance: <span id="d">--</span> cm</p>
<p>IMU AX: <span id="ax">--</span></p>
<p>IMU AY: <span id="ay">--</span></p>
<p>IMU AZ: <span id="az">--</span></p>
<p>State: <span id="st">--</span></p>
</div>

<script>
function send(cmd){
  fetch("/cmd?m="+cmd);
}
setInterval(()=>{
fetch("/data").then(r=>r.json()).then(d=>{
document.getElementById("d").innerHTML=d.distance;
document.getElementById("ax").innerHTML=d.ax;
document.getElementById("ay").innerHTML=d.ay;
document.getElementById("az").innerHTML=d.az;
document.getElementById("st").innerHTML=d.state;
});
},1000);
</script>

</body>
</html>
)rawliteral";
}

// ---------- Motor Control ----------
void stopMotors() {
  digitalWrite(IN1,LOW); digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW); digitalWrite(IN4,LOW);
  motionState="STOP";
}

void forward() {
  digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);
  motionState="FORWARD";
}

void backward() {
  digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH);
  motionState="BACKWARD";
}

void left() {
  digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);
  motionState="LEFT";
}

void right() {
  digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH);
  motionState="RIGHT";
}

// ---------- Ultrasonic ----------
float getDistance() {
  digitalWrite(TRIG,LOW); delayMicroseconds(2);
  digitalWrite(TRIG,HIGH); delayMicroseconds(10);
  digitalWrite(TRIG,LOW);
  long duration = pulseIn(ECHO,HIGH,30000);
  return duration * 0.034 / 2;
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);

  pinMode(IN1,OUTPUT); pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT); pinMode(IN4,OUTPUT);
  pinMode(ENA,OUTPUT); pinMode(ENB,OUTPUT);

  pinMode(TRIG,OUTPUT);
  pinMode(ECHO,INPUT);

  analogWrite(ENA,180);
  analogWrite(ENB,180);

  Wire.begin();
  mpu.begin();

  WiFi.softAP(ssid,password);
  Serial.println(WiFi.softAPIP());

  server.on("/",[](){ server.send(200,"text/html",webpage()); });

  server.on("/cmd",[](){
    String m = server.arg("m");
    if(m=="F") forward();
    else if(m=="B") backward();
    else if(m=="L") left();
    else if(m=="R") right();
    else stopMotors();
    server.send(200,"text/plain","OK");
  });

  server.on("/data",[](){
    sensors_event_t a,g,temp;
    mpu.getEvent(&a,&g,&temp);
    distanceCM = getDistance();
    ax=a.acceleration.x; ay=a.acceleration.y; az=a.acceleration.z;

    // CSV LOG
    Serial.printf("%s,%.2f,%.2f,%.2f,%.2f\n",
      motionState.c_str(),distanceCM,ax,ay,az);

    server.send(200,"application/json",
      "{\"distance\":"+String(distanceCM)+
      ",\"ax\":"+String(ax)+
      ",\"ay\":"+String(ay)+
      ",\"az\":"+String(az)+
      ",\"state\":\""+motionState+"\"}");
  });

  server.begin();
}

// ---------- Loop ----------
void loop() {
  server.handleClient();
}
