
#include "Monsters.h"
#include "MonsterImages.c"

#define BUNKERMAXHP 30

sprite Player_Init(unsigned char x, unsigned char y){
	sprite player;
	player.x = x;
	player.y = y;
	player.hp = 1;
	player.pt = 0;
	player.image[0] = Player;
	player.explode = Player_Explode;
	player.width = Player[18];
	player.height = Player[22];
	return player;
}

sprite Bunker_Init(unsigned char x, unsigned char y){
	sprite bunker;
	bunker.x = x;
	bunker.y = y;
	bunker.hp = BUNKERMAXHP;
	bunker.pt = 0;
	bunker.image[0] =  Bunker_NoDmg;
	bunker.image[1] = Bunker_LightDmg;
	bunker.image[2] =Bunker_HeavyDmg;
	bunker.width = Bunker_NoDmg[18];
	bunker.height = Bunker_NoDmg[22];
	return bunker;
}

sprite Enemy_Init(unsigned char x, unsigned char y, unsigned char variant){  // Intialise  monster sprite
	sprite monster;
	monster.x = x;
	monster.y = y;
	monster.hp = 1;
	monster.pt = 1;
	if(variant == 0){ 
		monster.image[0] =  Monster_pointA;
		monster.image[1] =  Monster_pointB;
	} else if(variant == 1){
		monster.image[0] = Monster_pointA;
		monster.image[1] = Monster_pointB;
	} else if(variant == 2){
		monster.image[0] = Monster_pointA;
		monster.image[1] = Monster_pointB;
	} else{
		monster.image[0] = Monster_pointA;
		monster.image[1] = Monster_pointB;
	}
	monster.explode = Enemy_Explode;
	monster.width = Enemy_Explode[18];
	monster.height = Enemy_Explode[22];
	return monster;
}



sprite ShootLaser(sprite* spr, unsigned char updir){
	sprite laser;
	laser.width = Laser[18];
	laser.height = Laser[22];
	laser.x = spr->x + spr->width / 2 - 1;
	laser.y = updir == 0 ? spr->y + laser.height : spr->y - spr->height;
	if(updir == 0)
		laser.y = spr->y + laser.height - 1;
	else
		laser.y = spr->y - spr->height;
	laser.hp = 1;
	laser.pt = 0;
	laser.image[0] = Laser;
	return laser;
}

unsigned char CrashCheck(sprite* spr1, sprite* spr2){
	if((((spr1->x > spr2->x) && (spr1->x - spr2->x < spr2->width)) || 
		 ((spr1->x < spr2->x) && (spr2->x - spr1->x < spr1->width))) && 
	   (((spr1->y > spr2->y) && (spr1->y - spr2->y < spr2->height)) || 
		 ((spr1->y < spr2->y) && (spr2->y - spr1->y < spr1->height)))) 
		return 1;
	else
		return 0;
}

