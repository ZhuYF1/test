#include "led.h"
#include "sys.h"
#include "delay.h"
int main(void)
{
LED_Init();
delay_init(168);
while(1)
{
delay_ms(500);
led0=0;
led1=1;
delay_ms(500);
led0=1;
led1=0;
}
}
