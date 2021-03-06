# Test PowerDue Functionalities
Make sure PowerDue LEDs, WiFi, LoRa functions correctly. The testing scripts are in tcpServer_LoRaSend and tcpClient_LoRaReceiver folders. As the name describes, one script is to set up a TCP server and send LoRa messages, and the other script is to set up a TCP client and receives the LoRa message. The LoRa message is sent from the TCP Server, and then it's received from the TCP client through the WiFi communication. During the process, LED blinks on and off to indicate when the message is sent and received. Specific instructions are included below.

## Prerequisites
- Target, Intrumentation and WiFi firmware is flashed correctly onto the board
- LoRa library is installed on the Arduino IDE

## Test Instructions
1. Flash tcpServer_LoRaSend.ino found in the tcpServer_LoRaSend folder onto a PowerDue board and open the target serial port, then you should see the IP address printed out if the wifi module works correctly.
2. Change the IP address in the tcpClient_LoRaReceiver.ino file found in the tcpClient_LoRaReceiver folder to the IP address found in the previous step, and then flash it onto another PowerDue board.
3. On the serial target port that you flashed the tcpServer_LoRaSend code, you should see the printout of "test message" being received for about 7 times with one second interval in between. It shows that the two PowerDues' LEDs, WiFi and LoRa are function correctly. One example is shown below.![example](https://github.com/peteryej/PowerDue_Tests/blob/master/example_printout.jpg)
