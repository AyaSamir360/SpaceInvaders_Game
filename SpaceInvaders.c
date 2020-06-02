#include "tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"
#include "PlayerInput.h"
#include "Monsters.h"



#define PLAYERH 8 // height of player ship's row 
#define BUNKERH 5 // height of bunker's row 
#define MAXFPS 15 // maximum Frames per second 
#define MINSCREENY 2 // Minimum screen y-coord (top)
#define MAXSCREENY 47 // Maximum screen y-coord (bottom)
#define EFFSCREENH (MAXSCREENY - MINSCREENY) // effective screen height
#define MINSCREENX 0 // Minimum screen x-coord (left)
#define UFOH 7 
#define MAXSCREENX 82 // Maximum screen x-coord (right)
#define EFFSCREENW (MAXSCREENX - MINSCREENX) 
#define MONSTERGAPX 0 // horizontal gap between adjacent monsters
#define MONSTERGAPY 1 // vertical gap between adjacent monsters
#define MONSTERROWS 2 // number of rows in enemy horde
#define MONSTERCOLS 4 // number of columns in enemy horde
#define MONSTERNUM (MONSTERROWS * MONSTERCOLS) // number of enemies in horde
#define BUNKERNUM 3 // max number of bunkers
#define MONSTERSHOOTDIV 1000
#define TIMERMAX 12

unsigned long score;
unsigned int level;
unsigned long monsterSpeed; 
unsigned long monsterShoot; 


sprite playerSpr, UFOSpr, bunkerSprs[BUNKERNUM], monsterSprs[MONSTERNUM]; // sprites
sprite playerBullet, monsterBullets[MONSTERCOLS]; // bullet sprites

unsigned char screenRefreshFlag;
unsigned char MonsterMove; 
unsigned char timer; 
unsigned char lvlCleared; 
sprite* toExplode; 

void DisableInterrupts(void); 
void EnableInterrupts(void);  

void TitleScreen(void); 
void GameOver(void); 
void SysTick_Init(unsigned long period); // screen refresh & player input
void Timer1_Init(unsigned char seconds); 
void Timer2_Init(unsigned long freq); 
void PrintSpr(sprite* spr, unsigned char var); 
void ExplodeSpr(sprite* spr); // Print sprite's explosion
void PrintBunker(sprite* bunker); 
void GameInit(void);                       // Initalize player & bunker
void EnemyHordeInit(void);
void SprCrash(sprite* bullet, sprite* spr, unsigned char explodeFlag); // Checks collision and  update
void Delay100ms(unsigned long count);
void MonsterShoot(void){   //change monster shoot after every level
	if(level == 1)
		monsterShoot = 20;
	else if (level < 9)
	monsterShoot += 20;
	else
		monsterShoot += 100 / level;}

void MonsterSpeedFn(){        //change monster speed after every level
	if(level == 1)
		monsterSpeed = 5000;
	else
		monsterSpeed -= 500 / level;
}


int main(void){
	unsigned char i;	
	TExaS_Init(NoLCD_NoScope);  
	Random_Init(1);
	Nokia5110_Init();	
	Controls_Init();
	EnableInterrupts(); 
	
	while(1){ 	// Update screen  showing 
		Timer1_Init(MAXFPS);
		GameInit();
		screenRefreshFlag = 1;
		lvlCleared = 1;	
		TitleScreen();
		while(playerSpr.hp > 0){
			while(screenRefreshFlag == 0){}
			if(lvlCleared){
				level += 1;
				EnemyHordeInit();
				MonsterShoot();
				MonsterSpeedFn();
				SysTick_Init(monsterSpeed);
				lvlCleared = 0;	}
		Nokia5110_ClearBuffer();			//show SCREEN
		
			if(playerSpr.hp > 0)        //  ship of Player
				PrintSpr(&playerSpr,0);
			if(playerBullet.hp > 0)                  //  bullets of Player
				PrintSpr(&playerBullet,0);
			for(i = 0; i < BUNKERNUM; i++){                // Bunkers
				PrintBunker(&bunkerSprs[i]);
			} for(i = 0; i < MONSTERNUM; i++){              // Monsters
				if(monsterSprs[i].hp > 0)
					PrintSpr(&monsterSprs[i], timer % 2);
			} for(i = 0; i < MONSTERCOLS; i++){         // bullets of monster
				if(monsterBullets[i].hp > 0)
					PrintSpr(&monsterBullets[i], 0);
			} 
			if(toExplode != 0){ 
				ExplodeSpr(toExplode);
				toExplode = 0;}
			Nokia5110_DisplayBuffer();
			screenRefreshFlag = 0;}

		GameOver();
	}}

	
