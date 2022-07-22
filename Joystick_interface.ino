
#define MOVE_STATE_NONE 0
#define MOVE_STATE_UP 1
#define MOVE_STATE_DOWN 2
#define MOVE_STATE_LEFT 3
#define MOVE_STATE_RIGHT 4
#define MOVE_STATE_UL 5
#define MOVE_STATE_DL 6
#define MOVE_STATE_DR 7
#define MOVE_STATE_UR 8


int VRx = A0;
int VRy = A1;
int SW = 2;

int xPosition = 0;
int yPosition = 0;
int SW_state = 0;

int MOVE_STATE = 0;

void fun_print(int x, int y, int z);
void fun_logic(int xPosition, int yPosition, int SW_state );

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT); 
  
}

void loop() {
   // put your main code here, to run repeatedly:
  xPosition = analogRead(VRx);
  yPosition = analogRead(VRy);
  SW_state = digitalRead(SW);

//  Serial.println("Joystick is at center ");
  
  fun_logic(xPosition, yPosition, SW_state);

//  delay(1000);
  
}

void fun_print(int x, int y, int z)
{
  
  Serial.print("X: ");
  Serial.println(x);
  Serial.print("Y: ");
  Serial.println(y);
  Serial.print("SW state: ");
  Serial.println(z);
  Serial.println(); 
}

void fun_logic(int xPosition, int yPosition, int SW_state)
{

  //Default-center
  if((500 <= xPosition) && (xPosition <= 520) && (500 <= yPosition) && (yPosition <= 520))
  {
    if (MOVE_STATE != MOVE_STATE_NONE)
    {
      MOVE_STATE = MOVE_STATE_NONE;
      Serial.println("Joystick is at center ");

      fun_print(xPosition, yPosition, SW_state);


    }      
  }

  //Up  
  else if((500 <= xPosition) && (xPosition <= 520)&& (1000 <= yPosition) && (yPosition <= 1024))
  {
    if (MOVE_STATE != MOVE_STATE_UP)
    {
      MOVE_STATE = MOVE_STATE_UP;
      Serial.println("Move Forward ");

      fun_print(xPosition, yPosition, SW_state);

      
    }      
  }

  //Down
  else if((500 <= xPosition) && (xPosition <= 520)&& (0 <= yPosition) && (yPosition <= 20))
  {
    if (MOVE_STATE != MOVE_STATE_DOWN)
    {
      MOVE_STATE = MOVE_STATE_DOWN;
      Serial.println("Move Backward ");

      fun_print(xPosition, yPosition, SW_state);      

    }      
  }

  //Left
  else if((500 <= yPosition) && (yPosition <= 520)&& (1000 <= xPosition) && (xPosition <= 1024))
  {
    if (MOVE_STATE != MOVE_STATE_LEFT)
    {
      MOVE_STATE = MOVE_STATE_LEFT;
      Serial.println("Move in Left Direction ");

      fun_print(xPosition, yPosition, SW_state); 

    }      
  }

  //Right
  else if((500 <= yPosition) && (yPosition <= 520) && (0 <= xPosition) && (xPosition <= 20))
  {
    if (MOVE_STATE != MOVE_STATE_RIGHT)
    {
      MOVE_STATE = MOVE_STATE_RIGHT;
      Serial.println("Moving in Right direction ");

      fun_print(xPosition, yPosition, SW_state); 

    }      
  }

  //MOVE_STATE_UL, up-left dir
  else if((520 < yPosition) && (yPosition < 1000)&& (1000 > xPosition) && (xPosition > 520))
  {
    if (MOVE_STATE != MOVE_STATE_UL)
    {
      MOVE_STATE = MOVE_STATE_UL;
      Serial.println("Move in Up-Left Direction ");

      fun_print(xPosition, yPosition, SW_state); 

    }      
  }

  //MOVE_STATE_DL, down-left dir
  else if((20 < yPosition) && (yPosition < 500)&& (1000 > xPosition) && (xPosition > 520))
  {
    if (MOVE_STATE != MOVE_STATE_DL)
    {
      MOVE_STATE = MOVE_STATE_DL;
      Serial.println("Move in Down-Left Direction ");

      fun_print(xPosition, yPosition, SW_state); 

    }      
  }
  
  //MOVE_STATE_DR, down-left dir
  else if((20 < yPosition) && (yPosition < 500)&& (500 > xPosition) && (xPosition > 20))
  {
    if (MOVE_STATE != MOVE_STATE_DR)
    {
      MOVE_STATE = MOVE_STATE_DR;
      Serial.println("Move in Down-Right Direction ");

      fun_print(xPosition, yPosition, SW_state); 

    }      
  }
  
  //MOVE_STATE_UR, down-left dir
  else if((520 < yPosition) && (yPosition < 1000)&& (500 > xPosition) && (xPosition > 20))
  {
    if (MOVE_STATE != MOVE_STATE_UR)
    {
      MOVE_STATE = MOVE_STATE_UR;
      Serial.println("Move in Up-Right Direction ");

      fun_print(xPosition, yPosition, SW_state); 

    }      
  }
  
  else
  {
//    Serial.println("Moving inbetween");
//    fun_print(xPosition, yPosition, SW_state); 
  }

}
