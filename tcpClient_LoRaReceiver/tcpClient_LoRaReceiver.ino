#include <assert.h>
#include <FreeRTOS_ARM.h>
#include <IPAddress.h>
#include <PowerDueWiFi.h>
#include <Arduino.h>
#include <LoRa.h>

// update these
#define WIFI_SSID "PowerDue"
#define WIFI_PASS "powerdue"


#define SERVER_PORT 9999

// update the IP address below to the IP address printed by the server
#define SERVER_IP "10.230.12.99"


/*------------------------------------------------------------*/

// fixed parameters
#define FREQUENCY         915E6   // 915MHz
#define BANDWIDTH         125000  // 125kHz bandwidth
#define SLEEPTIME         4000    // 4 seconds

// vary these parameters
#define TX_POWER          20   // valid values are from 6 to 20
#define SPREADING_FACTOR  7    // valid values are 7, 8, 9 or 10
#define PASSWORD          "crossmobile"

#define DATALEN 100

/*------------------------------------------------------------*/
char buf[DATALEN];
uint8_t packetSize = 0; 

void getLoRaMsg(){
  while(1) {
    packetSize = LoRa.parsePacket();
    if (packetSize > 0) {
      // read packet
      int i=0;
      
      while (LoRa.available()) {
        buf[i] = (char)LoRa.read();
        ++i;
      }      
      SerialUSB.print("received LoRa MSG: ");
      SerialUSB.println(buf);
      return;
    }
  }
}

void tcpClient(void * argument)
{  
  while(true){
    struct sockaddr_in serverAddr;  
    socklen_t socklen;
    memset(&serverAddr, 0, sizeof(serverAddr));
  
    serverAddr.sin_len = sizeof(serverAddr);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT); 
    inet_pton(AF_INET, SERVER_IP, &(serverAddr.sin_addr)); 
  
  
    int s = lwip_socket(AF_INET, SOCK_STREAM, 0);
  
    while(lwip_connect(s, (struct sockaddr *)&serverAddr, sizeof(serverAddr))){
      lwip_close(s);
      SerialUSB.println("Failed to connect to server. Retrying...");
      s = lwip_socket(AF_INET, SOCK_STREAM, 0);
      delay(500);
    }
    SerialUSB.println("Connected to server");

    turnOffLEDs();
    getLoRaMsg();
    turnOnLEDs();
        
    // send data  
    if (lwip_write(s, buf, DATALEN)){
      SerialUSB.println("sent");
    }else{
      SerialUSB.println("failed to send");
    }
    
    // close socket after everything is done
    lwip_close(s);
    SerialUSB.println("socket closed");
    memset(buf, 0, DATALEN);
    delay(1000);
  }

}

/*------------------------------------------------------------*/

void initLEDs(){
  // turn off LEDs
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  turnOffLEDs();
}

void turnOffLEDs(){
  digitalWrite(6,LOW);
  digitalWrite(7,LOW);
  digitalWrite(8,LOW);
}

void turnOnLEDs(){
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

  xTaskCreate(tcpClient, "tcpClient", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
}

void setup() {
  initLEDs();
  SerialUSB.begin(115200);
  //while(!SerialUSB);
  

  LoRa.setPins(22, 59, 51);
  while(!LoRa.begin(FREQUENCY)) {
    SerialUSB.println("LoRa begin retrying... ");
    delay(100);
  }
  turnOnLEDs();
  
  LoRa.setTxPower(TX_POWER);
  LoRa.setSpreadingFactor(SPREADING_FACTOR);
  LoRa.setSignalBandwidth(BANDWIDTH);

  
  PowerDueWiFi.init(WIFI_SSID, WIFI_PASS);
  PowerDueWiFi.setCallbacks(onReady, onError);
   
  vTaskStartScheduler();
  SerialUSB.println("Insufficient RAM");
  while(1);
}

void loop() {
  // not used in freertos
}
