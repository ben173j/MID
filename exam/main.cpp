#include "mbed.h"
#include "uLCD_4DGL.h"
uLCD_4DGL uLCD(D1, D0, D2);
InterruptIn button1(PA_6);//button1 used for up D12
InterruptIn button2(PA_7);//button2 used for confirm and reselect D11
InterruptIn button3(PA_2);//button3 used for down D10
AnalogOut Aout(PA_4);//D7
//AnalogOut Freq1(PA_5);//D13
AnalogIn Ain(A0);
EventQueue queue(32 * EVENTS_EVENT_SIZE);

Thread thread;
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

void freq(int index)
{
    for(int i=0;i<index;i++)
    {
        Aout = i;
        wait_us(40);
    }
    wait_us(40);
    for(int i=index; i>=0;i--)
    {
        Aout=i;
        wait_us(40);
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
    printf("Lindex: %d\r\n",Lindex);   
}

void button2_push()
{

    //ThisThread :: sleep_for(200ms);
    printf("button2 is pressed Lindex: %d!\r\n",Lindex);
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
    printf("button3 Lindex: %d \r\n",Lindex);
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
    
}