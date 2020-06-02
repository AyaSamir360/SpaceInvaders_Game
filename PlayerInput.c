#include "PlayerInput.h"
#include "tm4c123gh6pm.h"

void Controls_Init(void){
// Initalise port E
	unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x10; 					// activate clock for Port E
	delay = SYSCTL_RCGC2_R; 					// allow clock to start
	GPIO_PORTE_LOCK_R = 0x4C4F434B;   
	GPIO_PORTE_PCTL_R = 0x00000000;  
	GPIO_PORTE_DIR_R &= ~0x05;        // PE0 & PE2 inputs
  GPIO_PORTE_AMSEL_R &= ~0x01;      
  GPIO_PORTE_AFSEL_R &= ~0x01;      
  GPIO_PORTE_DEN_R |= 0x01;         // enable digital I/O on PE0
	GPIO_PORTE_AMSEL_R |= 0x04;     	
	GPIO_PORTE_AFSEL_R |= 0X04;  			
	GPIO_PORTE_DEN_R &= ~0X04;     		
	// Intialise ADC on PE2
	SYSCTL_RCGC0_R |= 0x00010000;   	// activate ADC0
	delay = SYSCTL_RCGC2_R;        		// allow clock to start
	SYSCTL_RCGC0_R &= ~0x00000300; 	 	
	ADC0_SSPRI_R = 0x0123;          	
	ADC0_ACTSS_R &= ~0x0008;        	// disable SS3
	ADC0_EMUX_R &= ~0xF000;         	// software trigger
	ADC0_SSMUX3_R &= ~0x000F;      	 	
	ADC0_SSMUX3_R += 1;             	
	ADC0_SSCTL3_R = 0x0006;         
	ADC0_ACTSS_R |= 0x0008;        	  
}

unsigned long SlidePot(void){  
	unsigned long result;
	ADC0_PSSI_R = 0x0008;
	while((ADC0_RIS_R&0x08)==0){};
	result = ADC0_SSFIFO3_R&0xFFF;
	ADC0_ISC_R = 0x0008;
	return result;
}
