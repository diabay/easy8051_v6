#include <reg52.h>
#include <stdio.h>
#include <math.h>

unsigned char M[10]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
unsigned int donvi,chuc,tram,nghin,num;

//sbit nutnhan=P2^0;

//void delay(void)
//{
//	TMOD=0x02;
//	TH0=TL0=-100;
//	TR0=1;
//	while(!TF0);
//	TF0=0;
//	TR0=0;
//}

void delay(unsigned int time )
{
    unsigned int i;
//   unsigned int j;

  for( i=0; i<time; i++ );
//  for( j=0; j<100; j++ );
}

//void TangSoDem (void) interrupt 1
//{
//	TR0=0;
//	TR0=1;
//}

void main()
{
    IE=0x84;                  
    IT1=1; 
		num=10;
		nghin=num/1000;
		tram=(num%1000)/100;
		chuc=((num%1000)%100)/10;
		donvi=((num%1000)%100)%10;
	
	while(1)
	{

		P0=M[donvi];
		P1=0x01;
		delay(10);
		
		P0=M[chuc];
		P1=0x02;
		delay(10);
		
		P0=M[tram];
		P1=0x04;
		delay(10);
		
		P0=M[nghin];
		P1=0x08;
		delay(10);
		
	}
}
void tangsodem(void) interrupt 2			//Tao ngat chan P3.3
{
		++num;
		nghin=num/1000;
		tram=(num%1000)/100;
		chuc=((num%1000)%100)/10;
		donvi=((num%1000)%100)%10;
}