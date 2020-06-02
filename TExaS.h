// TExaS.h
// Runs on LM4F120/TM4C123
// Periodic timer Timer5A which will interact with debugger and grade the lab 
// It initializes on reset and runs whenever interrupts are enabled
// Jonathan Valvano. Daniel Valvano
// April 9, 2014

enum DisplayType{
  UART0_Emulate_Nokia5110_NoScope,
  SSI0_Real_Nokia5110_Scope,
  SSI0_Real_Nokia5110_NoScope,
  NoLCD_NoScope
};


void TExaS_Init(enum DisplayType display);

void TExaS_Stop(void);
