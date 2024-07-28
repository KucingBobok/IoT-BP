#include <Q2HX711.h>
#include <Average.h>
#include <U8x8lib.h>

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include "time.h"

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

WiFiMulti wifiMulti;

const uint32_t connectTimeoutMs = 1000;
unsigned long checkFirebase = 0;

// Insert Firebase project API Key
#define API_KEY "AIzaSyAyauqYEJCCeXXRlERU-N3xRRR715B0Ijg"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "afikrazanafi@gmail.com"
#define USER_PASSWORD "11223344"

// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "https://backend-e21c9-default-rtdb.asia-southeast1.firebasedatabase.app/"

unsigned long bpm;
int counter;
int motor = D1;
int solenoid = D0;
float dataadc;
int tombol = D3;     // Tombol Start
int userTombol = D6; // Pemilihan User
int tombolx;
int hitung;
double difference;
float pressure;
int mmhg;
int mmhgx;
int sistole;
int between;
int diastole;
int sistolex;
int diastolex;
int MAPres;
int mark = 0;
int count = 1;
int countPulse = 0;
int pump = 1;
int countBP = 0;
int countBPx;
int fail;
int exitHasil;
int tombolExit;
int checkS;
const int maxReadings = 500; // Adjust based on your needs
int oscillationAmplitudes[maxReadings];
int pressureValues[maxReadings];
int numReadings = 0;
int skipConnection = 0;

int userCount;
String userSelect;

float lastDifference;
float differencePres;

String HMS;
String monthDay;

float data1;
float data2;
float data3;
unsigned long time1;
unsigned long time2;

int globalDelay = 20;
float offsetSys = 800;
float offsetDia = 12000;

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

// Database main path (to be updated in setup with the user UID)
String databasePath;
// Database child nodes
String sisPath = "/sistole";
String diaPath = "/diastole";
String mapPath = "/map";
String bpPath = "/hr";
String timePath = "/timestamp";

// Parent Node (to be updated in every loop)
String parentPath;

int timestamp;
FirebaseJson json;

const char *ntpServer = "pool.ntp.org";

const long gmtOffset_seconds = 25200;
const int daylightOffset_seconds = 3600;

const byte MPS_OUT_pin = D8; // OUT data pin
const byte MPS_SCK_pin = D9; // clock data pin
int avg_size = 1;            // #pts to average over

U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(D5, D4, U8X8_PIN_NONE);
// U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0);

Q2HX711 MPS20N0040D(MPS_OUT_pin, MPS_SCK_pin); // start comm with the HX710B
Average<long> ave(avg_size);

// Initialize WiFi
void initWiFi()
{
  u8x8.drawString(0, 1, "Starting ..");
  WiFi.setMinSecurity(WIFI_AUTH_WPA2_PSK);
  WiFi.mode(WIFI_STA);
  int n = WiFi.scanNetworks();
  if (n == 0)
  {
    Serial.println("no networks found");
    u8x8.drawString(0, 3, "No network");
    u8x8.drawString(0, 4, "found");
  }
  else
  {
    u8x8.drawString(0, 3, "Network found:");
    u8x8.drawString(0, 4, String(n).c_str());
  }
  delay(500);
  u8x8.drawString(0, 0, "Connecting to");
  u8x8.drawString(0, 1, "WiFi ..        ");
  Serial.print("Connecting to WiFi ..");
  int TryCount = 0;
  wifiMulti.run();
  while (wifiMulti.run() != WL_CONNECTED)
  {
    u8x8.drawString(0, 0, "Connecting to");
    u8x8.drawString(0, 1, "WiFi ..");
    u8x8.drawString(0, 2, "Loading.    ");
    delay(200);
    u8x8.drawString(0, 2, "Loading..");
    delay(200);
    u8x8.drawString(0, 2, "Loading...");
    delay(200);
    u8x8.drawString(0, 2, "Loading....");
    delay(200);
    u8x8.drawString(0, 2, "Loading.....");
    delay(200);
    Serial.print('.');
    wl_status_t wifiStatus = WiFi.status();
    String statusString;
    switch (wifiStatus)
    {
    case WL_IDLE_STATUS:
      statusString = "Idle";
      break;
    case WL_NO_SSID_AVAIL:
      statusString = "No SSID available";
      break;
    case WL_CONNECTED:
      statusString = "Connected";
      break;
    case WL_CONNECT_FAILED:
      statusString = "Connection failed";
      break;
    case WL_DISCONNECTED:
      statusString = "Disconnected";
      break;
    default:
      statusString = "Unknown status";
      break;
    }
    u8x8.drawString(0, 5, statusString.c_str());
    if (TryCount == 100)
    {
      ESP.restart();
    }
    TryCount++;
  }
  if (wifiMulti.run() == WL_CONNECTED)
  {
    Serial.println(WiFi.localIP());
    u8x8.drawString(0, 0, "Connected!      ");
    u8x8.drawString(0, 1, "IP:             ");
    u8x8.drawString(0, 2, WiFi.SSID().c_str());
    u8x8.drawString(0, 3, WiFi.localIP().toString().c_str());
    delay(2000);
  }
  u8x8.clear();
}

