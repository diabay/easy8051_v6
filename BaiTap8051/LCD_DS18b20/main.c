#include <reg52.h>
#include <stdio.h>
//#include <math.h>
#include <intrins.h>
#include <string.h>
#define Pdata P2

#define RS 0x01
#define EN 0x02

sbit DQ=P1^2;

//sbit RS= P2^0;
//sbit EN= P2^1;

unsigned char x,y,z;
unsigned char giay;

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

///////[start] Do nhiet do ds18b20//////////////

// OW_RESET - performs a reset on the one-wire bus and
// returns the presence detect. Reset is 480us, so delay
// value is (480-24)/16 = 28.5 - we use 29. Presence checked
// another 70us later, so delay is (70-24)/16 = 2.875 - we use 3.

unsigned char ow_reset(void)
{
	unsigned char presence;
	DQ = 0; //pull DQ line low
	delay_timer0(400); // leave it low for 480us
	DQ = 1; // allow line to return high
	delay_timer0(58); // wait for presence
	presence = DQ; // get presence signal
	delay_timer0(342); // wait for end of timeslot
	return(presence); // presence signal returned
}// return 0: good

unsigned char read_byte()
{
unsigned char i;
unsigned char value = 0;
	for (i=0;i<8;i++)
	{
		DQ = 0; // pull DQ low to start timeslot
		delay_timer0(5);
		DQ = 1; // then return high
		delay_timer0(8); // delay 15us from start of timeslot (9us)
		if(DQ==1) {value= (value | 0x01)<<i;} // reads byte in, one byte at a time and then
																	// shifts it left
		delay_timer0(46); 											// wait for rest of timeslot
	}
return(value);
}

void write_byte(unsigned char val)
{
unsigned char i;
unsigned char temp;
	for (i=0; i<8; i++) // writes byte, one bit at a time
		{
			temp = val>>i; // shifts val right 'i' spaces
			temp &= 0x01; // copy that bit to temp
			DQ = 0; // pull DQ low to start timeslot
			if(temp==1) 
				{delay_timer0(5); DQ =1;delay_timer0(54);} // return DQ high if write 1
			else	{delay_timer0(50);DQ = 1;delay_timer0(9);}
	// hold value for remainder of timeslot
		}
//delay_timer0(86);
}

int Read_Temperature(void)
{
char get[10];
char temp_lsb,temp_msb;
unsigned char k;
char temp_c;
	EA=0;
while(ow_reset());
write_byte(0xCC); //Skip ROM
write_byte(0x44); // Start Conversion
delay_timer0(100); // 120us
while(ow_reset());
write_byte(0xCC); // Skip ROM
write_byte(0xBE); // Read Scratch Pad
for (k=0;k<2;k++){get[k]=read_byte();}
//printf("\n ScratchPAD DATA = %X%X%X%X%X\n",get[8],get[7],get[6],get[5],get[4],get[3],get[2],get[1],get[0]);
temp_msb = get[1]; // Sign byte + lsbit
temp_lsb = get[0]; // Temp data plus lsb

if (temp_msb <= 0x80){temp_lsb = (temp_lsb/2);} // shift to get whole degree
temp_msb = temp_msb & 0x80; // mask all but the sign bit
if (temp_msb >= 0x80) {temp_lsb = (~temp_lsb)+1;} // twos complement
if (temp_msb >= 0x80) {temp_lsb = (temp_lsb/2);}// shift to get whole degree
if (temp_msb >= 0x80) {temp_lsb = ((-1)*temp_lsb);} // add sign bit
//printf( "\nTempC= %d degrees C\n", (int)temp_lsb ); // print temp. C
//temp_c =  (temp_msb << 8) | temp_lsb ; // ready for conversion to Fahrenheit
//temp_c >>= 4;

temp_c = (int)temp_lsb;

	EA=1;
return(temp_c);
}

void main()
{

	/*timer0, dinh thoi che do 1*/
		TMOD=0x01; 
		TH0=0x00;
		TL0=0x00;

		init();
		write_string("DiaBay.vn");
		write_command(0xc0);
		delay(10);
		write_string("Hello World!");

	while(1)
	{
			giay = Read_Temperature();
			x=giay%10; giay=giay/10;
			y=giay%10; z=giay/10;
			x=x+48;
			y=y+48;
			z=z+48;
		
			write_command(0x8A);
			write_data(z);
			write_data(y);
			write_data(x);
			write_command(0x8E);
			write_data('C');
		
//			for(i=0;i<16;i++)
//			{
//				write_command(0x18);
//				for(j=0;j<50;j++)
//				{
//					delay_timer0(16667);
//				}
//			}
			
	}
}

	