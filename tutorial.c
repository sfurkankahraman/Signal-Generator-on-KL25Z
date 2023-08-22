#include <MKL25Z4.H>
#include <stdio.h>
#include <math.h>
#include "utils.h"

void ADC0_init(void);
void DAC0_init(void);
void saw_tooth_generator(void);
void square_generator(void);
void sinusoidal_generator(float sine[]);
void SysTick_Handler(void) ;
int check = 0;
long int result;
int i = 0;
int t;
int coef = 0;
int digits[] = {0,0,0};
static float sinetable[70];
int count;
char mode=1;
float amplitude;
float amplitude2;
int furkan;


int main (void)
{
LCD_init(); /* Configure LEDs */
ADC0_init(); /* Configure ADC0 */
DAC0_init(); /* Configure DAC0 */
__disable_irq();
SIM->SCGC5 |= 0x200; /* enable clock to Port A */
	
PORTA->PCR[1]|=0x00100; //interrupt
PORTA->PCR[1]|=0x00003;
PTA->PDDR&=~0x0002;
SysTick->LOAD = 2400000;
SysTick->CTRL = 3;
__enable_irq();
for (i = 0; i <70; i++) {
sinetable[i]=(sinf(2*3.1416/70*i));  //interrupt
if( sinetable[i]<0){
sinetable[i]=0;
}
}

/******************************************************/
    /* the top loop it always iterate 3 waveform */
/******************************************************/

while(1){

/*****************************************************/
               /* sawtooth waveform */
/*****************************************************/
	

while(1){

Delay(30);
saw_tooth_generator();
if(!(GPIOA->PDIR & (1UL << 1)))
break;
}

/*****************************************************/
                  /* sin waveform */
/*****************************************************/

while(1){
Delay(30);
sinusoidal_generator(sinetable);
if(!(GPIOA->PDIR & (1UL << 1)))
break;
}

/*****************************************************/
                 /* square waveform */
/*****************************************************/

while(1){	
Delay(30);
square_generator();
if(!(GPIOA->PDIR & (1UL << 1)))
break;
}
}
}


/*****************************************************/
/*              end of the main functiion            */
/*****************************************************/

void SysTick_Handler(void) {
	i = 0;
	ADC0->SC1[0] = 0; 								/* start conversion on channel 0 */
	while(!(ADC0->SC1[0] & 0x80)) {}  /* wait for conversion complete */
	result = ADC0 -> R[0]; 						/* read conversion result and clear COCO flag */
	furkan = (4095-result);
	result = (4095-result)/12.3;
		 
	amplitude = result;
	if ( amplitude == amplitude2){}
	else{
	amplitude2 = amplitude;
	}
	clear_lcd();
	print_fnc("Frequency=750Hz");
	LCD_command(0xC0);
	print_fnc("Amplitude= ");

int result1 = 0;
int temp0;
int temp1;
int temp2;

result1 = result;

temp0 = result1 / 100;
result1 -= temp0 * 100;
temp1 = result1 / 10;
temp2 = result1 - (temp1*10);

LCD_data(temp0+0x30);
LCD_data('.');
LCD_data(temp1+0x30);
LCD_data(temp2+0x30);
LCD_data('V');
}

void ADC0_init(void)
{
	SIM->SCGC5 |= 0x2000;     /* clock to PORTE */
	PORTE->PCR[20] = 0;       /* PTE20 analog input */
	SIM->SCGC6 |= 0x8000000;  /* clock to ADC0 */
	ADC0->SC2 &= ~0x40; 			/* software trigger */
	                          /* clock div by 4, long sample time, single ended 12 bit, bus clock */
	ADC0->CFG1 = 0x40 | 0x10 | 0x04 | 0x00;
}

void DAC0_init(void) {
	SIM->SCGC6 |= 0x80000000;  /* clock to DAC module */
	DAC0->C1 = 0; 						 /* disable the use of buffer */
	DAC0->C0 = 0x80 | 0x20;		 /* enable DAC and use software trigger */
}

/******************************************************/
			  	/*	sawtooth generator function */
/******************************************************/

void saw_tooth_generator(void){
	for(int i=0;i<0x1000;i+=0x002b){
	DAC0->DAT[0].DATL =(i*furkan/0xfff)&0xff;
	DAC0->DAT[0].DATH =((i>>8)*furkan/0xfff)&0x0f;
	Delay(3);
}
}

/******************************************************/
			  	/*	sinusoidal generator function */
/******************************************************/

void sinusoidal_generator(float sine[]){
for(int i=0;i<70;i++){
	DAC0->DAT[0].DATL = ((int)(furkan*sine[i]))&(0xff);  
  DAC0->DAT[0].DATH = ((int)(furkan*sine[i]) >> 8)&(0x0f);
	Delay(6);
}
}

/******************************************************/
					/*	square generator function */
/******************************************************/

void square_generator(void){
	for(int i=0;i<208;i++){
	DAC0->DAT[0].DATL=furkan&0xff; 
	DAC0->DAT[0].DATH=(furkan>>8)&0x0f;
	Delay(1);
}
	for(int i=0;i<208;i++){
	DAC0->DAT[0].DATL=0;
	DAC0->DAT[0].DATH=0;
	Delay(1);
}
}


/* sawteeth waves 
key = 0;
while(key != 1)
{
key = get_number_from_keypad();
DAC0_init();
for (int i = 0; i < 0x1000; i += 0x0010){
DAC0->DAT[0].DATL = i & 0xff;
DAC0->DAT[0].DATH = (i >> 8) & 0x0f;
delayMs(1); 
}
}*/

/* sine wave 
key = 0;
while(key != 1)
{
key = get_number_from_keypad();
DAC0_init();
for (int i = 0; i < WAVEFORM_LENGTH; i++){
DAC0->DAT[0].DATL = sinewave[i] & 0xff;
DAC0->DAT[0].DATH = (sinewave[i] >> 8) & 0x0f;
delayMs(1);
}
}*/

/* square wave 
key = 0;
while(key != 1)
{
key = get_number_from_keypad();
DAC0_init(); 
for (int i=0;i<2048;i++)
{
DAC0->DAT[0].DATL =0xff;
DAC0->DAT[0].DATH=0x0f;
delayMs(1);
}

for(int i=0;i<2048;i++)
{
DAC0->DAT[0].DATL=0;
DAC0->DAT[0].DATH=0;	
delayMs(1);
}
}
}
}
*/

/*************************************
clear_lcd();
print_fnc("FREQUENCY: 750Hz");
LCD_command(0xC0);
print_fnc("AMPLITUDE: ");
int result1;
result1=0;
int temp0;
int temp1;
int temp2;
result1=result;
temp0 = result1 / 100;
result1 -= temp0 * 100;
temp1 = result1 / 10;
temp2 = result1 - (temp1*10);
LCD_data(temp0+0x30);
LCD_data('.');
LCD_data(temp1+0x30);
LCD_data(temp2+0x30);
LCD_data('V');
*************************************/
