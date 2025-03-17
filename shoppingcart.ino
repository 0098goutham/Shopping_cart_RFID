#include <Wire.h>
#include <LiquidCrystal_I2C_Hangul.h>
LiquidCrystal_I2C_Hangul lcd(0x27, 16, 2);
#include <SPI.h>
#include <MFRC522.h>
#include <IOXhop_FirebaseESP32.h>

#define SS_PIN 5
#define RST_PIN 2
const int buzzer = 13;
int item1price = 0;
int item2price = 0;
int item3price = 0;
int item4price = 0;
int totalprice = 0;


MFRC522 mfrc522(SS_PIN, RST_PIN);

struct Button {
  const uint8_t PIN;
  bool pressed;
};

Button button1 = {12,  false};

void IRAM_ATTR isr1() {
  button1.pressed = true;
}

#define FIREBASE_HOST "smartshopping-f36c7-default-rtdb.firebaseio.com"                         // the project name address from firebase id
#define FIREBASE_AUTH "OucUqyKo5Jzlo53f7ORGxK0a59ZG5U5QirW8P5rB"            // the secret key generated from firebase
#define WIFI_SSID "Vinay"                                           // input your home or public wifi name 
#define WIFI_PASSWORD "Vinay@12345"

String fireStatus;

void setup()
{
  Serial.begin(115200);   // Initiate a serial communication
  delay(200);
  Serial.println("WELCOME");
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  pinMode(buzzer, OUTPUT);
  Wire.begin ();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(4, 0);
  lcd.print("WELCOME");
  delay(2000);
  pinMode(button1.PIN, INPUT_PULLUP);
  attachInterrupt(button1.PIN, isr1, FALLING);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  delay(200);
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  delay(200);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  delay(200);
  //  Firebase.setFloat ("completeflag", 0);
  //  delay(200);
  //  Firebase.setFloat ("billstatus", 0);
  //  delay(200);
  //  Firebase.setFloat ("item1price", 0);
  //  delay(200);
  //  Firebase.setFloat ("item2price", 0);
  //  delay(200);
  //  Firebase.setFloat ("item3price", 0);
  //  delay(200);
  //  Firebase.setFloat ("item4price", 0);
  //  delay(200);
  Firebase.remove("totalprice");
  delay(200);
  Firebase.remove("item1price");
  delay(200);
  Firebase.remove("item2price");
  delay(200);
  Firebase.remove("item3price");
  delay(200);
  Firebase.remove("item4price");
  delay(200);

}

void readrfid()
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if ((content.substring(1) == "E3 41 DE 2E"))
  {
    buzzeron();
    Serial.println("Rice Price:89 ");
    Serial.println();
    item1price = 89;
    content = "";
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Rice Price:89");
    Firebase.setFloat ("item1price", item1price);
    delay(500);

  }

  else if ((content.substring(1) == "21 9B 78 26"))
  {
    buzzeron();
    Serial.println("Sugar Price:49 ");
    Serial.println();
    item2price = 49;
    content = "";
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Sugar Price:49");
    Firebase.setFloat ("item2price", item2price);
    delay(500);


  }

  else if ((content.substring(1) == "65 5D 0A 49"))
  {
    buzzeron();
    Serial.println("Oil Price:106");
    Serial.println();
    item3price = 106;
    content = "";
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Oil Price:106");
    Firebase.setFloat ("item3price", item3price);
    delay(500);

  }

  else if ((content.substring(1) == "CA D2 0A 49"))
  {
    buzzeron();
    Serial.println("Soap Price:184");
    Serial.println();
    item4price = 184;
    content = "";
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Soap Price:184");
    Firebase.setFloat ("item4price", item4price);
    delay(500);

  }

  else   {

    Serial.println("UNKNOWN ITEM");
    buzzer1on();
    lcd.setCursor(0, 1);
    lcd.print("UNKNOWN ITEM     ");
    delay(500);


  }
}

void buzzeron()
{
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  delay(200);
}

void buzzer1on()
{
  digitalWrite(buzzer, HIGH);
  delay(3000);
  digitalWrite(buzzer, LOW);
  delay(200);
}



void loop()
{

  if (button1.pressed) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SHOPPING DONE");
    delay(1000);
    Serial.println("SHOPING DONE" );
    Firebase.setFloat ("completeflag", 1);
    delay(200);
    button1.pressed = false;
    buzzeron();
    lcd.setCursor(0, 1);
    lcd.print("TOTAL BILL:");
    lcd.setCursor(12, 1);
    lcd.print(totalprice);
    delay(500);
    Firebase.setFloat ("totalprice", totalprice);
    delay(200);

  }
  readrfid();
  totalprice = item1price + item2price + item3price + item4price;
  Serial.print("Total Bill:" );
  Serial.println(totalprice);
  delay(10);

  //readfirebase();
}
