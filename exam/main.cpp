#include "mbed.h"
#include "uLCD_4DGL.h"
uLCD_4DGL uLCD(D1, D0, D2);
InterruptIn button1(PA_6);//button1 used for up D12
InterruptIn button2(PA_7);//button2 used for confirm and reselect D11
InterruptIn button3(PA_2);//button3 used for down D10
InterruptIn USER(USER_BUTTON);
AnalogOut Aout(PA_4);//D7
//AnalogOut Freq1(PA_5);//D13
AnalogIn Ain(A0);
EventQueue queue(32 * EVENTS_EVENT_SIZE);
EventQueue queue1(32 * EVENTS_EVENT_SIZE);
//EventQueue eventQueue;
Timeout tim;

Thread thread;
Thread thread1;

int Pindex;
//42,70,98,124
int Lindex=42;//Used for determining the position of white line
int unit=28; // The unit added/subtracted to Lindex when button is pressed
int sample=100000; //Number of points to be taken
int sam_index=0; //samling index
float ADCdata[100000];// This is to store the Ain values for sampling



//Display() is used to display the INSTRUCTIONS in uLCD
void Display()
{
    sam_index=0;//This is to restore the sam_index when the person presses the user reselect
                //Without restoring the index, only the printing of ADCdata will occur
    uLCD.cls();//cls is to clear all the texts with the background color
    uLCD.textbackground_color(BLACK);
    uLCD.color(RED); //RED as text color

    uLCD.printf("\nPlease select the frequency:\n"); //Default Green on black text
    uLCD.locate(5,4);//position of the text
    uLCD.printf("1");
    uLCD.locate(5,7);
    uLCD.printf("1/2 Hz");
    uLCD.locate(5,10);
    uLCD.printf("1/4 Hz");
    uLCD.locate(5,13);
    uLCD.printf("1/8 Hz");
    
    uLCD.line(30, Lindex, 80, Lindex, WHITE);
    

}

void SAMPLE()
{
    for(;sam_index<sample;sam_index++){
        ADCdata[sam_index]=Ain;

    }
    for(int k=0;k<sam_index;k++){
        printf("%f\r\n", ADCdata[k]);
    }
}
/*
void SAMPLE()
{   
    for(int i=0;i<25;i++){
        
        printf("BUTTON2 IS PRESSED \r\n");
        wait_us(10);
    }
}*/


void freq(int index)
{
   // int waiting1=0,waiting2=0;
    
    float i=0;
    if(index==42){
        
        while(1)
        {
            for( i=0;i<=0.9;i=i+0.018){
                Aout = i;
                wait_us(1550);

            }
            
            ThisThread::sleep_for(80ms);
            for( i=0.9; i>=0;i=i-0.018){
                Aout=i;
                wait_us(1550);
            }
             
    
        }
    } 
    else if(index == 70){

        while(1)
        {
            for( i=0;i<=0.9;i=i+0.018){
                Aout = i;
                wait_us(730);

            }
            ThisThread::sleep_for(160ms);

            for( i=0.9; i>=0;i=i-0.018){
                Aout=i;
                wait_us(730);
            }
        }

    }

    else if(index == 98){

        while(1)
        {
            for( i=0;i<=0.9;i=i+0.018){
                Aout = i;
                wait_us(360);

            }
            ThisThread::sleep_for(200ms);

            for( i=0.9; i>=0;i=i-0.018){
                Aout=i;
                wait_us(360);
            }
        }

    }

    else{

        while(1)
        {
            for( i=0;i<=0.9;i=i+0.018){
                Aout = i;
                wait_us(130);

            }
            ThisThread::sleep_for(220ms);

            for( i=0.9; i>=0;i=i-0.018){
                Aout=i;
                wait_us(130);
            }
        }

    }
    

}

void moveLine(int Nindex,int Pindex)
{
    uLCD.line(30,Pindex,80,Pindex,BLACK);//This is to make the previous texts to appear black
                                        //The purpose is to make the previous texts invisible to the user
    uLCD.line(30,Nindex,80,Nindex,WHITE);//This is to make the current texts to appear white and allow user to see
}

void button1_push()
{
   
    //ThisThread :: sleep_for(200ms); // Without this, the selection will
                                    //move very fast when we push the button
    Pindex=Lindex;
    Lindex= Lindex - unit;
    if(Lindex<42){
        Lindex = 126;
    } 
    moveLine(Lindex,Pindex);
    //printf("button1 is pressed!\r\n");
   // printf("Lindex: %d\r\n",Lindex);   
}

void button2_push()
{

    //ThisThread :: sleep_for(200ms);
    //printf("button2 is pressed Lindex: %d!\r\n",Lindex);
    freq(Lindex);
}

void button3_push()
{
    //ThisThread :: sleep_for(200ms);
    Pindex=Lindex;
    Lindex=Lindex+unit;
    if(Lindex>126) Lindex =42;
    moveLine(Lindex,Pindex);
    // printf("button3 is pressed!\r\n");
    //printf("button3 Lindex: %d \r\n",Lindex);
} 


/*
The main function only needs to call Display()
*/


int main()
{
    
    Display();
    thread.start(callback(&queue, &EventQueue::dispatch_forever));
    button1.rise(queue.event(button1_push));
    button2.rise(queue.event(button2_push));
    button3.rise(queue.event(button3_push));
    USER.rise(queue.event(SAMPLE));  
            

}