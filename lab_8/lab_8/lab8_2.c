//*******************************************
//Lab 8, Section 2
//Name: Steven Miller
//Class #: 11318
//PI Name: Anthony Stross
//Description: outputs sinewave at 988 hz
//*******************************************
#include <avr/io.h>
#include <avr/interrupt.h>
#define maxvoltage 4095 //2.5v converted to decimal
#define slope (2.5/4095)
extern void clock_init(void);
extern uint8_t* sine_wave;
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
	//enable global interrupts
	PMIC.CTRL = PMIC_MEDLVLEN_bm;
	sei();
	//dummy loop
	while (1)
	{
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
void tcc0_init(void)
{
	uint16_t period =  63;
	uint8_t offset = 0;
	//INITIALIZE COUNT REGISTER
	TCC0.CNT = 0;
	//initialize tcc0 period register
	TCC0.PER = period + offset;
	TCC0.CNT = 0;
	//enable tcc0 overflow interrupts as medium priority
	TCC0.INTCTRLA = TC_OVFINTLVL_MED_gc;
};

ISR(TCC0_OVF_vect)
{
	//reset counter
	TCC0.CNT = 0;
	//begin conversion
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