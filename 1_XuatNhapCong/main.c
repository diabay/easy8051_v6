#include <reg51.h>
#include <stdio.h>
#include <intrins.h>

int j;
unsigned char x; /* bien 8 bit */

void delay()
{
	unsigned long int i;
	for(i=1;i<=5000;++i);
}
void main()
{
	while(1)
	{
		x=0xef;
		for(j=1;j<=8;++j)
		{
			P2=x;
			delay();
			x=_crol_(x,1); /*Quay x de cac LED sang tuan tu*/
		}
	}
}