#pragma once
#define _USE_MATH_DEFINES

#include <iostream>
#include <SDL.h>
#include <math.h>
#include <C:\Users\git6f\arcadeEngine\arcade.cpp>
#include <C:\Users\git6f\arcadeEngine\object.cpp>

using namespace std;
using namespace Arcade;

typedef Control::Process Process;
typedef Control::Output Output;

class Control::Output {

	public:

		// variableOfType = (type*) pointerToObjectofType
		// pointerToObjectofType = &(variableOfType)

		// grab the variable at the passed pointer
		void forceUP(Object* objectPointer, float intensity) {
			// cout << "Processed: W";
			Vector2 upForce = Vector2{ 0, -intensity };
			force(&(*objectPointer), upForce);
		}

		void forceRIGHT(Object* objectPointer, float intensity) {
			// cout << "Processed: A";
			Vector2 rightForce = Vector2{ -intensity, 0 };
			force(&(*objectPointer), rightForce);
		}

		void forceDOWN(Object* objectPointer, float intensity) {
			// cout << "Processed: S";
			Vector2 downForce = Vector2{ 0, intensity };
			force(&(*objectPointer), downForce);
		}

		void forceLEFT(Object* objectPointer, float intensity) {
			// cout << "Processed: D";
			Vector2 leftForce = Vector2{ intensity, 0 };
			force(&(*objectPointer), leftForce);
		}

		void force(Object* objectPointer, Vector2 direction) {
			int processIndex = (*objectPointer).motion.processIndex;

			Vector2 currentVelocity = (*objectPointer).motion.velocity;
			Vector2 newVelocity = Vector2{ currentVelocity.x + direction.x, currentVelocity.y + direction.y };

			(*objectPointer).motion.setVelocity(newVelocity);
		}

};

class Control::Process {

	public:
		bool isProcessing = false;
		Output output = Output{};
		Object* objectPointers[50];
		int objectCount = 0;

		// grabs the variable at the address passed in
		int add(Object* objectPointer) {
			// saves the address of this variable
			objectPointers[objectCount] = &(*objectPointer);
			objectCount++;
			return objectCount;
		}

		// note : always process with respect to time interval in order to
		// keep game play consistent
		void processContInput(const Uint8* keystate, float timeInterval) {

			//continuous-response keys
			float intensity = 750 * timeInterval;
			if (keystate[SDL_SCANCODE_W]) {
				// cout << "hello";
				output.forceUP(objectPointers[0], intensity);
			}
			if (keystate[SDL_SCANCODE_A]) {
				output.forceRIGHT(objectPointers[0], intensity);
			}
			if (keystate[SDL_SCANCODE_S]) {
				output.forceDOWN(objectPointers[0], intensity);
			}
			if (keystate[SDL_SCANCODE_D]) {
				output.forceLEFT(objectPointers[0], intensity);

			};
		}

};