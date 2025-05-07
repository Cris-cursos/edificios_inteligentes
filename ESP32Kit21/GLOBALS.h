#define RFID_SDA 5     // ESP32 pin GPIO05
#define RFID_RST 4     // ESP32 pin GPIO04
#define RFID_MISO 19   // ESP32 pin GPIO19
#define RFID_MOSI 23   // ESP32 pin GPIO23
#define RFID_SCK 18    // ESP32 pin GPIO18
#define doorOpen 2     // ESP32 pin GPIO02
#define microPin 34    // ESP32 pin GPIO34
#define detectPin 16   // ESP32 pin GPIO16
#define pirPin 26      // ESP32 pin GPIO26
#define NUMITEMS(arg) ((unsigned int) (sizeof (arg) / sizeof (arg[0])))
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_MOSI   13 // ESP32 pin GPIO13
#define OLED_D0     14 // ESP32 pin GPIO14
#define OLED_DC     17 // ESP32 pin GPIO17
#define OLED_CS     15 // ESP32 pin GPIO15
#define OLED_RESET  25 // ESP32 pin GPIO25

WiFiClient espClient;
PubSubClient mqttClient(espClient);
MFRC522 rfid(RFID_SDA, RFID_RST);
BH1750 lightMeter;
Adafruit_CCS811 ccs;
Adafruit_ADS1115 ads;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_D0, OLED_DC, OLED_RESET, OLED_CS);
Scheduler runner;

//******** MQTT CONFIG *********//
String newTopic;
String payload;
String content;
String TOPICS_PUBLICATIONS[] = { "esp21/Identification","esp21/Noise","esp21/Lux","esp21/Motion","esp21/PirDetect","esp21/Power","esp21/CO2","esp21/DoorOpen"};
String TOPICS_SUBSCRIBED[] = { "esp21/AccessGranted" };
String uid = "";
//******** RELAY OUTPUT *********//
int accessGranted = 0;
//******** PIR SENSOR *********//
bool pirMotion = 0;
//******** MOTION SENSOR *********//
bool detectMotion = 0;
//******** POWER SENSOR *********//
const float FACTOR = 30;
const float multiplier = 0.0625F;
const float voltage = 220;