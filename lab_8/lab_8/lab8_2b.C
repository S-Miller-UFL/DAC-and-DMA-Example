//*******************************************
//Lab 8, Section 2b
//Name: Steven Miller
//Class #: 11318
//PI Name: Anthony Stross
//Description: outputs sinewave at 1567.98 Hz 
//******************************************* 

#include <avr/io.h>
#include <avr/interrupt.h>
#define maxvoltage 4095 //2.5v converted to decimal
#define slope (2.5/4095)
extern void clock_init(void);
extern uint16_t sine_wave[256];
volatile uint8_t index = 255;
int main(void)
{
	//set system clock to 32 mhz
	clock_init();
	//initialize DAC
	dac_init();
	//initialize timer counter
	tcc0_init();
	//start tcc0 timer
	TCC0.CTRLA = TC_CLKSEL_DIV2_gc;
	while (1)
	{
		while(!(DACA.STATUS & DAC_CH0DRE_bm))
		{
			//do nothing
		}
		DACA.CH0DATA = sine_wave[index];
		if(index == 0)
		{
			index = 255;
		}
		else
		{
			index = index-1;
		}
		
	}
}

void dac_init(void)
{
	//use only channel 0
	DACA.CTRLB = DAC_CHSEL_SINGLE_gc| DAC_CH0TRIG_bm;
	//use arefb
	DACA.CTRLC = DAC_REFSEL_AREFB_gc;
	//enable channel 0
	DACA.CTRLA = DAC_CH0EN_bm | DAC_ENABLE_bm;
	//enable conversion trigger on channel event 0
	DACA.EVCTRL = DAC_EVSEL_0_gc;
}
void tcc0_init(void)
{
	uint16_t period = 40;
	int8_t offset = 0;
	//INITIALIZE COUNT REGISTER
	TCC0.CNT = 0;
	//initialize tcc0 period register
	TCC0.PER = period + offset;
	//set tcc0 as event channel 0 source
	EVSYS.CH0MUX = EVSYS_CHMUX_TCC0_OVF_gc;
};
