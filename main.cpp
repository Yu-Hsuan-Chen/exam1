#include "mbed.h"
#include "uLCD_4DGL.h"

#include <cstdio>
using namespace std::chrono;

AnalogIn Up(A0);
AnalogIn Down(A1);
AnalogIn Select(A2);

// InterruptIn Up_button(A0);
// InterruptIn Down_button(A1);
// InterruptIn Select_button(A2);


AnalogOut Aout(D7);
AnalogIn Ain(A3);
uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;


Timer k;

void LCD();
void generate_wave(int y);
void sample();

float ADCdata[20000];
int slew;
int waitting;
int num;           
Thread t;
EventQueue queue(32 * EVENTS_EVENT_SIZE);

int main()
{
    printf("START! \n\r");  
    
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    LCD();

}

// void ISR1() {

// }


void LCD() {
    uLCD.media_init(); // initialize uSD card
    uLCD.locate(0, 0);
    uLCD.text_width(2); //4X size text
    uLCD.text_height(2);
    uLCD.color(RED);
    uLCD.printf("\nSlew Rate\n");

    uLCD.color(WHITE);
    uLCD.printf("  1/8 \n");
    uLCD.printf("  1/4 \n");
    uLCD.printf("  1/2 \n");
    uLCD.printf("  1 \n");
    int x1 = 20; int y1 = 48;
    uLCD.rectangle(x1, y1, x1+65, y1+15, RED);

    while(1) {
        if(Up.read() > 0.9f) {
            printf("Up botton PRESSED \n\r");
            uLCD.rectangle(x1, y1, x1+65, y1+15, BLACK);
            if(y1 < 93) {
                y1 += 15;
                uLCD.rectangle(x1, y1, x1+65, y1+15, RED);                
            }else{
                y1 = 93;
                uLCD.rectangle(x1, y1, x1+65, y1+15, RED);                 
            }

        }
            
        if(Down.read() > 0.9f) {
            printf("Down botton PRESSED \n\r");
            uLCD.rectangle(x1, y1, x1+65, y1+15, BLACK);
            if(y1 > 48) {
                y1 -= 15;
                uLCD.rectangle(x1, y1, x1+65, y1+15, RED);
            }else {
                y1 = 48;
                uLCD.rectangle(x1, y1, x1+65, y1+15, RED);
            }

        }
        //48 63 78 93 
        if(Select.read() > 0.9f) {
            printf("Choose botton PRESSED \n\r"); 
            generate_wave(y1);
        }
           
        ThisThread::sleep_for(250ms);
    }

}

void generate_wave(int y) {

    switch (y)
    {
    case 93:
    //10hz
        slew = 80000;
        waitting = 80000; 
        num = 452;
        break;
    //100
    case 78:
        slew = 40000;
        waitting = 160000;  
        num = 452;
        break;
    //500
    case 63:
        slew = 20000;
        waitting = 200000; 
        num = 453;     
        break;
    case 48:
        slew = 10000;
        waitting = 220000;    
        num = 453;   
        break;
    default:
        break;
    }
    float tmp = (0.9f/slew)*530;

    printf("Start generate wave form!! \n\r");
    printf("%d\n\r", num);
    ThisThread::sleep_for(1s);
    int idx = 0;
    while (Down.read() < 0.9f && Up.read() < 0.9f && Select.read() < 0.9f) {
        idx = 0;
        // k.start();
        for (float i = 0.0f; i <= 0.9f; i += tmp) {
            Aout = i;
            ADCdata[idx++] = Ain;
            wait_us(500);
        }
        
        
        for (int i = 0; i<= (waitting/532); i++) {
            Aout = 0.9f;
            ADCdata[idx++] = Ain;
            wait_us(500);
        }
        
        for (float i = 0.9f; i >= 0.0f; i -= tmp) {
            Aout = i;
            ADCdata[idx++] = Ain;
            wait_us(500);
        }
        // k.stop();
        // auto us = k.elapsed_time().count();
        // printf("Timer time: %lluus\n", us);
        // printf("%d\n", idx);
        // k.reset();
    }  
    printf("Stop generate wave form! \n\r");
    queue.call(sample);

}
     

void sample() {

    printf("Start catching data\n\r");
    ThisThread::sleep_for(200ms);
    for(int i=0; i<num; i++) {
        printf("%f\n\r", ADCdata[i]);
        ThisThread::sleep_for(200ms);
    }

}