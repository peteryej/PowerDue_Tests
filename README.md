# Test PowerDue Functionalities
Make sure PowerDue LEDs, WiFi, LoRa functions correctly. The testing scripts in tcpServer_LoRaSend and tcpClient_LoRaReceiver folders. As the name describes, one script is to set up a TCP server and send LoRa messages, and the other script is to set up a TCP client and receives the LoRa message. The LoRa message is sent from the TCP Server, and then it's received from the TCP client through the WiFi communication. During the process, LED blinks on and off to indicate when the message is sent and received. Specific instructions are included below.

## Prerequisites
- Target, Intrumentation and WiFi firmware is flashed correctly
- LoRa library is installed on the Arduino IDE

## Test Instructions
1. Flash tcpClient_LoRaReceiver.ino found in the tcpClient_LoRaReceiver folder onto one PowerDue board.
2. Flash tcpServer_LoRaSend.ino found in the tcpServer_LoRaSend folder onto another PowerDue board.
3. Open the serial target port that you flashed the tcpServer_LoRaSend code. You should see the printout of "test message" being received for about 7 times with one second interval in between. It shows that the two PowerDues' LEDs, WiFi and LoRa are function correctly.  
