
#include <REG8253.H>
#include <stdio.h>
//#include <math.h>
#include <intrins.h>
#include <string.h>
#define Pdata P2

#define RS 0x01
#define EN 0x02

// MCP23S17 Registers Definition for BANK=0 (default)
#define IODIRA 0x00
#define IODIRB 0x01
#define IOCONA 0x0A
#define GPPUA  0x0C
#define GPPUB  0x0D
#define GPIOA  0x12
#define GPIOB  0x13

//sbit DQ=P1^2;
	sbit CS=P3^4;
	sbit Reset=P3^7;

//sbit RS= P2^0;
//sbit EN= P2^1;


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

void init_LCD(void)
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

void SPI_Write (unsigned char address, unsigned char setb)
{
	//Write
	CS=0;
	SPDR=0x40;
	while((SPSR & 0x80)==0);
	SPDR=address;
	while((SPSR & 0x80)==0);
	SPDR= setb;
	while((SPSR & 0x80)==0);
	CS=1;
	delay(5);
}

unsigned char SPI_Read (unsigned char address)
{
	//Read
	unsigned char b;
	CS=0;
	SPDR=0x41;
	while((SPSR & 0x80)==0);
	SPDR=address;
	while((SPSR & 0x80)==0);
	SPDR=0x00;
	while((SPSR & 0x80)==0);
	b=SPDR;
	CS=1;
	delay(5);
	return(b);
}

void main()
{
	unsigned char i,j,value,x;

/*timer0, dinh thoi che do 1*/
	TMOD=0x01; 
	TH0=0x00;
	TL0=0x00;

// Khoi tao truyen du lieu SPI
	SPCR=0x51;
	CS=1;
	Reset=0;
	delay_timer0(10);
	Reset=1;

	init_LCD();
	write_string("DiaBay.vn");
	write_command(0xc0);
	delay(10);

		// SPI init
  // Initial the MCP23S17 SPI I/O Expander
  SPI_Write(IOCONA,0x28);   // I/O Control Register: BANK=0, SEQOP=1, HAEN=1 (Enable Addressing)
  SPI_Write(IODIRA,0x00);   // GPIOA As Output
  SPI_Write(IODIRB,0x00);   // GPIOB As Output
  SPI_Write(GPIOB,0x00);    // Reset Output on GPIOB
  SPI_Write(GPIOA,0x00);    // Reset Output on GPIOA
		
	while(1)
	{
			for(i=0;i<225;i++)
		{
			SPI_Write(GPIOA,i);
			SPI_Write(GPIOB,255-i);
			x=SPI_Read(GPIOA);
			for(j=0;j<8;j++)
			{
				value = x & 0x80;
				if(value ==0x80)
					write_data('1');
				else
					write_data('0');
				x <<=1;
			}
			write_command(0xC0);
			for (j=0;j<50;j++)
			{
				delay_timer0(8333);
			}
		}
	}
}

	