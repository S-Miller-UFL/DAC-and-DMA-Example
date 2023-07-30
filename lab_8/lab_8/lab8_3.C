//*******************************************
//Lab 8, Section 3
//Name: Steven Miller
//Class #: 11318
//PI Name: Anthony Stross
//Description: outputs sinewave at ~1567.98 Hz using DMA
//*******************************************
#include <avr/io.h>
#include <avr/interrupt.h>
#define maxvoltage 4095 //2.5v converted to decimal
#define slope (2.5/4095) //0.000610500611 volts/decimalvalue
extern void clock_init(void);
extern uint16_t sine_wave[256];
volatile uint16_t sine_wave_address = (&sine_wave);
volatile uint16_t daca_ch0_data_address = (&DACA_CH0DATA);
int main(void)
{
	//set system clock to 32 mhz
	clock_init();
	//initialize DAC
	dac_init();
	//initialize timer counter
	tcc0_init();
	//initialize DMA system
	DMA_CH0_INIT();
	//start tcc0 timer
	TCC0.CTRLA = TC_CLKSEL_DIV2_gc;
	//dummy loop
	while (1)
	{
		//DO NOTHING
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
	uint16_t period = 40;
	int8_t offset = 0;
	//INITIALIZE COUNT REGISTER
	TCC0.CNT = 0;
	//initialize tcc0 period register
	TCC0.PER = period + offset;
	//set tcc0 as event channel 0 source
	EVSYS.CH0MUX = EVSYS_CHMUX_TCC0_OVF_gc;
};

void DMA_CH0_INIT(void)
{
	//set beginning of sinewave table as source address on ch0
	DMA.CH0.SRCADDR0 = (uint8_t)((uintptr_t)sine_wave);
	DMA.CH0.SRCADDR1 = (uint8_t)(((uintptr_t)sine_wave)>>8);
	DMA.CH0.SRCADDR2 = (uint8_t)((uint32_t)(((uintptr_t)sine_wave)>>16));
	//set daca ch0 register as destination address
	DMA.CH0.DESTADDR0 = (uint8_t)((uintptr_t)&DACA.CH0DATA);
	DMA.CH0.DESTADDR1 = (uint8_t)(((uintptr_t)&DACA.CH0DATA)>>8);
	DMA.CH0.DESTADDR2 = (uint8_t)((uint32_t)(((uintptr_t)&DACA.CH0DATA)>>16));
	//set block transfer size on ch0 to 512 bytes
	DMA.CH0.TRFCNT = 512;
	//set burst size to 2 bytes and turn on repeat mode
	DMA.CH0.CTRLA |= (DMA_CH_BURSTLEN_2BYTE_gc |DMA_CH_REPEAT_bm);
	//enable single shot mode
	DMA.CH0.CTRLA |= DMA_CH_SINGLE_bm;
	//set dma ch0 to reload source address after block transfer and 
	//destination address after burst transfer
	DMA.CH0.ADDRCTRL |= (DMA_CH_SRCRELOAD_BLOCK_gc |  DMA_CH_DESTRELOAD_BURST_gc);
	//set dma to increment source and destination address after byte transfer
	DMA.CH0.ADDRCTRL |= (DMA_CH_SRCDIR_INC_gc|DMA_CH_DESTDIR_INC_gc);
	//set dma ch0 as event channel 0 user
	DMA.CH0.TRIGSRC |= DMA_CH_TRIGSRC_EVSYS_CH0_gc;
	//enable dma ch0
	DMA.CTRL |= DMA_ENABLE_bm;
	DMA.CH0.CTRLA |= DMA_CH_ENABLE_bm; 
}
 