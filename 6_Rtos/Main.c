#include <reg51.h>
#include <RTX51TNY.h>
#define on 0
#define off 1

sbit led1=P3^4;
sbit led2=P3^5;
sbit led3=P3^6;
sbit led4=P3^7;
void delay(unsigned int n)
{
	int i;
	for(i=0;i<n;i++)
	{
		TMOD=0x01;
		TH0=15535/256;
		TL0=15535%256;
		TR0=1;
		while(!TF0);
		TF0=TR0=0;
	}
}
void nhay1() _task_ 1
{
	while(1)
	{
		led1=on;delay(30);
		led1=off;delay(30);
	}
}
void nhay2() _task_ 2
{
	while(1)
	{
		led2=on;delay(40);
		led2=off;delay(40);
	}
}
void nhay3() _task_ 3
{
	while(1)
	{
		led3=on;delay(50);
		led3=off;delay(50);
	}
}
void nhay4() _task_ 4
{
	while(1)
	{
		led4=on;delay(60);
		led4=off;delay(60);
	}
}
void nhay() _task_ 0
{
	os_create_task (1);
	os_create_task (2);
	os_create_task (3);
	os_create_task (4);
	os_create_task (0);
}