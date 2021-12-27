#include <Arduino.h>
#include<SoftwareSerial.h>


#define DEBUG					true
#define MAX_MSG_SIZE			128
#define HC12_TX                 10
#define HC12_RX                 11
#define LED                     9
#define PUSH_BUTTON             2
#define DEBOUNCE_TIME             500



bool hc12_msg_rcv = false;
unsigned long tic;
unsigned int counter = 0;
bool button_pressed = 0;
bool isr_started = false;

SoftwareSerial HC12(HC12_TX, HC12_RX);


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


void vPushButtonPressed()
{
    button_pressed = 1;
}

void concat(char str1[], char str2[], char buff[])
{
    int final_length = sizeof(str1) + sizeof(str2);
    // Serial.println(final_length);

    int i = 0, j = 0;

    while(i < final_length)
    {
        while (str1[j] != '\0')
        {
            buff[i++] = str1[j++];
        }
        j = 0;
        while (str2[j] != '\0')
        {
            buff[i++] = str2[j++];
        }
    }
    buff[i] = '\0';
}

void setup() 
{
    pinMode(LED, OUTPUT);
    pinMode(PUSH_BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON), vPushButtonPressed, RISING);
	Serial.begin(9600);
	HC12.begin(9600);

	// delay(3000);
	
    char msg[MAX_MSG_SIZE], rcv_msg[MAX_MSG_SIZE];
    button_pressed = 0;
    // tic = millis();
	while (1)
	{
        // while (!bPushButtonPressed());

        if(button_pressed)
        {
            button_pressed = 0;
            counter++;

            char count[16];
            itoa(counter, count, 10);
            concat("Message count: ", count, msg);
            Serial.println(msg);
            vHC12Send(msg, HC12);
        }
        
		vHC12Receive(rcv_msg, MAX_MSG_SIZE, HC12);

		if(hc12_msg_rcv)
		{
            char return_msg[MAX_MSG_SIZE];
            concat("Echo ", msg, return_msg);
            Serial.println(rcv_msg);

            if(!strcmp(return_msg, rcv_msg))
            {
                digitalWrite(LED, HIGH);
                delay(250);
                digitalWrite(LED, LOW);
                delay(250);
                digitalWrite(LED, HIGH);
                delay(250);
                digitalWrite(LED, LOW);
                delay(250);
            }
            else
            {
                digitalWrite(LED, HIGH);
                delay(1000);
                digitalWrite(LED, LOW);
            }
			hc12_msg_rcv = false;

            // delay(500);
            // vHC12Send("Echo ", HC12);
            // vHC12Send(msg, HC12);
		}
	}
	
}


void loop() 
{
  
}