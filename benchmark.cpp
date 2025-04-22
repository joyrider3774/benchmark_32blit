#include <stdlib.h>
#include <string.h>
#include <32blit.hpp>
#include "assets.hpp"

using namespace blit;

Surface *Sprite;

constexpr int MaxModes = 8;

const char * ModeNames[MaxModes] = {"L-blit", "L-stretch", "L-blit trans horiz", "L-stretch trans horiz", 
	"H-blit", "H-stretch", "H-blit trans horiz", "H-stretch trans horiz"};

struct SpriteObj 
{
	uint16_t x;
	uint16_t y;
	int8_t xi;
	int8_t yi;
};

SpriteObj *SpriteObjs;
int16_t numSprites = 0;
uint32_t fps_render, fps = 0;
uint8_t mode = 0;
uint32_t start_frame2 = 0;

void addSprites(int8_t amount)
{
	if (amount == 0)
		return;	

	int16_t newAmount = numSprites + amount;
	if(newAmount < 0)
		newAmount = 0;
	
	if (numSprites == 0)
		SpriteObjs = (SpriteObj *)calloc(newAmount, sizeof(SpriteObj));
	else
    {		
		SpriteObjs = (SpriteObj *)realloc(SpriteObjs, newAmount*sizeof(SpriteObj));		
	}

    if(amount > 0)
	{
		for(int16_t i = numSprites; i < numSprites + amount; i++)
		{
			SpriteObjs[i].x = rand() % (screen.bounds.w - 2 * Sprite->bounds.w);
			SpriteObjs[i].y = rand() % (screen.bounds.h - 2 * Sprite->bounds.h);
			SpriteObjs[i].xi = 1 + rand() % 3;
			if (rand() % 2 == 0)
				SpriteObjs[i].xi *= -1;
			SpriteObjs[i].yi = 1 + rand() % 3;
			if (rand() % 2 == 0)
				SpriteObjs[i].yi *= -1;
		}
	}
	numSprites = newAmount;
}

//intialisation of game & global variables
void init() 
{
	set_screen_mode(ScreenMode::lores);
	Sprite = Surface::load_read_only(spritetest);
	mode = 0;
	addSprites(1);
}

void render(uint32_t time) 
{  	
	uint32_t us2 = now_us() - start_frame2;
	char text1[25];
	char text2[20];
	char text3[30];
	sprintf(text1, "%s", ModeNames[mode]);
	sprintf(text2, "Sprites:%d", numSprites);	
	sprintf(text3, "FPS:%lu FPS R:%lu",fps, fps_render); 
	
	uint32_t start_frame = now_us(); 
	screen.pen = Pen(255,255,255);
	screen.clear();		
	for(int16_t i = 0; i < numSprites; i ++)
	{
		switch(mode)
		{
			case 0: //l-blit
			case 4: //h-blit
				screen.blit(Sprite, Rect(0,0, Sprite->bounds.w, Sprite->bounds.h), Point(SpriteObjs[i].x,SpriteObjs[i].y));
				break;
			case 1: //l-stretch_blit
			case 5: //h-stretch_blit
 				screen.stretch_blit(Sprite, Rect(0,0, Sprite->bounds.w, Sprite->bounds.h), Rect(SpriteObjs[i].x,SpriteObjs[i].y, Sprite->bounds.w *2, Sprite->bounds.h *2));
				break;
			case 2: //l-blit flipped
			case 6: //h-blit flipped
				screen.blit(Sprite, Rect(0,0, Sprite->bounds.w, Sprite->bounds.h), Point(SpriteObjs[i].x,SpriteObjs[i].y), SpriteTransform::HORIZONTAL);
				break;
			case 3: //l-stretch_blit flipped
			case 7: //h-stretch_blit flipped
				screen.stretch_blit(Sprite, Rect(0,0, Sprite->bounds.w, Sprite->bounds.h), Rect(SpriteObjs[i].x,SpriteObjs[i].y, Sprite->bounds.w *2, Sprite->bounds.h *2), SpriteTransform::HORIZONTAL);
				break;			
		}
	}
	uint32_t end_frame = now_us();
	uint32_t us = end_frame - start_frame;  
	fps_render = 1000000.0 / (us == 0 ? 1 : us);
	us += us2;
    fps = 1000000.0 / (us == 0 ? 1 : us);
 

	screen.pen = Pen(0,0,0);
	screen.rectangle(Rect(0,0,strlen(text1)* minimal_font.char_w, minimal_font.char_h));
	screen.rectangle(Rect(0,minimal_font.char_h,strlen(text2)* minimal_font.char_w, minimal_font.char_h));
	screen.rectangle(Rect(0,minimal_font.char_h*2,strlen(text3)* minimal_font.char_w, minimal_font.char_h));
	screen.pen = Pen(255,255,255);
	screen.text(text1,minimal_font, Point(0,0), false);
	screen.text(text2,minimal_font, Point(0,minimal_font.char_h), false);
	screen.text(text3,minimal_font, Point(0,minimal_font.char_h*2), false);
	start_frame2 = now_us();
}

