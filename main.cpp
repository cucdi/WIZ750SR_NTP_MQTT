                 
                              
#include "mbed.h"
#include "NTPClient.h"
#include "SPI.h"
#include "MQTTEthernet.h"
#include "MQTTClient.h"

#define ECHO_SERVER_PORT   7
 
// Nucleo Pin for MFRC522 reset (pick another D pin if you need D8)
//#define MF_RESET    D8
EthernetInterface eth;
NTPClient ntpClient;

 
//Serial connection to PC for output
Serial pc(USBTX, USBRX);
 Serial a(D1,D0);
char c[100]="";
 
int main(void) {
    //pc.printf("starting...\n");
    int cnt = 0;
    
    Serial pc(USBTX, USBRX);
    Serial a(D1,D0);
   // pc.baud(115200);
    printf("Wait a second...\r\n");
    char* topic = "Vending machine";
    MQTTEthernet ipstack = MQTTEthernet();
    
    MQTT::Client<MQTTEthernet, Countdown> client = MQTT::Client<MQTTEthernet, Countdown>(ipstack);
    
    char* hostname = "172.16.73.4";
    int port = 1883;
    
 
     int rc = ipstack.connect(hostname, port);
    if (rc != 0)
    printf("rc from TCP connect is %d\n", rc);
        
    printf("Topic: %s\r\n",topic);
    
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;       
    data.MQTTVersion = 3;
    data.clientID.cstring = "parents";

    if ((rc = client.connect(data)) == 0)
        printf("rc from MQTT connect is %d\n", rc);
           
   // if ((rc = client.subscribe(topic, MQTT::QOS1, messageArrived)) != 0)
//        printf("rc from MQTT subscribe is %d\n", rc); 
// 
  //Init. RC522 Chip

 
  while (true) {
    if (a.readable())
    {
        
        int i;
        c[i]=0;
        for(i=0;i<=15;i++){
        char c1 =a.getc();
        c[i] = c1; 
        } 
   pc.printf("The value returned is %s ",c);
  
    char domainName[3][80] = {"kr.pool.ntp.org", "time.bora.net", "time.nuri.net"};//SET TO DOMAIN NAME OF SERVER GETTING TIME FROM
    char buffer[80]; //BUFFER TO HOLD FORMATTED TIME DATA
    time_t sysTime;
             pc.printf("Server IP Address is %s\r\n", eth.getIPAddress());
    
    pc.printf("Getting time information by using NTP...\r\n");
    
    cnt = 0;
    while(1)
    {
        if(ntpClient.setTime(domainName[cnt],123,0x00005000) != NTP_OK)
        {
            pc.printf("Cannot get time information by NTP\r\n");
            cnt++;
        }
        else
            break;
            
        if(cnt > 3)
        {
            pc.printf("All NTP servers are not resposed!!\r\n");
            return 1;
        }
    }
        
    printf("Completed Get and Set Time\r\n\r\n");
    eth.disconnect();
    
             sysTime = time(NULL)+(3600*5.30); //TIME with offset for eastern time Indian
            //FORMAT TIME FOR DISPLAY AND STORE FORMATTED RESULT IN BUFFER
            strftime(buffer,80,"%Y/%m/%d  %p %I:%M:%S \r\n",localtime(&sysTime));
            pc.printf("Date and Time\r\n%s\r\n", buffer);
            
            
     MQTT::Message message;
    char buf[100];
    sprintf(buf, "%s %s",c,buffer);
     message.qos = MQTT::QOS0;
            message.retained = false;
            message.dup = false;

            message.payload = (void*)buf;
            message.payloadlen = strlen(buf);

            rc = client.publish("RFID", message);
            pc.printf("Rc result: %c \n ",rc);
            client.yield(60);
             }
}
}