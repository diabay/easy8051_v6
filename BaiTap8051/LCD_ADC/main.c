
#include <REG8253.H>
#include <stdio.h>
//#include <math.h>
#include <intrins.h>
#include <string.h>
#define Pdata P2

#define RS 0x01
#define EN 0x02

//sbit DQ=P1^2;
	sbit CS=P3^5;

//sbit RS= P2^0;
//sbit EN= P2^1;

char x;

void delay( unsigned int t)
{
	unsigned int i;
	for(i=0;i<t;++i);
}

void delay_timer0(unsigned int i)  //delay 1.2us
{
	unsigned int load_value;
	load_value=65536 - i;
	TH0=load_value >> 8;TL0=load_value & 0x00FF;
	TR0=1;
	while(TF0==0);
	TF0=0;
	TR0=0;
}

///////[start] hien thi lcd//////////////

void write_command(char LCDcmd) // ghi lenh ra LCD
{
	Pdata=((LCDcmd & 0xF0) >>2)|EN;
	delay_timer0(1);
	Pdata=((LCDcmd & 0xF0) >>2);
	delay_timer0(2);
	Pdata=((LCDcmd & 0x0F) <<2)|EN;
	delay_timer0(1);
	Pdata=((LCDcmd & 0x0F) <<2);
	delay_timer0(2);
}

void write_data(char LCDdt) //ghi du lieu can hien thi ra LCD
{
	Pdata =((LCDdt & 0xF0)>>2)|RS|EN;
	delay_timer0(1);
	Pdata =((LCDdt & 0xF0)>>2)|RS;
	delay_timer0(2);
	Pdata =((LCDdt & 0x0F)<<2)|RS|EN;
	delay_timer0(1);
	Pdata =((LCDdt & 0x0F)<<2)|RS;
	delay_timer0(2);
}

void write_string( unsigned char *s)
{
	while(*s)//ghi mot chuoi ky tu ra LCD
	{
		write_data(*s);
		s++;
	}
}

void lcd_reset()
{
        Pdata = 0xFF;
        delay_timer0(14200);
        Pdata = 0x0C+EN;
        Pdata = 0x0C;
        delay_timer0(8400);
        Pdata = 0x0C+EN;
        Pdata = 0x0C;
        delay_timer0(833);
        Pdata = 0x0C+EN;
        Pdata = 0x0C;
        delay_timer0(833);
        Pdata = 0x08+EN;
        Pdata = 0x08;
        delay_timer0(833);
}

void init(void)
{
	lcd_reset();         // Call LCD reset
	write_command(0x28); /* 4-bit mode - 2 line - 5x7 font. */
	delay_timer0(40);
	write_command(0x0C); /* Display no cursor - no blink. */
	delay_timer0(40);
	write_command(0x06); /* Automatic Increment - No Display shift */
	delay_timer0(40);
	write_command(0x01);	/* Clear display */
	delay_timer0(1500);
	write_command(0x80);	/* Address DDRAM with 0 offset 80h */
	delay_timer0(1500);
}

unsigned int convert(unsigned int c1,unsigned int c2)
{
	unsigned int c3;
//	float c5;
	c3= (c1 << 8)| c2;
	// c5=(float)((c2*5000)/4096); // VALUE GOIN OUT OF RANGE
//	c5= (float)(4.096*c3);
//	c3=(float)(c5/4096);
	return(c3);
}

void main()
{
		unsigned int vol_1,vol_2;
		unsigned int volt;

	/*timer0, dinh thoi che do 1*/
		TMOD=0x01; 
		TH0=0x00;
		TL0=0x00;
	
	// Khoi tao truyen du lieu SPI
		SPCR=0x55;
		CS=1;
//		SCK=0;
//		MISO=1;

		init();
		write_string("DiaBay.vn");
		write_command(0xc0);
		delay(10);
		write_string("Hello World!");

	while(1)
	{
			CS=0;
			delay_timer0(1);
			SPDR=0x06;
			while((SPSR & 0x80)==0);
			SPDR=0x00;
			while((SPSR & 0x80)==0);
			vol_1=SPDR;
			SPDR=0x00;
			while((SPSR & 0x80)==0);
			vol_2=SPDR;
			delay_timer0(1);
			CS=1;
			vol_1 &= 0x0f;
			volt = convert(vol_1,vol_2);
			
			x=volt/1000 +48; 
			write_command(0x8A);
			write_data(x);
			volt %=1000;
			x=volt/100 +48;
			write_data(x);
			volt %=100;
			x=volt/100 +48;
			write_data(x);
			volt %=10;
			x=volt +48;
			write_data(x);
			
			write_command(0x8F);
			write_data('V');
			
			for (x=0;x<50;x++)
			{
				delay_timer0(8333);
			}
	}
}

	