// Function that gets current epoch time
unsigned long getTime()
{
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}

void dbConnection()
{

  int connectionMenu = 0;
  int connectionCount = 0;
  u8x8.drawString(0, 0, "Connect to");
  u8x8.drawString(0, 1, "Database?");
  while (connectionMenu == 0)
  {
    int tombolMenu = digitalRead(userTombol);
    int tombolExit = digitalRead(tombol);
    if (tombolMenu == LOW)
    {
      delay(250);
      connectionCount++;
    }
    if (connectionCount = 0)
    {
      u8x8.drawString(0, 4, "  >Connect<   ");
      u8x8.drawString(0, 5, "  Disconnect  ");
    }
    if (connectionCount = 1)
    {
      u8x8.drawString(0, 4, "   Connect    ");
      u8x8.drawString(0, 5, " >Disconnect< ");
    }
    if (connectionCount = 2)
    {
      connectionMenu = 0;
    }
    if (tombolExit == LOW)
    {

      connectionMenu = 1;
      skipConnection = 1;
    }
  }
}

void setup()
{
  u8x8.begin();
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);

  pinMode(motor, OUTPUT);
  pinMode(solenoid, OUTPUT);
  pinMode(tombol, INPUT_PULLUP);
  pinMode(userTombol, INPUT_PULLUP);

  // dbConnection();

  if (skipConnection == 0)
  {
    wifiMulti.addAP("afikaa", "1122334455");
    wifiMulti.addAP("backup", "12345678");

    u8x8.setFont(u8x8_font_chroma48medium8_r);
    initWiFi();
    configTime(gmtOffset_seconds, daylightOffset_seconds, ntpServer);

    // Assign the api key (required)
    config.api_key = API_KEY;

    // Assign the user sign in credentials
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    // Assign the RTDB URL (required)
    config.database_url = DATABASE_URL;

    Firebase.reconnectWiFi(true);
    fbdo.setResponseSize(4096);

    // Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

    // Assign the maximum retry of token generation
    config.max_token_generation_retry = 5;

    // Initialize the library with the Firebase authen and config
    Firebase.begin(&config, &auth);

    // Getting the user UID might take a few seconds
    Serial.println("Getting User UID");
    while ((auth.token.uid) == "")
    {
      u8x8.drawString(0, 0, "Getting User UID");
      u8x8.setFont(u8x8_font_chroma48medium8_r);
      u8x8.drawString(0, 2, "Loading.");
      delay(200);
      u8x8.drawString(0, 2, "Loading..");
      delay(200);
      u8x8.drawString(0, 2, "Loading...");
      delay(200);
      u8x8.drawString(0, 2, "Loading....");
      delay(200);
      u8x8.drawString(0, 2, "Loading.....");
      delay(200);
      u8x8.clear();
      Serial.print('.');
    }
    // Print user UID
    u8x8.clear();
    uid = auth.token.uid.c_str();
    Serial.print("User UID: ");
    u8x8.drawString(0, 1, "User ID:");
    u8x8.drawString(0, 2, uid.c_str());
    Serial.println(uid);
    delay(500);
  }
  MPS20N0040D.setGain(64);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.clear();
}

