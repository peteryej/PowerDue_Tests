#include <assert.h>
#include <FreeRTOS_ARM.h>
#include <IPAddress.h>
#include <PowerDueWiFi.h>
#include <LoRa.h>

// update these
#define WIFI_SSID "PowerDue"
#define WIFI_PASS "powerdue"

#define INET_LEN 16

/* The size of the buffer in which the dynamic WEB page is created. */
#define MESSAGE_SIZE  100

/* The port on which we listen. */
#define CUSTOM_PORT    ( 9999 )

// fixed parameters
#define FREQUENCY         920E6   // 915MHz
#define BANDWIDTH         125000  // 125kHz bandwidth
#define SLEEPTIME         4000    // 4 seconds

// vary these parameters
#define TX_POWER          20   // valid values are from 6 to 20
#define SPREADING_FACTOR  7    // valid values are 7, 8, 9 or 10

#define MESSAGE            "test message"

/*------------------------------------------------------------*/

void vProcessConnection( void *client )
{
  int clientFd = (int)client;
  char buf[MESSAGE_SIZE], cPageHits[ 11 ];;
  static unsigned long ulPageHits = 0;
  if(clientFd > 0){
    SerialUSB.println("Client connected!");
    // wait for it to send something
    if( lwip_read(clientFd, buf, MESSAGE_SIZE) > 0) {
      SerialUSB.print("Received: ");
      SerialUSB.println(buf);
    }

    // close client connection
    lwip_close(clientFd);
    SerialUSB.println("client connection closed");
  }
  vTaskDelete(NULL);
}

void tcpServer(void * argument)
{  
  struct sockaddr_in addr;  
  struct sockaddr_in clientAddr;
  socklen_t clientLen;
  
  memset(&addr, 0, sizeof(addr));
  memset(&clientAddr, 0, sizeof(clientAddr));

  addr.sin_len = sizeof(addr);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(CUSTOM_PORT);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  
  int serverSocket = lwip_socket(AF_INET, SOCK_STREAM, 0);
  int err = lwip_bind(serverSocket, (struct sockaddr *)&addr, sizeof(addr));
  if(err != 0){
    SerialUSB.println("Failed to bind to server socket!!");
    assert(false);
  } 
  
  SerialUSB.print("Server sock fd: ");
  SerialUSB.println(serverSocket);
  
  err = lwip_listen(serverSocket, 10);
  if(err != 0){
    SerialUSB.print("Listen failed?! ");
    SerialUSB.println(err);
    assert(false);
  }
  
  while(1){
    // wait for incoming connections
    SerialUSB.println("Waiting for client...");
    int clientFd = lwip_accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLen);

    turnOffLED();
    for (int i =0; i<10; i++){
      sendLoRaMsg();
    }
    turnOnLED();

    
    // get the details
    char str[INET_LEN];
    // now get it back and print it
    inet_ntop(AF_INET, &(clientAddr.sin_addr), str, INET_LEN);
    SerialUSB.print("Client: ");
    SerialUSB.print(str);
    SerialUSB.print(":");
    SerialUSB.println(ntohs(clientAddr.sin_port));
    
    xTaskCreate(vProcessConnection, "Connection", configMINIMAL_STACK_SIZE*10, (void *)clientFd, 1, NULL);
  }
  // close socket after everything is done
  lwip_close(serverSocket);
  SerialUSB.println("server socket closed");
}

/*------------------------------------------------------------*/

void initPins(){
  // turn off LEDs
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  turnOffLED();
}

void turnOffLED(){
  digitalWrite(6,LOW);
  digitalWrite(7,LOW);
  digitalWrite(8,LOW);
}

void turnOnLED(){
  digitalWrite(6,HIGH);
  digitalWrite(7,HIGH);
  digitalWrite(8,HIGH);
}

void onError(int errorCode){
  SerialUSB.print("Error received: ");
  SerialUSB.println(errorCode);
}

void onReady(){
  SerialUSB.println("Device ready");  
  SerialUSB.print("Device IP: ");
  SerialUSB.println(IPAddress(PowerDueWiFi.getDeviceIP()));  
  
  xTaskCreate(tcpServer, "tcpServer", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
}

void sendLoRaMsg(){
  // send LoRa packet
  LoRa.beginPacket();
  LoRa.print(MESSAGE);
  LoRa.endPacket();  
}


void setup() {
  initPins();
  SerialUSB.begin(0);
  while(!SerialUSB);
  
  
  
  SerialUSB.println("LoRa Sender");

  LoRa.setPins(22, 59, 51);
  while (!LoRa.begin(FREQUENCY)) {
    SerialUSB.println("Starting LoRa failed!");
    delay(200);
  }

  LoRa.setTxPower(TX_POWER);
  LoRa.setSpreadingFactor(SPREADING_FACTOR);
  LoRa.setSignalBandwidth(BANDWIDTH);
  
  turnOnLED();

  PowerDueWiFi.init(WIFI_SSID, WIFI_PASS);
  PowerDueWiFi.setCallbacks(onReady, onError);
   
  vTaskStartScheduler();
  SerialUSB.println("Insufficient RAM");
  while(1);
}

void loop() {
  // not used in freertos
}
