#pragma once
#define _USE_MATH_DEFINES

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include <C:\Users\git6f\arcadeEngine\arcade.cpp>
#include <C:\Users\git6f\arcadeEngine\control.cpp>
#include <C:\Users\git6f\arcadeEngine\object.cpp>

using namespace std;
using namespace Arcade;

typedef Visual::Sprite Sprite;
typedef Visual::Animator Animator;
typedef Visual::Particle Particle;


// what we're going to do is
// have the sprite collect all the processor information
// and then render it from there?
// e.g.

class Visual::Sprite {

	public:
		Object* objectPointer;
		SDL_Renderer* rendererPointer;
		string spritePath;
		SDL_Rect rect;
		const char* path;
		SDL_Surface* surface;
		SDL_Texture* texture;


		void initializeSprite() {

			path = spritePath.c_str();;
			rect.w = (*objectPointer).shape.length;
			rect.h = (*objectPointer).shape.length;
			cout << spritePath << "\n";
			
		}

		void drawSprite() {

			/*const char* path = spritePath.c_str();;
			SDL_Surface* surface = SDL_LoadBMP(path);
			SDL_Texture* returnTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
			SDL_FreeSurface(tempSurface);*/

			rect.x = (*objectPointer).motion.position.x - (*objectPointer).shape.length / 2;
			rect.y = (*objectPointer).motion.position.y - (*objectPointer).shape.length / 2;

			surface = IMG_Load(path);
			texture = SDL_CreateTextureFromSurface(rendererPointer, surface);

			// cout << "rendering " << rect.w << ", " << rect.h << ", " << rect.x << ", " << rect.y << ", " << spritePath << "\n";
			

			SDL_RenderCopy(rendererPointer, texture, NULL, &rect); // destination rect

		}


};

class Arcade::Character {

	public:
		Processor* processorPointer;
		string mainSpritePath;
		string particleSpritePath;
		Sprite sprites[100];
	
		void initializeSprites(SDL_Renderer* renderer) {

			sprites[0] = Sprite{ (*processorPointer).objectPointers[0], renderer, mainSpritePath };
			sprites[0].initializeSprite();

			int objectCount = (*processorPointer).objectCount;
			for (int i = 1; i < objectCount; i++) {
				sprites[i] = Sprite{ (*processorPointer).objectPointers[i], renderer, particleSpritePath };
				sprites[i].initializeSprite();
			}

		}

		void render() {
			int objectCount = (*processorPointer).objectCount;
			for (int i = 0; i < objectCount; i++) {
				sprites[i].drawSprite();
			}
		}
	

	// main sprite
	// particle sprites

	// blah blah
	// store processor in here
	// store character in scene?

};