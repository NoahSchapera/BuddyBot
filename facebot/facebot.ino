//By Eden, for Noel
#include <LedControl.h>
#include <HCSR04.h>
#include <Servo.h>

// pin variables
int DIN = 10;
int CS = 11;
int CLK  = 12;


// library variables
LedControl lc=LedControl(DIN, CLK, CS,0);
UltraSonicDistanceSensor distanceSensor(3, 2);
Servo neck;

// define class
class Bot 
{
  public:  
  // time vals, millis
  //overflow in approx 1 month
  long unsigned search_init;
  long unsigned search_elapsed; 
  // duration between search intervals, millis
  // 2 mins
  long unsigned search_wait = 120000;

  // emote wait times
  long unsigned emote_init;
  long unsigned emote_elapsed;
  long unsigned emote_wait = 20000;

  // smile bitmap
  int smile[8] = 
  {
    B00000000,
    B00100100,
    B00100100,
    B00100100,
    B00000000,
    B01000010,
    B00111100,
    B00000000  
  };
  // blink bitmap
  int smile_blink[8] = 
  {
    B00000000,
    B00000000,
    B01100110,
    B00000000,
    B00000000,
    B01000010,
    B00111100,
    B00000000  
  };

  // wink bitmap
  int smile_wink[8] = 
  {
    B00000000,
    B00000100,
    B01100100,
    B00000100,
    B00000000,
    B01000010,
    B00111100,
    B00000000  
  };

  // question bitmap
  int question[8] = 
  {
    0b00000000,
    0b00111100,
    0b01100110,
    0b00000110,
    0b00011100,
    0b00011000,
    0b00000000,
    0b00011000 
  };

  // heart bitmap anim
  int heart_anim[4][8] = 
  {
    { 
      0b00000000,
      0b00000000,
      0b00000000,
      0b00011000,
      0b00011000,
      0b00000000,
      0b00000000,
      0b00000000
    }, // frame 1

    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00111100,
      0b00111100,
      0b00011000,
      0b00000000,
      0b00000000
    }, // frame 2
            
    {
      0b00000000,
      0b00000000,
      0b01100110,
      0b01111110,
      0b01111110,
      0b00111100,
      0b00011000,
      0b00000000
    }, // frame 3

    {
      0b00000000,
      0b01100110,
      0b11111111,
      0b11111111,
      0b11111111,
      0b01111110,
      0b00111100,
      0b00011000
    } // frame 4
  };

  // sleep bitmap anim
  int sleep_anim[3][8] = 
  {
    {
      0b11111111,
      0b10000110,
      0b00001100,
      0b00011000,
      0b00110000,
      0b01100001,
      0b11000011,
      0b11111111
    },
    {
      0b00000000,
      0b01111110,
      0b01000100,
      0b00001000,
      0b00010000,
      0b00100010,
      0b01111110,
      0b00000000
    },
    {
      0b00000000,
      0b00000000,
      0b00111100,
      0b00001000,
      0b00010000,
      0b00111100,
      0b00000000,
      0b00000000
    }
  };

  
 
  //display a bitmap
  //default, lc as display
  void disp(int pic[], LedControl l);

  void dispAnim(int pic_arr[]);

  void search(Servo s, UltraSonicDistanceSensor ud);

};

void Bot::disp(int pic[], LedControl l = lc)
{
  for(int i = 0; i < 8; i++)
  {
    l.setRow(0,i,pic[i]);
  }
}

void Bot::dispAnim(int pic_arr[])
{
  int rows = sizeof(pic_arr) / sizeof(pic_arr[0]); // 2 rows

  for(int i = 0; i < rows; i++)
  {
    disp(pic_arr[i]);
  }  
}


//search function for bot
//default, ultrasonic distance sensor, neck servo
// search range
void Bot::search(Servo s = neck, UltraSonicDistanceSensor d = distanceSensor)
{
  disp(question);

  int dists[181];
  int min_val=INT8_MAX;
  int min_index;

  s.write(0);
  delay(10);
  for (int pos = 0; pos <= 180; pos += 1) 
  { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    s.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);
    // waits 15ms for the servo to reach the position
    dists[pos] = d.measureDistanceCm();
    Serial.println(dists[pos]);
    //Serial.println(dists[pos]);
    delay(100); //wait for measurement   
  }
  for (int i=1; i<180; i++)
  {
    if (dists[i] < min_val)
    {
        min_index = i;
        min_val = dists[i];
    }
  }
  s.write(min_index);

  Serial.println(min_index);
  disp(smile);

}

// initialize bot
Bot bot;


// runtime variables
// can we get rid of this??
int pos = 90;
int rval = 0;



void  setup() 
{
  // begin serial protocol


  Serial.begin(9600);

  // configure LED
  lc.shutdown(0,false);
  lc.setIntensity(0,0);
  lc.clearDisplay(0);
  
  // configure SERVO
  neck.attach(13);

  // display smile, search for user
  bot.disp(bot.smile);
  bot.search();
}

void loop()
{

  // check for search delay elapsed
  if(millis() - bot.search_init > bot.search_wait)
  {
    bot.search_init = millis();
    bot.search();
  }


  // check for emote delay elapsed
  if(millis() - bot.emote_init > bot.emote_wait)
  {
    //reset timer

    
    // get a random number
    rval = random(0,10);
  


    // if < 8,
    // blink
    if(rval < 8)
    {
      bot.disp(bot.smile_blink);
    }


    // if > 8,
    // wink
    else
    {
      bot.disp(bot.smile_wink);
    }

    // delay for emote, then return t smile
    delay(200);
    bot.disp(bot.smile);

    //emote wait =randome(2500, 10000)
    bot.emote_wait = random(2500, 10000);
    // reset timer
    bot.emote_init = millis();
  }
  
 }