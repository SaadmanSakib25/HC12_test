#include <Arduino.h>
#include<SoftwareSerial.h>
#include <EEPROM.h>

#define DEBUG					true
#define MAX_MSG_SIZE			128
#define HC12_TX                 10
#define HC12_RX                 11
#define LED                     9
#define LED2                    8
#define PUSH_BUTTON_1           2
#define PUSH_BUTTON_2           3
#define DEBOUNCE_TIME           500
#define NumOf_BAUDRATE          6

uint32_t baudRates[NumOf_BAUDRATE] = {1200, 2400, 4800, 9600, 57600, 115200};

bool hc12_msg_rcv = false;
unsigned long tic;
unsigned int counter = 0;
bool button1_pressed = 0;
bool button2_pressed = 0;
bool isr_started = false;
// uint32_t elapsed = millis();
SoftwareSerial HC12(HC12_TX, HC12_RX);

void vSoftwareReset(unsigned long delayMillis) 
{
  uint32_t resetTime = millis() + delayMillis;
  while ( resetTime > millis()) { 
    /* wait and do nothing until the required delay expires... */
  }
  // jump to the start of the program
  asm volatile ( "jmp 0");
}


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


void vPushButton1Pressed()
{
    button1_pressed = 1;
}

void vPushButton2Pressed()
{
    button2_pressed = 1;
}


void concat(const char str1[], const char str2[], char buff[])
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

void vBaudBlink ()
{
    int ind;
    EEPROM.get(0, ind);
    // if(millis()<5000) {
        for (int x = 0; x <= ind; x++){
        digitalWrite(LED2, HIGH);
        delay(250);
        digitalWrite(LED2, LOW);
        delay(250);
        }
    // }
}

void setup() 
{
    pinMode(LED, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(PUSH_BUTTON_1, INPUT_PULLUP);
    pinMode(PUSH_BUTTON_2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON_1), vPushButton1Pressed, RISING);
    attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON_2), vPushButton2Pressed, RISING);
    //Serial.begin(DEFAULT_BAUDRATE);
	//HC12.begin(DEFAULT_BAUDRATE); 
    int incrm = 0;
    int ind;
    EEPROM.get(0, ind);
    Serial.begin(baudRates[ind]);
    HC12.begin(baudRates[ind]);
    vBaudBlink ();
    // for (int x = 0; x < ind; x++){
    //     digitalWrite(LED2, HIGH);
    //     delay(250);
    //     digitalWrite(LED2, LOW);
    //     delay(250);
    // }

    char msg[MAX_MSG_SIZE], rcv_msg[MAX_MSG_SIZE];
    button1_pressed = 0;
    button2_pressed = 0;
    // tic = millis();
	while (1)
	{
        // while (!bPushButtonPressed());
        // EEPROM.get(0,ind);
        // for (int x = 0; x < ind; x++){
        //     digitalWrite(LED, HIGH);
        //     delay(200);
        //     digitalWrite(LED, LOW);
        //     delay(200);
        // }

        if(button2_pressed) {
            button2_pressed = 0;
            EEPROM.get(0,ind);
            incrm = ind;
            if (incrm<NumOf_BAUDRATE-1) {
                incrm++;
                EEPROM.put(0, incrm);

            }
            else if (incrm=>NumOf_BAUDRATE-1) {
                incrm = 0;
                EEPROM.put(0, incrm);
            }
            // Serial.println(F("Pressed"));
            vSoftwareReset(100);
        }

        if(button1_pressed)
        {
            button1_pressed = 0;
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