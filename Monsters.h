
#ifndef __SPRITES_INCLUDED__
#define __SPRITES_INCLUDED__

typedef struct {
	unsigned char x; 								
	unsigned char y; 									
	unsigned char hp; 								// hit
	unsigned char pt; 								// score for killing sprite
	unsigned char width;							
	unsigned char height;							
	const unsigned char* explode; 	
	const unsigned char* block; 		
	const unsigned char* image[5]; 	
} sprite;


sprite Player_Init(unsigned char x, unsigned char y);

sprite Bunker_Init(unsigned char x, unsigned char y);

sprite Enemy_Init(unsigned char x, unsigned char y, unsigned char variant);

sprite ShootLaser(sprite* spr, unsigned char updir);

void TitleScreen(void);

void GameOverScreen(void);

unsigned char CrashCheck(sprite* spr1, sprite* spr2);

#endif
