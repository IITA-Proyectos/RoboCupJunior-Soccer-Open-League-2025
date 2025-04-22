#include <Arduino.h>
#include <zirconLib.h>


void setup(void)
{

  InitializeZircon();

}


void loop(void)
{      
  motor1(100, 1);
  motor2(100, 1);
  motor3(100, 1);
  delay(47);
  motor1(0, 1);
  motor2(0, 1);
  motor3(0, 1);
  delay(9000);
}