// Global pressure measure function
void measure()
{
  ave.push(MPS20N0040D.read());
  float dataadc = ave.mean();
  pressure = dataadc;
  float datatoMmhg = (dataadc - 8671782) * (168 - 0) / (12582911 - 8671782);
  mmhg = map(datatoMmhg, 0, 168, 0, 168);
}

// OLED normal display function
void display(float dataDisplay, int lineX, int lineY)
{
  char datasen[16];
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(lineX, lineY, dtostrf(dataDisplay, 4, 0, datasen));
}

// OLED 2x2 display function
void display2x2(float dataDisplay, int lineX, int lineY)
{
  char datasen[16];
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.draw2x2String(lineX, lineY, dtostrf(dataDisplay, 4, 0, datasen));
}

// Upload data to Database
void uploadDB()
{
  u8x8.clear();
  if (Firebase.ready())
  {
    // Getting time and display to OLED
    timestamp = getTime();
    Serial.print("time: ");

    display(0, 2, long(timestamp));
    u8x8.drawString(0, 0, "Uploading to DB");

    parentPath = databasePath + "/" + String(timestamp);

    // Getting data and parsing to json format
    json.set(sisPath.c_str(), String(sistolex));
    json.set(diaPath.c_str(), String(diastolex));
    json.set(mapPath.c_str(), String(MAPres));
    json.set(bpPath.c_str(), String(bpm));
    json.set(timePath, String(timestamp));
    // Sending json to database
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
    u8x8.drawString(0, 4, fbdo.errorReason().c_str());
    delay(1000);
    u8x8.clear();
  }
}

// Initial pump to pressure
void pumping()
{
  u8x8.clear();
  while (mmhg <= 160 && pump == 1)
  {
    measure();
    Serial.println(mmhg);
    display2x2(mmhg, 2, 2);
    if (mmhg >= 160)
    {
      delay(400);
      digitalWrite(motor, LOW);
      delay(50);
      pump = 0;
      countPulse = 0;
      delay(200);
    }
  }
}

// Main detection function
void MAP()
{
  u8x8.clear();

  while ((countPulse == 0) || (mmhg < 55))
  {
    measure();
    display2x2(mmhg, 2, 2);
    data1 = pressure;
    delay(globalDelay);
    data2 = pressure;

    while ((data1 + offsetSys > data2))
    {
      measure();
      display2x2(mmhg, 2, 2);
      Serial.println("Sis valley");
      Serial.println(data1 + offsetSys - data2);
      data1 = data2;
      delay(globalDelay);
      data2 = pressure;

      if (mmhg < 60)
      {
        countPulse = 1;
        return;
      }

      if (count > 400)
      {
        countPulse = 1;
        fail = 1;
        mark = 4;
        return;
      }
      count++;
    }

    count = 0;
    measure();
    data3 = pressure;

    while (data3 >= data2)
    {
      Serial.println(data3 - data2);
      Serial.println("Sis peak");
      measure();
      display2x2(mmhg, 2, 2);
      data2 = data3;
      delay(globalDelay);
      data3 = pressure;
    }

    u8x8.draw2x2String(10, 5, "B");
    delay(globalDelay);
    u8x8.draw2x2String(10, 5, " ");
    delay(globalDelay);
    difference = data3 - data1;

    // Store oscillation amplitude and corresponding pressure
    if (numReadings < maxReadings)
    {
      float datatoMmhg = (data1 - 8671782) * (168 - 0) / (12582911 - 8671782);
      int mmhgR = map(datatoMmhg, 0, 168, 0, 168);
      oscillationAmplitudes[numReadings] = difference;
      pressureValues[numReadings] = mmhgR;
      numReadings++;
    }

    display(countBP, 12, 6);

    if (countBP == 0)
    {
      time1 = millis();
    }

    if (countBP > 1)
    {
      time2 = millis();
    }

    countBP++;
    mark = 4;
    Serial.println("Sis S= ");
    Serial.println(mmhg);
    Serial.print(data1);
    Serial.print("  ");
    Serial.print(data2);
    Serial.print("  ");
    Serial.print(data3);
    Serial.print("  ");
    Serial.print(difference);
    Serial.print("     ");
    Serial.println(lastDifference);
  }
}

