#include <reg52.h>
#include <stdio.h>
#include <math.h>
#include <intrins.h>
#include <string.h>

sfr Pdata= 0xA0;
sbit RS= P2^0;
sbit EN= P2^1;
char x,str;
sbit hang0 = P0^4;
sbit hang1 = P0^5;
sbit hang2 = P0^6;
sbit hang3 = P0^7;
sbit Enter = P3^6;
unsigned char i,j,n;
//unsigned char m_kytu[4][4]={
//{'1','2','3','A'},
//{'4','5','6','B'},
//{'7','8','9','C'},
//{'*','0','#','D'}};
char m_kytu[]="123A456B789C*0#D";
unsigned char m_hang[4];

void delay10ms(void)
{
	TMOD=0x01;
	TH0=55535/256;
	TL0=35535%256;
	TR0=1;
	while(!TF0);
	TR0=TF0=0;
}
//void delay( int t)
//{
//	int i;
//	for(i=0;i<t;++i);
//}
void delay(unsigned long int t)    		
{
  unsigned long int i,j;
     for(i=0;i<t;i++)
     for(j=0;j<100;j++);
}
//void busy_flag(void)
//{
//	P2= 0xff; //Set cac chan P2 doc tin hieu ???
//	RS=0; // lua chon thanh ghi lenh
//	RW=1; // doc tu lcd module
//	do
//	{
//		EN=1; // tin hieu cho phep
//		delay(10);
//		EN=0;
//		x=P2;
//		x=x&0x80; // lap kiem tra bit D7 cho den khi khong con bang 1
//	} while(x!=0x80);
//}
void write_command(char LCDcmd) // ghi lenh ra LCD
{
//	busy_flag();
	delay10ms();
	Pdata =(LCDcmd & 0xF0)>>2;
	RS=0;	//chon thanh ghi lenh
//	RW=0; // ghi vao LCD module
	EN=1;
	delay(1);
	EN=0;
	delay(2);
}
void write_data(char LCDdt) //ghi du lieu can hien thi ra LCD
{
//	busy_flag();
	delay10ms();
	Pdata =(LCDdt & 0xF0)>>2;
	RS=1; //chon thanh ghi du lieu
//	RW=0;
	EN=1;
	delay(1);
	EN=0;
	delay(2);
}

void write_string( unsigned char *s)
{
	while(*s)//ghi mot chuoi ky tu ra LCD
	{
		write_data(*s);
		write_data(*s <<4);
		s++;
	}
//		int i;
//	for(i=0;s[i]!=0;i++)  /* Send each char of string till the NULL */
//	{
//		write_data (s[i]);  /* Call LCD data write */
//	}
}
void init(void)
{
 delay(20);  /* LCD Power ON Initialization time >15ms */
	write_command (0x02); /* 4bit mode */
	write_command (0x02 << 4);
	write_command (0x28); /* Initialization of 16X2 LCD in 4bit mode */
	write_command (0x28 << 4);
	write_command (0x0C); /* Display ON Cursor OFF */
	write_command (0x0C << 4);
	write_command (0x06); /* Auto Increment cursor */
	write_command (0x06 << 4);
// write_command (0x01); /* Clear display */
// write_command (0x80); /* Cursor at home position */
	write_command(0x01);//0000 0001
	write_command(0x01 << 4);
	write_command(0x80);// 1000 0000
	write_command(0x80 << 4);
}

void main()
{

	P2 = 0x00;
	P3 =0x40;
	init();
		write_string("Thanh oi!");
		write_command(0xc0);// 1100 0000
		write_command(0xc0 << 4);
		write_string("Em co yeu anh?");
		while(!Enter);
		write_command(0x01);//0000 0001
		write_command(0x01 << 4);
		write_command(0x80);// 1000 0000
		write_command(0x80 << 4);
		write_string("phim so may?");
		write_command(0xc0);// 1100 0000
		write_command(0xc0 << 4);

	while(1)
	{
		scan:
		x=0xFE;
		for(i=0;i<=3;++i)
		{
			P0=x;
			x=_crol_(x,1);// lenh quay trai bit
			for(j=0;j<=3;++j)
			{
					m_hang[0]=hang0;
					m_hang[1]=hang1;
					m_hang[2]=hang2;
					m_hang[3]=hang3;
//					i=atoi(str);
//					if (hang1==0) {write_string (&str);}
//					else if (hang2==0) {write_string (&str);}
//					else if (hang3==0) {write_string (&str);}
//					else if (hang0==0) {write_string (&str);}
					if (m_hang[j]==0) 
						{
//						n= strlen(m_kytu);
//						m_kytu[n]='\0';
						str = m_kytu[j*4+i];
						write_string (&str);
						goto scan;
						//while(m_hang[j]==0);
						};
//					itoa(j*4+i,&str);
			}
		}
	}
}

	