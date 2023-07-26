//*******************************************
//Lab 8, Section 1
//Name: Steven Miller
//Class #: 11318
//PI Name: Anthony Stross
//Description: outputs constant 1.7 volts using DAC
//*******************************************
#include <avr/io.h>
#define onepointsevenvolts 0xAE1
#define maxvoltage 4095 //2.5v converted to decimal
#define slope (2.5/4095) 
extern void clock_init(void);

int main(void)
{
	//set system clock to 32 mhz
	clock_init();
	//initialize DAC
	dac_init();
	
    while (1) 
    {
		//wait until channel 0 is empty (conversion is finished)
		while(!(DACA.STATUS & DAC_CH0DRE_bm))
		{
			//do nothing
		}
		DACA.CH0DATA = onepointsevenvolts;
    }
}

void dac_init(void)
{
	//use only channel 0
	DACA.CTRLB = DAC_CHSEL_SINGLE_gc;
	//use arefb
	DACA.CTRLC = DAC_REFSEL_AREFB_gc;
	//enable channel 0
	DACA.CTRLA = DAC_CH0EN_bm | DAC_ENABLE_bm;
}

