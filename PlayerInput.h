 #ifndef	__PLAYERINPUT_INCLUDED__
#define __PLAYERINPUT_INCLUDED__
 
                                                                       
#define PLAY (*((volatile unsigned long *)0x40024004))   // main fire button (PE0)

// Initalise port E 
void Controls_Init(void);

// Read slide pot (Busy/Wait) 
unsigned long SlidePot(void); 

#endif
