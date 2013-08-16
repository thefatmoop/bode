/*
R1
8/16/2013
Reibot.org for full guide 
This work is licensed under a Creative Commons Attribution-NonCommercial 3.0 United States License
*/

#include <AD9850.h>//dds sine wave generator

AD9850 ad(7, 8, 9); // w_clk, fq_ud, d7
int led = 13;
int scaleOut = 5;
// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);     
  pinMode(scaleOut, OUTPUT);

  Serial.begin(57600);
  delay(1000);
  Serial.println("Hello!");
}



// the loop routine runs over and over again forever:
void loop() {
  //connect pin 13 to the scope's external trigger, and trigger off it. 
  //chan1 goes to your filter, chan2 connects to pin5. 
  //pin 5 will show the log scale magnitudes for x axis. 
  
  long sweepTime_mS = 250;
  long endFrequency = 1000000;
  int points = 150;//keep the number of points between 20 and 200,
  byte axisWidth = 1; //if your scope doesn't see the magnitude markers on chan2, increase this to something below 5
  long marker = 50000; //highlight a frequency (put 0 if you don't want to use)
  double markerWidth = .1; //how fat that marker is
  
  //end of what you need to edit
  
  double expMultiplier = log(endFrequency)/sweepTime_mS;
  unsigned long timeDelayuSec = (sweepTime_mS*1000)/((double)points);//the 600 is about low long it takes to write to dds
  double i_increment = sweepTime_mS/((double)points);
  double i = 0;
  
  long axisLocation_mS = sweepTime_mS*log(10)/log(endFrequency); //for 100Khz pk, 10,100,1000,10k,100k occur every 50mSec
  
  
  //Serial.print(" micros: ");
  //Serial.println(micros());
  
     //try and do some overflow handling. will sit and twiddle its thumbs for 2 secs before overflow
   while( micros() > 4294964295){ //overflows at 4,294,967,295
      delay(10);
      Serial.println("waiting for overflow");
      Serial.print("micros: ");
      Serial.println( micros() );
   }
  
  ad.setfreq(15);//keep hpf biased
   
  unsigned long previousUptime = micros();
  unsigned long startTime = micros();
  unsigned long timeElapsed = 0;
   
  digitalWrite(scaleOut,HIGH);
  digitalWrite(led, HIGH); 
  
  while( micros() <= startTime + sweepTime_mS*1000.){
  
    long frequency = pow(2.71828,i*expMultiplier);
    
    //keep generating something if it's a very low freq. this keeps hpf biased 
    if(frequency <= 15)    
    ad.setfreq( 15 );
    else
    ad.setfreq( frequency );
    
    
    
//chan2 highlights
    timeElapsed = (micros() - startTime)/1000.;
     
     
 
     for(int ii = 0; ii < 10; ii++){//up to 10 markers on log scale. 10magnitude capability is overkill
     
      if( timeElapsed >= ii*axisLocation_mS && timeElapsed <= ii*axisLocation_mS+axisWidth){
        digitalWrite(scaleOut,LOW);
      }else
        digitalWrite(scaleOut,HIGH); 
     }
     
    
    //used to highlight your expected filter peak or -3dB poinr
    if( frequency > marker - markerWidth*marker && frequency < marker + markerWidth*marker)
      digitalWrite(5,LOW);
    else
      digitalWrite(5,HIGH);
    

//end chan2 highlights
      
    //wait until uptime has elapsed instead of using delay.
    //this somewhat compensates for any time lost mainly due to writing to the dds
    while(micros() <= previousUptime + timeDelayuSec){}

 
    previousUptime = micros();
    
    
    //if(pow(2.71828,i*expMultiplier) > endFrequency){
    //  break;
   // }
    
    i+= i_increment;
    
   
  }
  
  digitalWrite(scaleOut,LOW);
  digitalWrite(led, LOW); 
  ad.setfreq(300000);//stop generating any sines, but keep hpf biased
  //Serial.print("\nBREAK\n");
  delay(sweepTime_mS+100);          
}