void SysTick_Handler(void){ //this function Update game including move monsters, shoots, collisions and so on 
  unsigned char i, j;
	for(i = 0; i < MONSTERNUM; i++){ // for monster Moving
		if(monsterSprs[i].hp > 0){ 
			monsterSprs[i].x += MonsterMove == 0 ? 1 : -1;
			PrintSpr(&monsterSprs[i], timer%2);	}}
	for(i = 0; i < MONSTERCOLS; i++){ // for Shoots 
		if((monsterBullets[i].hp == 0) &&
			(((Random()) % MONSTERSHOOTDIV) < monsterShoot)){ // random shoots
			for(j = 0; j < MONSTERROWS; j++){ 
				if(monsterSprs[j * MONSTERROWS + i].hp != 0){
					monsterBullets[i] = ShootLaser(&monsterSprs[j * MONSTERROWS + i], 0);
					break;
				}	}}}
	
	for(j = 0; j < BUNKERNUM; j++)                       // for collosion checks
		SprCrash(&playerBullet, &bunkerSprs[j], 0);
	for(i = 0; i < MONSTERNUM; i++){
		SprCrash(&playerBullet, &monsterSprs[i], 1); 
		SprCrash(&monsterBullets[i], &playerSpr, 1); 
		for(j = 0; j < BUNKERNUM; j++)  
			SprCrash(&monsterBullets[i], &bunkerSprs[j], 0);}
	SprCrash(&playerBullet, &UFOSpr, 1);
	
	for(i = 0; i < MONSTERNUM; i++){	 //for monsters directions	
		if(monsterSprs[i].hp > 0){
			if(monsterSprs[i].x >= SCREENW - monsterSprs[i].width) // Enemy collides on right
				MonsterMove = 1;
			else if(monsterSprs[i].x <= 0) 
				MonsterMove = 0;}}


	lvlCleared = 1;
	for(i = 0; i < MONSTERNUM; i++){
		if(monsterSprs[i].hp > 0)
			lvlCleared = 0;	}
	timer = (timer + 1) % TIMERMAX;}

void Timer1A_Handler(void){
	unsigned char i;
	TIMER1_ICR_R = 0x00000001;
	
	playerSpr.x = MINSCREENX + SlidePot() * (EFFSCREENW - playerSpr.width - 1) / 4094 + 1; 
	if((PLAY && playerBullet.hp == 0) && playerSpr.hp > 0){ 
		playerBullet = ShootLaser(&playerSpr, 1);	}

	if(playerBullet.hp != 0){
		if(playerBullet.y <= MINSCREENY + playerBullet.height)
			playerBullet.hp = 0;
		else 
			playerBullet.y -= 1; 
	}
	
	for(i = 0; i < MONSTERNUM; i++){ 
		if(monsterBullets[i].hp > 0){
			if(monsterBullets[i].y >= MAXSCREENY)
				monsterBullets[i].hp = 0;
			else 
				monsterBullets[i].y += 1;}	}
	screenRefreshFlag = 1;}

	void TitleScreen(void){   // the start sreen
	Nokia5110_Clear();
	Nokia5110_SetCursor(3,1);
	Nokia5110_OutString("SPACE"); 
	Nokia5110_SetCursor(2,2);
	Nokia5110_OutString("INVADERS!");
	Nokia5110_SetCursor(1,4);
	Nokia5110_OutString("Press Play");
	Delay100ms(3);	
	while(!PLAY){};
	Delay100ms(3);
}

void GameOver(void){  //  the Game Over Screen
	Nokia5110_Clear();
	Nokia5110_SetCursor(1,1);
	Nokia5110_OutString("Good Job!");
	Nokia5110_SetCursor(0,3);
	Nokia5110_OutString("Score");
	Nokia5110_OutUDec(score);
	Nokia5110_SetCursor(0,4);
	Nokia5110_OutString("level:");
	Nokia5110_OutUDec(level);
	Delay100ms(3);	
	while(!PLAY){};
	Delay100ms(3);
}

void SysTick_Init(unsigned long milliseconds){
	NVIC_ST_CTRL_R = 0; 
	NVIC_ST_RELOAD_R = milliseconds * 80000 - 1;
  NVIC_ST_CURRENT_R = 0;
  NVIC_SYS_PRI3_R |= 0x01 << 29; 
	NVIC_ST_CTRL_R = 0x07;}

