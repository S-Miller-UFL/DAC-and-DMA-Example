//*******************************************
//Lab 8, Section 5
//Name: Steven Miller
//Class #: 11318
//PI Name: Anthony Stross
//Description: synthesizer!
//*******************************************

#include <avr/io.h>
#include <avr/interrupt.h>
#define maxvoltage 4095 //2.5v converted to decimal
#define slope (2.5/4095) //0.000610500611 volts/decimalvalue
extern void clock_init(void);
extern uint16_t sine_wave[256];
extern uint16_t triangle_wave[256];
//global variables
volatile uint8_t poweramp_on = (0x01<<7);
volatile uint8_t wave_type = 0; //0 = sine/ 1 = triangle
volatile int8_t bsel = 4;
volatile int8_t bscale = 1;
volatile uint8_t DMA_CH_DISABLE_bm = (0x1<<7);
volatile uint8_t switch_wave_flag = 0;
volatile uint8_t switch_frequency_flag = 0;
volatile uint8_t key_pressed =0;
volatile uint8_t note = 0;
/////////////////////
int main(void)
{
	//set system clock to 32 mhz
	clock_init();
	//initialize DAC
	dac_init();
	//initialize power amplifier
	poweramp_init();
	//initialize timer counter
	tcc0_init();
	//initialize DMA system
	DMA_CH0_INIT();
	//initialize usart set baud rate to 200000
	usartd0_init();
	//enable global interrupts
	PMIC.CTRL = PMIC_MEDLVLEN_bm;
	sei();
	while (1)
	{
			//check if we need to switch waveforms
			if(switch_wave_flag == 1)
			{
				switch_wave_flag = 0;
				//switch to triangle wave
				if(wave_type == 1)
				{
					switch_waves(1);
				}
				//switch to sine wave
				else if (wave_type == 0)
				{
					switch_waves(0);
				}
			}
			else if(key_pressed)
			{
				//if key pressed, turn on timer
				TCC0.CTRLA = TC_CLKSEL_DIV2_gc;
				TCC0.PER = note;
				//keep timer on for certain amount of time
				for(volatile uint16_t i =0;i < 32767; i++)
				{
					while(!(TCC0.INTFLAGS &TC0_OVFIF_bm))
					{
						//do nothing
					}
				}
				//turn off timer
				TCC0.CTRLA = 0;
				key_pressed = 0;
			}
			
	}
}

void dac_init(void)
{
	//use only channel 1
	DACA.CTRLB = DAC_CHSEL_SINGLE1_gc;
	//use arefb
	DACA.CTRLC = DAC_REFSEL_AREFB_gc;
	//enable channel 1
	DACA.CTRLA = DAC_CH1EN_bm | DAC_ENABLE_bm;
}
void tcc0_init(void)
{
	uint16_t period = 63;
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
	//RESET DMA SYSTEM
	DMA.CTRL = DMA_CH_DISABLE_bm;
	DMA.CTRL = DMA_RESET_bm;
	//set beginning of sinewave table as source address on ch0
	DMA.CH0.SRCADDR0 = (uint8_t)((uintptr_t)sine_wave);
	DMA.CH0.SRCADDR1 = (uint8_t)(((uintptr_t)sine_wave)>>8);
	DMA.CH0.SRCADDR2 = (uint8_t)((uint32_t)(((uintptr_t)sine_wave)>>16));
	//set daca ch1 register as destination address
	DMA.CH0.DESTADDR0 = (uint8_t)((uintptr_t)&DACA.CH1DATA);
	DMA.CH0.DESTADDR1 = (uint8_t)(((uintptr_t)&DACA.CH1DATA)>>8);
	DMA.CH0.DESTADDR2 = (uint8_t)((uint32_t)(((uintptr_t)&DACA.CH1DATA)>>16));
	//set block transfer size on ch0 to 512 bytes
	DMA.CH0.TRFCNT = 512;
	//set burst size to 2 bytes and turn on repeat mode
	DMA.CH0.CTRLA = (DMA_CH_BURSTLEN_2BYTE_gc |DMA_CH_REPEAT_bm);
	//enable single shot mode
	DMA.CH0.CTRLA |= DMA_CH_SINGLE_bm;
	//set dma ch0 to reload source address after block transfer and
	//destination address after burst transfer
	DMA.CH0.ADDRCTRL = (DMA_CH_SRCRELOAD_BLOCK_gc |  DMA_CH_DESTRELOAD_BURST_gc);
	//set dma to increment source and destination address after byte transfer
	DMA.CH0.ADDRCTRL |= (DMA_CH_SRCDIR_INC_gc|DMA_CH_DESTDIR_INC_gc);
	//set dma ch0 as event channel 0 user
	DMA.CH0.TRIGSRC = DMA_CH_TRIGSRC_EVSYS_CH0_gc;
	//enable dma ch0
	DMA.CTRL = DMA_ENABLE_bm;
	DMA.CH0.CTRLA |= DMA_CH_ENABLE_bm;

}


