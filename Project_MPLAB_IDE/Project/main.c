

#include <p24fj128ga010.h>
#include "uart2.h"

//Setting Configuration bits
_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2) 
_CONFIG2( IESO_OFF & FNOSC_PRI &  FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_XT)

void _ISR _T2Interrupt(void);
void GetDataFromUART2(void);
void PassDataToPanTiltSystem(void);


int led_mask = 0b00000000;
int oc1rs_value = 750;
int oc2rs_value = 750;
char Ch[4];

int position_value;



int main(void){

//.....Oscillator Settings......................................................................
	OSCCON = 0x2200;	
						// Current OScillator Selection = Primary Oscillator (XT, HS, EC).
						// New Oscillator Selection = Primary Oscillator (XT, HS, EC).
	CLKDIV = 0X0000;	// No Divide
						//Fosc = 8MHz; Fy (instruction cycle clock) = Fosc/2 = 4MHz

//....Port A Settings............................................................................
						// Connect to LEDs RA0 to RA7 of PORTA
	TRISA = 0xff00;		// PORTA bits 0 through 7 set as output

//....Initialize the UART2 serial port............................................................
	UART2Init();

//....Initialize Timer 2..........................................................................
						//Set timer period
	PR2 = 9999;			// Timer time period = 
						// (PRy + 1)*Tcy*TMRy prescale value = 20 milli seconds
						// Tcy = 1/4MHz
						// TMRy prescale value = 8

//.......PWM settings..............................................................................
	OC1RS = 750; 		// Intial PWM 1 duty cycle = 7.5%
	OC1R = 750; 		

	OC2RS = 750; 		// Intial PWM 2 duty cycle = 7.5%
	OC2R = 750;	

						// Timer 2 Interrupt enable
	_T2IF = 0; 			// Clear Interrupt Flag
	_T2IE = 1;			// Enable Interrupt
	_T2IP = 4;			// Interrupt priority

	
	_IPL = 0; 			// Set processor priority level; default value

	TMR2 = 0;			// Clear the timer

						//Output compare module 1 - set PWM
	OC1CON = 0x0006;	// Timer 2 is the clock source for output compare;
						// PWM mode on OC1; Fault pin is disabled
						// OC1 is multiplexed with pin RD0

						//Output compare module 2 - set PWM
	OC2CON = 0x0006;	//	Timer 2 is the clock source for the output compare module;
						// PWM mode on OC2; Fault pin is disabled;
						// OC2 is multiplexed with pin RD1


						// Start Timer 
	T2CON = 0x8010; 	// TON = 1;Prescale value = 8; Internal clock (Fosc/2) selected

	

	while(1){
		Ch[0] = UART2GetChar();
		if(Ch[0] == 'p' || Ch[0] == 't'){
			GetDataFromUART2();
			PassDataToPanTiltSystem();
		}
		else if(Ch[0] == 'a'){
			oc1rs_value = 300;
			OC1RS = oc1rs_value;
		}
		else if(Ch[0] == 's'){
			oc1rs_value = 310;
			OC1RS = oc1rs_value;
		}
		else if(Ch[0] == 'd'){
			oc1rs_value = 750;
			OC1RS = oc1rs_value;
		}
		else if(Ch[0] == 'f'){
			oc1rs_value = 1350;
			OC1RS = oc1rs_value;
		}
		else if(Ch[0] == 'g'){
			oc1rs_value = 1370;
			OC1RS = oc1rs_value;
		}
		else if(Ch[0] == 'z'){
			oc2rs_value = 290;
			OC2RS = oc2rs_value;
		}
		else if(Ch[0] == 'x'){
			oc2rs_value = 300;
			OC2RS = oc2rs_value;
		}
		else if(Ch[0] == 'c'){
			oc2rs_value = 750;
			OC2RS = oc2rs_value;
		}
		else if(Ch[0] == 'v'){
			oc2rs_value = 1330;
			OC2RS = oc2rs_value;
		}
		else if(Ch[0] == 'b'){
			oc2rs_value = 1345;
			OC2RS = oc2rs_value;
		}

	
	}
	return(0);
}

void _ISR _T2Interrupt(void){
	


	led_mask = led_mask ^ 0b10000000;
	LATA = led_mask;

	_T2IF = 0; // Clear interrupt flag
}


void GetDataFromUART2(void){


	Ch[1] = UART2GetChar();
	Ch[2] = UART2GetChar();
	Ch[3] = UART2GetChar();


	position_value = Ch[3] - '0';
	position_value = position_value + ((Ch[2] - '0')*10);
	position_value = position_value + ((Ch[1] - '0')*100);

}

void PassDataToPanTiltSystem(void){

	if(Ch[0] == 't'){
		if(position_value > 500 && position_value < 1000)
		{
			oc1rs_value = position_value;
			OC1RS = oc1rs_value;
			led_mask = led_mask ^ 0b00000001;
		}
	}
	else if(Ch[0] == 'p'){
		if(position_value > 500 && position_value < 1000)
		{
			oc2rs_value = position_value;
			OC2RS = oc2rs_value;
			led_mask = led_mask ^ 0b00000010;
		}

	}

}

