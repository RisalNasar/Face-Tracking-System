/* UART2.C

Implementing the UART2 in the PIC24FJ128GA010 microcontroller with the 
Explorer 16 Development board.
*/

#include <p24fj128ga010.h>
#include "uart2.h"

//#define UTXBF 0x0200; // UART Transmit buffer full status bit (read-only)

//UART2 initialization function
void UART2Init(){

//Set up registers

U2BRG =  25;		//Baud Rate Speed
U2MODE = 0x8000;	//Turn on uart2 module; BRGH = 0(low baud rate select)
U2STA = 0x8400;		//Set Receive and Transmit Interrupts;UART2 transmitter enabled

//Reset RX interrupt flag
_U2RXIF = 0;

}

//UART2 transmission function; Parameter 'Ch' is the character to be sent
void UART2PutChar(char Ch){

//Transmit only if TX buffer is empty
//while((U2STA&UTXBF)== UTXBF); // Wait until UART2 Transmit Buffer is empty
//while((U2STA&0x0200) == 0x0200);
while(U2STAbits.UTXBF == 1); // Seems like this is defined in p24fj128ga010.h; Good

U2TXREG = Ch; // Move Ch to Transmit register

}

//UART2 receive function; Returns the value received
char UART2GetChar (){

char Ch;

//wait for Receive buffer to fill up; wait for receive interrupt
while (_U2RXIF == 0);
Ch = U2RXREG;

//Reset the interrupt
_U2RXIF = 0;

//Return the byte received
return(Ch);

}
