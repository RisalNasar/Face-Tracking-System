/* UART2.H

Implementing the UART2 in the PIC24FJ128GA010 microcontroller with the 
Explorer 16 Development board.

*/


//Prototypes

//UART2 initialization function
extern void UART2Init();

//UART2 transmission function; Parameter 'Ch' is the character to be sent
extern void UART2PutChar(char Ch);

//UART2 receive function; Returns the value received
extern char UART2GetChar ();