void Timer1_Init(unsigned char freq){  // Initalization of Timer1 to trigger every specified seconds 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x02;
  delay = SYSCTL_RCGCTIMER_R;
  TIMER1_CTL_R = 0x00000000;
  TIMER1_CFG_R = 0x00000000;
  TIMER1_TAMR_R = 0x00000002;                    // default down-count
  TIMER1_TAILR_R = 80000000 / freq -1;
  TIMER1_TAPR_R = 0;
  TIMER1_ICR_R = 0x00000001;
  TIMER1_IMR_R = 0x00000001;
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF0FFF)|(0x02 <<13); 
	NVIC_EN0_R |= 1<<21;
	TIMER1_CTL_R = 0x00000001; 
}

void Timer2_Init(unsigned long seconds){ // Initalization of Timer2 to trigger every few seconds
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;
  delay = SYSCTL_RCGCTIMER_R;
  TIMER2_CTL_R = 0x00000000;
  TIMER2_CFG_R = 0x00000000; 
  TIMER2_TAMR_R = 0x00000002; 
  TIMER2_TAILR_R = seconds * 80000000 - 1; 
  TIMER2_TAPR_R = 0; 
  TIMER2_ICR_R = 0x00000001;
  TIMER2_IMR_R = 0x00000001;
  NVIC_PRI5_R = (NVIC_PRI5_R&0x0FFFFFFF)|(0x03 << 29); 
	NVIC_EN0_R |= 1<<23;
	TIMER2_CTL_R = 0x00000001; 
}
void PrintSpr(sprite* spr, unsigned char var){
	Nokia5110_PrintBMP(spr->x, spr->y, spr->image[var],0);}

void ExplodeSpr(sprite* spr){
	Nokia5110_PrintBMP(spr->x, spr->y, spr->explode,0);}

void PrintBunker(sprite* bunker){
	if(bunker->hp > (20))
		PrintSpr(bunker, 0);
	else if(bunker->hp > (10))
		PrintSpr(bunker, 1);
	else if(bunker->hp > 0)
		PrintSpr(bunker,2 );}

void GameInit(void){
	unsigned char x_coord, y_coord; 		
	unsigned char i; 
	unsigned char gap; 
	sprite tmp; 
	level = 0;
	score = 0;
	MonsterMove = 0;
	timer = 0;
	playerBullet.hp = 0;
	for(i = 0; i < MONSTERNUM; i++){
		monsterSprs[i].hp = 0;
		monsterBullets[i].hp = 0;
	}
	tmp = Player_Init(0,0);
	playerSpr = Player_Init(MINSCREENX + (EFFSCREENW - tmp.width)/2,MAXSCREENY);
	tmp = Bunker_Init(0,0);
	y_coord = MAXSCREENY - PLAYERH;
	gap = (EFFSCREENW - BUNKERNUM * tmp.width) / (BUNKERNUM - 1);
	for(i = 0; i < BUNKERNUM; i++){ // For each bunker
		x_coord = MINSCREENX + i * (tmp.width + gap);
		bunkerSprs[i] = Bunker_Init(x_coord, y_coord);
	}}


void EnemyHordeInit(void){  //fill Monster arrauy
	unsigned char y_coord, x_coord; 
	unsigned char i, j; 
	unsigned char	var; 
	sprite enemyT = Enemy_Init(0,0,0); 
	MonsterMove = 0; 
	for(i = 0; i < MONSTERNUM; i++){ 
		monsterSprs[i].hp = 0; }
	for(i = 0; i < MONSTERROWS; i++){ 
		y_coord = MINSCREENY + UFOH + (enemyT.height + 1) * (MONSTERROWS - i);	
		var = (Random() >> 24) % 4;	
		for(j = 0; j < MONSTERCOLS; j++){ 
			x_coord = MINSCREENX + j * (enemyT.width + MONSTERGAPY);											
			monsterSprs[i * MONSTERCOLS + j] = Enemy_Init(x_coord, y_coord, var);}  }}

void SprCrash(sprite* bullet, sprite* spr, unsigned char explodeFlag){
	if((bullet->hp > 0 && spr->hp > 0) && CrashCheck(bullet, spr)){
		bullet->hp -= 1;
		spr->hp -= 1;
		score += spr->pt * level;
		if(explodeFlag == 1 && spr->hp == 0)
			toExplode = spr;
	}}

void Delay100ms(unsigned long count){
	unsigned long volatile time;
  while(count>0){
    time = 727240;  
    while(time){
	  	time--;
    }
    count--;
  }
}
