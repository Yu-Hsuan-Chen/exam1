#include "mbed.h"
#include "uLCD_4DGL.h"

#include <cstdio>

AnalogIn Up(A0);
AnalogIn Down(A1);
AnalogIn Select(A2);

// InterruptIn Up_button(A0);
// InterruptIn Down_button(A1);
// InterruptIn Select_button(A2);


AnalogOut Aout(D7);
AnalogIn Ain(A3);
uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;

DigitalIn Sample(USER_BUTTON);


void LCD();
void generate_wave(int y);

float ADCdata[20000];
             
Timer t;

int main()
{
    printf("START! \n\r");  
    
    // t.start();
    // int idx = 0;
    // for (float i = 0.0f; i <= 0.9f; i += 0.01) {
    //     Aout = i;
    //     ADCdata[idx++] = Ain;
    // }
    // t.stop();
    // auto us = t.elapsed_time().count();
    // printf ("Timer time: %llu us\n", us);
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
    int slew = 0;
    int waitting = 0;
    switch (y)
    {
    case 93:
    //10hz
        slew = 80000;
        waitting = 80000; 
        break;
    //100
    case 78:
        slew = 40000;
        waitting = 160000;        
        break;
    //500
    case 63:
        slew = 20000;
        waitting = 200000;        
        break;
    case 48:
        slew = 10000;
        waitting = 220000;        
        break;
    default:
        break;
    }
    float tmp = (0.9f/slew)*24;

    printf("Start generate wave form!! \n\r");
    ThisThread::sleep_for(1s);
    
    while (Down.read() < 0.9f && Up.read() < 0.9f && Select.read() < 0.9f && Sample == 1) {
        int idx = 0;
        for (float i = 0.0f; i <= 0.9f; i += tmp) {
            Aout = i;
            ADCdata[idx++] = Ain;
        }
        for (int i = 0; i<= (waitting/25); i++) {
            Aout = 0.9f;
            ADCdata[idx++] = Ain;
        }
        for (float i = 0.9f; i >= 0.0f; i -= tmp) {
            Aout = i;
            ADCdata[idx++] = Ain;
        }
    }  


    // printf("Stop generate wave form! \n\r");
    // if(Sample == 0) {
    //     printf("Start catching data\n\r");
    //     ThisThread::sleep_for(200ms);
    //     for(int i=0; i<samples; i++) {
    //         printf("%f\n\r", ADCdata[i]);
    //         ThisThread::sleep_for(200ms);
    //     }
    // }

}
     
