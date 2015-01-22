#include <pitches.h>
#include <StopWatch.h> //http://playground.arduino.cc/Code/StopWatchClass
const int melodyPin = 3;
int doorSwitches[] = {11,12};
int doorCount = 0;
const int light = 13;

const int timeout = 2;
int timeoutCount = 0;
StopWatch time;

int melody[] = { NOTE_F6, NOTE_D6,0};
int tempo[] = {1,32,1};

void alarm()
{      
     int size = sizeof(melody) / sizeof(int);
     for (int thisNote = 0; thisNote < size; thisNote++) {
       if(IsDoorClosed())  return;

       // to calculate the note duration, take one second
       // divided by the note type.
       //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
       int noteDuration = 1000/tempo[thisNote];

       buzz(melodyPin, melody[thisNote],noteDuration);

       // to distinguish the notes, set a minimum time between them.
       // the note's duration + 30% seems to work well:
       int pauseBetweenNotes = noteDuration * 1.30;
       delay(pauseBetweenNotes);

       // stop the tone playing:
       buzz(melodyPin, 0,noteDuration);
  }
}

void buzz(int targetPin, long frequency, long length) {
  if(IsDoorClosed())  return;

  digitalWrite(light,HIGH);
  long delayValue = 1000000/frequency/2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length/ 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to 
  //// get the total number of cycles to produce
  for (long i=0; i < numCycles; i++){ // for the calculated length of time...
    digitalWrite(targetPin,HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin,LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  }
  digitalWrite(light,LOW);

}



void setup() 
{      
  Serial.begin(9600);
  
  pinMode(light, OUTPUT);  

  doorCount = sizeof(doorSwitches) / sizeof( int );
  for(int i = 0; i < doorCount; i++)
  {
    pinMode(doorSwitches[i], INPUT_PULLUP);
  }
  
  pinMode(melodyPin, OUTPUT);
  
  time = StopWatch(StopWatch::SECONDS);
  Serial.println("ready");
}

bool isDoorOpen()
{
  for(int i = 0; i < doorCount; i++)
  {
    if (digitalRead(doorSwitches[i]) == HIGH)
      return true;
  }
    
  return false;
}

bool IsDoorClosed()
{
  return !isDoorOpen();
}

bool shouldLight()
{
  return isDoorOpen();
}

void lightAlert(bool on)
{
  
  if(on)
  {
      Serial.println("light alert");
      digitalWrite(light, HIGH);
      time.start();
  }
  else
  {
    digitalWrite(light, LOW);
    time.reset(); 
  }  
}

bool shouldAlarm()
{ 
  Serial.println("should alarm");
  Serial.println(time.elapsed());
  return time.elapsed() >= timeout;
}

void loop() 
{     
  if(shouldLight())
  {
    Serial.println("should light");
    lightAlert(true);
    if (shouldAlarm())
        alarm();
  }
  else
    lightAlert(false); 
}



