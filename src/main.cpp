#include <Arduino.h>
#include<SoftwareSerial.h>


#define DEBUG					true
#define MAX_MSG_SIZE			128



bool hc12_msg_rcv = false;


SoftwareSerial HC12(10, 11);


void vHC12Send(const char* msg, SoftwareSerial &HC12)
{
    HC12.write(msg);
}



void vHC12Receive(char* buffer, uint16_t buffer_size, SoftwareSerial &HC12)
{ 
    char c; 
    uint16_t i = 0; 
    bool done_reading = false; 
 
    while (HC12.available()) 
    { 
        if(!hc12_msg_rcv) 
            hc12_msg_rcv = true; 
            
        delay(2); 
        if(i == buffer_size) 
        { 
            done_reading = true; 
        } 
 
        c = HC12.read(); 
 
        if(!DEBUG) 
        { 
            Serial.print(c); 
        } 
 
        if(!done_reading) 
            buffer[i++] = c; 
    }     
 
    buffer[i] = '\0'; 
}

void setup() 
{
	Serial.begin(9600);
	HC12.begin(9600);

	delay(3000);

	// vHC12Send("GID:8020211213001, DID:8021211213001, COMMAND: sleep_periodically, VALUE:0", HC12);
	char msg[MAX_MSG_SIZE];

	while (1)
	{
		vHC12Receive(msg, MAX_MSG_SIZE, HC12);

		if(hc12_msg_rcv)
		{
			Serial.println(msg);
			vHC12Send("GID:8020211213001, DID:8021211213001, COMMAND: sleep_periodically, VALUE:10", HC12);
			hc12_msg_rcv = false;
			// break;
		}
	}
	
}


void loop() 
{
  
}