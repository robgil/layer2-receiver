#include <U8x8lib.h>

#include <stdio.h>
#include <stdlib.h>

#include <Layer1_LoRa.h>
#include <LoRaLayer2.h>

//using namespace std; 

#define LL2_DEBUG

// the OLED used
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

int counter = 0;

#define LORA_CS 18
#define LORA_RST 14
#define LORA_IRQ 26
#define LORA_FREQ 915E6
#define LED 25

char LOCAL_ADDRESS[9] = "fffff00c";

Layer1Class *Layer1;
LL2Class *LL2;
int txPower = 20;

void setup() {
  SPI.begin(5, 19, 27, 18);

  Serial.begin(9600);
  while (!Serial);
  Serial.println("* Initializing LoRa...");

  // OLED
  u8x8.begin();
  u8x8.setPowerSave(0);
  //u8x8.setFont(u8x8_font_chroma48medium8_r);
  //u8x8.setFont(u8x8_font_artosserif8_r);
  u8x8.setFont(u8x8_font_5x7_f);
  u8x8.drawString(0, 0, "GATEWAY: ");
  u8x8.drawString(8, 0, LOCAL_ADDRESS);
  delay(500);

  // LED Setup
	pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW); 

  Layer1 = new Layer1Class();
  Layer1->setPins(LORA_CS, LORA_RST, LORA_IRQ);
  Layer1->setTxPower(txPower);
  Layer1->setLoRaFrequency(LORA_FREQ);
  if (Layer1->init())
  {
    Serial.println(" --> LoRa initialized");
    LL2 = new LL2Class(Layer1); // initialize Layer2
    LL2->setLocalAddress(LOCAL_ADDRESS); // this should either be randomized or set using the wifi mac address
    LL2->setInterval(10000); // set to zero to disable routing packets
  }
  else
  {
    Serial.println(" --> Failed to initialize LoRa");
  }
}

void loop() {
  LL2->daemon();

  char routes[128];
  char neighbors[128];
  char config[128];

  LL2->getRoutingTable(routes);
  Serial.println(routes);

  LL2->getNeighborTable(neighbors);
  Serial.println(neighbors);

  //LL2->getCurrentConfig(config);
  //Serial.println(config);

  struct Packet packet = LL2->readData();
  Serial.print(((char *)packet.datagram.message));

  delay(1000);
}