void ResetPositionsMovement()
{
	//reset positions to be more random
	for (int i=0; i < numSprites; i++)
	{
		SpriteObjs[i].x = rand() % (screen.bounds.w - 2 * Sprite->bounds.w);
		SpriteObjs[i].y = rand() % (screen.bounds.h - 2 * Sprite->bounds.h);
		SpriteObjs[i].xi = 1 + rand() % 3;
		if (rand() % 2 == 0)
			SpriteObjs[i].xi *= -1;
		SpriteObjs[i].yi = 1 + rand() % 3;
		if (rand() % 2 == 0)
			SpriteObjs[i].yi *= -1;		
	}
}


void update(uint32_t time) 
{
	for(int i = 0; i < numSprites; i ++)
	{
		if (SpriteObjs[i].x + SpriteObjs[i].xi < 0)
			SpriteObjs[i].xi = abs(SpriteObjs[i].xi);

		if (SpriteObjs[i].x + SpriteObjs[i].xi + Sprite->bounds.w * (((mode == 5) || (mode == 7) ||  (mode == 1) || (mode == 3)) ? 2 : 1)  >=  screen.bounds.w)
			SpriteObjs[i].xi = -abs(SpriteObjs[i].xi);
		
		if (SpriteObjs[i].y + SpriteObjs[i].yi < 0)
			SpriteObjs[i].yi = abs(SpriteObjs[i].yi);
		
		if (SpriteObjs[i].y + SpriteObjs[i].yi + Sprite->bounds.h * (((mode == 5) || (mode == 7) || (mode == 1) || (mode == 3)) ? 2 : 1) >=  screen.bounds.h)
			SpriteObjs[i].yi = -abs(SpriteObjs[i].yi);
		
		SpriteObjs[i].x =  SpriteObjs[i].x + SpriteObjs[i].xi;
		SpriteObjs[i].y =  SpriteObjs[i].y + SpriteObjs[i].yi;
	}

	if ((buttons.pressed & Button::DPAD_LEFT))
		addSprites(-1);
	
	if (buttons.pressed & Button::DPAD_RIGHT)
		addSprites(1);
	
	if ((buttons.pressed & Button::DPAD_DOWN))
		addSprites(-10);

	if (buttons.pressed & Button::DPAD_UP)
		addSprites(10);

	if ((buttons.pressed & Button::X))
		addSprites(+25);
	
	if ((buttons.pressed & Button::Y))
		addSprites(-25);

	if (buttons.pressed & Button::B)
	{
		if(mode == 0)
		{
			mode = MaxModes -1;
			set_screen_mode(ScreenMode::hires);
			ResetPositionsMovement();
		}
		else
		{
			mode--;
			if(mode == 3)
			{
				set_screen_mode(ScreenMode::lores);
				ResetPositionsMovement();
			}
		}
	}

	if (buttons.pressed & Button::A)
	{
		mode++;
		if(mode >= MaxModes)
		{
			mode = 0;
			set_screen_mode(ScreenMode::lores);
			ResetPositionsMovement();
		}
		if(mode == 4)
		{
			set_screen_mode(ScreenMode::hires);
			ResetPositionsMovement();			
		}
	}
}
