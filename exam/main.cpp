#include "mbed.h"
#include "uLCD_4DGL.h"
uLCD_4DGL uLCD(D1, D0, D2);

DigitalIn button1(PA_6);//button1 used for up D12
InterruptIn ibutton2(PA_7);//button2 used for confirm and reselect D11
DigitalIn button3(PA_2);//button3 used for down D10
void confirm_btn_isr(int index);

AnalogOut Aout(PA_4);//D7
//AnalogOut Freq1(PA_5);//D13
AnalogIn Ain(A0);


EventQueue queue(32 * EVENTS_EVENT_SIZE);
//42,70,98
int Lindex=42;//Used for determining the position of white line
int unit=28; // The unit added/subtracted to Lindex when button is pressed

int sample=100000; //Number of points to be taken
int sam_index=0; //samling index
float ADCdata[100000];// This is to store the Ain values for sampling

Thread thread;

void receiveButton();
void PRODUCE_ANALOG(int index);
void display_ulcd(int index);

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
    uLCD.printf("100 Hz");
    uLCD.locate(5,7);
    uLCD.printf("135 Hz");
    uLCD.locate(5,10);
    uLCD.printf("175 Hz");
    uLCD.locate(5,13);
    uLCD.printf("200 Hz");
    
    uLCD.line(30, Lindex, 80, Lindex, WHITE);
    receiveButton();

}

/*
Moveline(int,int) is used to move the selection line up/down 
when the user presses the 1st(left) or 3rd(right) button
*/
void moveLine(int Nindex,int Pindex)
{
    uLCD.line(30,Pindex,80,Pindex,BLACK);//This is to make the previous texts to appear black
                                        //The purpose is to make the previous texts invisible to the user
    uLCD.line(30,Nindex,80,Nindex,WHITE);//This is to make the current texts to appear white and allow user to see
}


/*
receiveButton() is used to determine
which button was pressed by the user.
Suppose button1 is pressed,
we will check the index and call moveLine(int,int)
and move the selection line to the correct position.
Similar action is taken when button3 is pressed.
WHen button2 is pressed, it will pass the last location
of the selection line as argument to freq(int).
Finally, freq(int) will produce the output and do the sample
of the freq selected

*/
void receiveButton()
{
    while(1){
    int Pindex;
        if(button1.read()==1){
            //ThisThread :: sleep_for(200ms); // Without this, the selection will
                                            //move very fast when we push the button
            Pindex=Lindex;
            Lindex= Lindex - unit;
            if(Lindex<42){
                Lindex = 126;
            } 
            moveLine(Lindex,Pindex);
            //printf("button1 is pressed!\r\n");
            
        }
        
                    //printf("button2 is pressed!\r\n");
             thread.start(callback(&queue, &EventQueue::dispatch_forever));
             ibutton2.rise(queue.event(confirm_btn_isr,Lindex));
             ibutton2.rise(queue.event(PRODUCE_ANALOG,Lindex));
             ibutton2.rise(queue.event(display_ulcd,Lindex));          
        
        if(button3.read()==1){
            //ThisThread :: sleep_for(200ms);
            Pindex=Lindex;
            Lindex=Lindex+unit;
            if(Lindex>126) Lindex =42;
            moveLine(Lindex,Pindex);
           // printf("button3 is pressed!\r\n");
            

        }
    }
}


/*
The main function only needs to call Display()
*/
void display_ulcd(int index)
{
    int frin=0; //This frin is to store the value of sampling frequecny
                //IN FOR LOOP for producing the wave output, 100 iterations were made.
                //This means that frin = 100* (selected freq)
                //EG. if USER selects 100Hz, then frin = 100*100=10000
    float i;
    int selection=84;//The frequency of wave can be determined from wait_us time
                // Thus, int selection will be the input os wait_us
    uLCD.cls();
    uLCD.color(RED);
    if(index==42){//From the selection line, we can know the frequency that the user chooses
        uLCD.printf("100Hz is selected");
        selection=84;//This is the corresponding frequency to the position of kthe white line
        frin=10000;
    }
    else if(index==70){
        uLCD.printf("135Hz is selected");
        selection=38;
        frin=13500;
    }
    else if(index==98){
        uLCD.printf("175Hz is selected");
        selection=22;
        frin=17500;
    }
    else{
        uLCD.printf("200Hz is selected");
        selection=13;
        frin=20000;
    }
}
void PRODUCE_ANALOG(int index)
{
    int frin=0; //This frin is to store the value of sampling frequecny
                //IN FOR LOOP for producing the wave output, 100 iterations were made.
                //This means that frin = 100* (selected freq)
                //EG. if USER selects 100Hz, then frin = 100*100=10000
    float i;
    int selection=84;

    if(index==42){//From the selection line, we can know the frequency that the user chooses
        //uLCD.printf("100Hz is selected");
        selection=84;//This is the corresponding frequency to the position of kthe white line
        frin=10000;
    }
    else if(index==70){
        //uLCD.printf("135Hz is selected");
        selection=38;
        frin=13500;
    }
    else if(index==98){
        //uLCD.printf("175Hz is selected");
        selection=22;
        frin=17500;
    }
    else{
        //uLCD.printf("200Hz is selected");
        selection=13;
        frin=20000;
    }
    while(1) // The analog output is produced as long as the sample index is not maximu
    {   
        for(i = 0; i <= 0.9; i=i+0.01){
            Aout = i;
            ADCdata[sam_index]=Ain;
            wait_us(selection);
            if(button3.read()) Display();
            sam_index++;
        }
        
        if(button3.read()) Display();

        for(i = 0.9; i >= 0; i=i-0.1){
            Aout = i;
            ADCdata[sam_index]=Ain;
            wait_us(selection);
            if(button3.read()) Display();
            sam_index++;
        }  
    }
}

void confirm_btn_isr(int index)
{
    printf("index: %d\r\n",index);
}

int main()
{

    Display();
}