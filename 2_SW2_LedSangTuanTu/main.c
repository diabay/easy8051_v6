#include <reg51.h>
#include <stdio.h>
#include <intrins.h>

sbit congtac= P0^7; /* Khai bao bit chan P3.7*/
unsigned char x;
int j;

void delay()
{
	unsigned long int i;
	for(i=0;i<=5000;++i);
}
void main(void)
{
	
	congtac=0x01; // khai bao cong tac la ngo vao
	while(1)
	{
		x= 0xfe;
		if(congtac==1)
			for(j=1;j<=8;++j)
		{
			P2=x;
			delay();
			x= _crol_(x,1);
		}
		P2=0;
		delay();
		P2=0xff;
		delay();
	}
}