void poweramp_init(void)
{
	//TURN ON THE POWER AMPLIFIER
	PORTC.OUTSET = poweramp_on;
	PORTC.DIRSET = poweramp_on; 
	
}
void usartd0_init(void)
{
	//initialize transmitter and reciever pins
	PORTD.OUTSET = PIN3_bm;
	PORTD.DIRSET = PIN3_bm;
	PORTD.DIRCLR = PIN2_bm;

	//set baud rate
	USARTD0.BAUDCTRLA = (uint8_t)bsel;
	USARTD0.BAUDCTRLB = (uint8_t)((bscale << 4)|(bsel >> 8));

	//set to 8 bit odd parity with 1 stop bit
	USARTD0.CTRLC =	(USART_CMODE_ASYNCHRONOUS_gc |USART_PMODE_ODD_gc| USART_CHSIZE_8BIT_gc)&(~USART_SBMODE_bm);

	//ENABLE TRANSMITTER AND RECIEVER
	USARTD0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
	
	//enable reciever interrupts
	USARTD0.CTRLA = USART_RXCINTLVL_MED_gc;
}
ISR(USARTD0_RXC_vect)
{
	char data;
	data = USARTD0.DATA;
	if(data == 's' || data == 'S')
	{
		if(wave_type == 1)
		{
			//SWITCH TO SINE WAVE
			wave_type = 0;
			switch_wave_flag = 1;
		}
		else if(wave_type == 0)
		{
			//SWITCH TO	TRIANGLE WAVE
			wave_type = 1;
			switch_wave_flag = 1;
		}
	}
	//check for notes
	/*
	E = 61
	4 = 57
	R = 54
	5 = 51
	T = 48
	Y = 45
	7 = 43
	U = 40
	8 = 38
	I = 36
	9 = 34
	O = 32
	P = 30
	
*/
	else
	{
		if(data == 'e' || data == 'E')
		{
			key_pressed = 1;
			note = 61;
		}
		else if(data == '4' || data == '4')
		{
			key_pressed = 1;
			note = 57;
		}
		else if(data == 'r' || data == 'R')
		{
			key_pressed = 1;
			note = 54;
		}
		else if(data == '5' || data == '5')
		{
			key_pressed = 1;
			note = 51;
		}
		else if(data == 't' || data == 'T')
		{
			key_pressed = 1;
			note = 48;
		}
		else if(data == 'y' || data == 'Y')
		{
			key_pressed = 1;
			note = 45;
		}
		else if(data == '7' || data == '7')
		{
			key_pressed = 1;
			note = 43;
		}
		else if(data == 'u' || data == 'U')
		{
			key_pressed = 1;
			note = 40;
		}
		else if(data == '8' || data == '8')
		{
			key_pressed = 1;
			note = 38;
		}
		else if(data == 'i' || data == 'I')
		{
			key_pressed = 1;
			note = 36;
		}
		else if(data == '9' || data == '9')
		{
			key_pressed = 1;
			note = 34;
		}
		else if(data == 'o' || data == 'O')
		{
			key_pressed = 1;
			note = 32;
		}
		else if(data == 'p' || data == 'P')
		{
			key_pressed = 1;
			note = 30;
		}
		
		
		
	}
	
}

void switch_waves(uint8_t flag)
{
	//switch to sine
	if(flag == 0)
	{
		//initialize DMA system
		DMA.CTRL = DMA_CH_DISABLE_bm;
		DMA.CTRL = DMA_RESET_bm;
		//set beginning of sinewave table as source address on ch0
		DMA.CH0.SRCADDR0 = (uint8_t)((uintptr_t)sine_wave);
		DMA.CH0.SRCADDR1 = (uint8_t)(((uintptr_t)sine_wave)>>8);
		DMA.CH0.SRCADDR2 = (uint8_t)((uint32_t)(((uintptr_t)sine_wave)>>16));
		//set daca ch1 register as destination address
		DMA.CH0.DESTADDR0 = (uint8_t)((uintptr_t)&DACA.CH1DATA);
		DMA.CH0.DESTADDR1 = (uint8_t)(((uintptr_t)&DACA.CH1DATA)>>8);
		DMA.CH0.DESTADDR2 = (uint8_t)((uint32_t)(((uintptr_t)&DACA.CH1DATA)>>16));
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
		wave_type = 0;
	}
	//switch to triangle
	else if(flag ==1)
	{
		//initialize DMA system
		DMA.CTRL = DMA_CH_DISABLE_bm;
		DMA.CTRL = DMA_RESET_bm;
		//set beginning of triangle wave table as source address on ch0
		DMA.CH0.SRCADDR0 = (uint8_t)((uintptr_t)triangle_wave);
		DMA.CH0.SRCADDR1 = (uint8_t)(((uintptr_t)triangle_wave)>>8);
		DMA.CH0.SRCADDR2 = (uint8_t)((uint32_t)(((uintptr_t)triangle_wave)>>16));
		//set daca ch1 register as destination address
		DMA.CH0.DESTADDR0 = (uint8_t)((uintptr_t)&DACA.CH1DATA);
		DMA.CH0.DESTADDR1 = (uint8_t)(((uintptr_t)&DACA.CH1DATA)>>8);
		DMA.CH0.DESTADDR2 = (uint8_t)((uint32_t)(((uintptr_t)&DACA.CH1DATA)>>16));
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
		wave_type =1;
	} 
	
}