// Calculating relative systolic and diastolic pressure and bpm values
void calculate()
{
  int maxAmplitude = 0;
  int maxIndex = 0;

  // Find the maximum amplitude and its index
  for (int i = 0; i < numReadings; i++)
  {
    if (oscillationAmplitudes[i] > maxAmplitude)
    {
      maxAmplitude = oscillationAmplitudes[i];
      maxIndex = i;
    }
  }

  // Retrieving maximum mean pressure
  MAPres = pressureValues[maxIndex];

  // Systolic pressure is the pressure at approximately 0.55 * maxAmplitude before the max
  // Diastolic pressure is the pressure at approximately 0.85 * maxAmplitude after the max
  for (int i = maxIndex; i >= 0; i--)
  {
    if (oscillationAmplitudes[i] <= 0.45 * maxAmplitude)
    {
      sistole = pressureValues[i];
      break;
    }
  }

  for (int i = maxIndex; i < numReadings; i++)
  {
    if (oscillationAmplitudes[i] <= 0.75 * maxAmplitude)
    {
      diastole = pressureValues[i];
      break;
    }
  }

  // Calculating BPM
  bpm = 60000 / ((time2 - time1) / countBP);

  sistolex = sistole;
  diastolex = diastole;

  delay(500);
  mark = 4;
}

// Displaying results to OLED display
void hasil()
{
  u8x8.clear();
  while (exitHasil == 0)
  {
    tombolExit = digitalRead(tombol);
    if (tombolExit == LOW)
    {
      delay(250);
      exitHasil = 1;
    }
    u8x8.draw2x2String(0, 0, "SYS");
    u8x8.draw2x2String(0, 2, "MAP");
    u8x8.draw2x2String(0, 4, "DIA");
    u8x8.draw2x2String(0, 6, "BPM");
    display2x2(sistolex, 7, 0);
    display2x2(MAPres, 7, 2);
    display2x2(diastolex, 7, 4);
    display2x2(bpm, 7, 6);
  }
  delay(500);
  u8x8.clear();
  return;
}

// Main procedure function
void mulai()
{
  pumping();
  MAP();
  calculate();
  if (fail == 1)
  {
    u8x8.clear();
    fail = 0;
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.drawString(0, 7, "failed, no systolic");
    delay(2000);
    u8x8.drawString(3, 7, "                   ");
    digitalWrite(solenoid, LOW);
    return;
  }
  if ((mark == 4) || (mmhg < 60))
  {
    delay(1000);
    mark = 0;
    count = 0;
    exitHasil = 0;

    digitalWrite(solenoid, LOW);
    if (skipConnection = 0)
    {
      uploadDB();
    }
    hasil();
    return;
  }

  delay(1);
}

void loop()
{
  u8x8.drawString(0, 0, "Pilih Pengguna: ");
  // u8x8.drawString(0, 6, "Tekan tombol ukur setelah");
  // u8x8.drawString(0, 7, "memilih user");
  Serial.println("Check Button");
  int tombolUser = digitalRead(userTombol);
  if (tombolUser == LOW)
  {
    delay(250);
    userCount++;
  }

  if (userCount == 0)
  {
    u8x8.drawString(0, 1, "Afik <");
    u8x8.drawString(0, 2, "Mom    ");
    u8x8.drawString(0, 3, "Gran   ");
    u8x8.drawString(0, 4, "User1  ");
    u8x8.drawString(0, 5, "User2  ");
    u8x8.drawString(0, 6, "User3  ");
    u8x8.drawString(0, 7, "User4        ");
    databasePath = "/afik";
  }
  if (userCount == 1)
  {
    u8x8.drawString(0, 1, "Afik   ");
    u8x8.drawString(0, 2, "Mom  <");
    u8x8.drawString(0, 3, "Gran   ");
    u8x8.drawString(0, 4, "User1  ");
    u8x8.drawString(0, 5, "User2  ");
    u8x8.drawString(0, 6, "User3  ");
    u8x8.drawString(0, 7, "User4        ");
    databasePath = "/mom";
  }
  if (userCount == 2)
  {
    u8x8.drawString(0, 1, "Afik   ");
    u8x8.drawString(0, 2, "Mom    ");
    u8x8.drawString(0, 3, "Gran <");
    u8x8.drawString(0, 4, "User1  ");
    u8x8.drawString(0, 5, "User2  ");
    u8x8.drawString(0, 6, "User3  ");
    u8x8.drawString(0, 7, "User4  ");
    databasePath = "/gran";
  }
  if (userCount == 3)
  {
    u8x8.drawString(0, 1, "Afik   ");
    u8x8.drawString(0, 2, "Mom    ");
    u8x8.drawString(0, 3, "Gran   ");
    u8x8.drawString(0, 4, "User1 <");
    u8x8.drawString(0, 5, "User2  ");
    u8x8.drawString(0, 6, "User3  ");
    u8x8.drawString(0, 7, "User4  ");
    databasePath = "/user1";
  }
  if (userCount == 4)
  {
    u8x8.drawString(0, 1, "Afik   ");
    u8x8.drawString(0, 2, "Mom    ");
    u8x8.drawString(0, 3, "Gran   ");
    u8x8.drawString(0, 4, "User1  ");
    u8x8.drawString(0, 5, "User2 <");
    u8x8.drawString(0, 6, "User3  ");
    u8x8.drawString(0, 7, "User4  ");
    databasePath = "/user2";
  }
  if (userCount == 5)
  {
    u8x8.drawString(0, 1, "Afik   ");
    u8x8.drawString(0, 2, "Mom    ");
    u8x8.drawString(0, 3, "Gran   ");
    u8x8.drawString(0, 4, "User1  ");
    u8x8.drawString(0, 5, "User2  ");
    u8x8.drawString(0, 6, "User3 <");
    u8x8.drawString(0, 7, "User4  ");
    databasePath = "/user3";
  }
  if (userCount == 6)
  {
    u8x8.drawString(0, 1, "Afik   ");
    u8x8.drawString(0, 2, "Mom    ");
    u8x8.drawString(0, 3, "Gran   ");
    u8x8.drawString(0, 4, "User1  ");
    u8x8.drawString(0, 5, "User2  ");
    u8x8.drawString(0, 6, "User3  ");
    u8x8.drawString(0, 7, "User4 <");
    databasePath = "/user4";
  }
  if (userCount == 7)
  {
    u8x8.drawString(0, 1, "Afik   ");
    u8x8.drawString(0, 2, "Mom    ");
    u8x8.drawString(0, 3, "Gran   ");
    u8x8.drawString(0, 4, "User1  ");
    u8x8.drawString(0, 5, "User2  ");
    u8x8.drawString(0, 6, "User3  ");
    u8x8.drawString(0, 7, "User4  > DC");
  }
  if (userCount == 8)
  {
    userCount = 0;
  }

  tombolx = digitalRead(tombol); // Membaca input dari Tombol Start

  if (tombolx == LOW && userCount <= 6)
  {
    mark = 0;
    numReadings = 0; // Reset number of readings

    checkS = 0;
    lastDifference = 0;
    countBP = 0;
    pump = 1;

    for (int i = 0; i < maxReadings; i++)
    {
      oscillationAmplitudes[i] = 0;
      pressureValues[i] = 0;
    }
    numReadings = 0;

    delay(1000);
    digitalWrite(motor, HIGH);
    digitalWrite(solenoid, HIGH);
    mulai();
  }

  if (tombolx == LOW && userCount == 7)
  {
    WiFi.disconnect();
    u8x8.clear();
    u8x8.draw2x2String(1, 3, "UNPLUG");
    delay(5000);
  }

  if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED)
  {
    u8x8.drawString(15, 3, "W");
  }
  else
  {
    u8x8.drawString(15, 3, " ");
    Serial.println("WiFi not connected!");
  }
  if ((millis() - checkFirebase > 15000 || checkFirebase == 0) && Firebase.ready())
  {
    checkFirebase = millis();
    u8x8.drawString(15, 4, "F");
  }
  if ((millis() - checkFirebase > 15000 || checkFirebase == 0) && Firebase.ready() == false)
  {
    checkFirebase = millis();
    u8x8.drawString(15, 4, " ");
  }

  delay(10);
}