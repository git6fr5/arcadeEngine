#pragma once
#define _USE_MATH_DEFINES

#include <iostream>
#include <SDL.h>
#include <math.h>
#include <C:\Users\git6f\arcadeEngine\arcade.cpp>
#include <C:\Users\git6f\arcadeEngine\object.cpp>

using namespace std;
using namespace Arcade;

typedef Control::Processor Processor;
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

class Control::Processor {

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

		virtual void runProcesses(const Uint8* keystate, float timeInterval) {
			//
			cout << "hello" << "\n";
		}
};

class Player : public Processor {

	// note : always process with respect to time interval in order to
	// keep game play consistent
	public:
		virtual void runProcesses(const Uint8* keystate, float timeInterval) override {

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

class Attractor : public Processor {

	public:
		// note : always process with respect to time interval in order to
		// keep game play consistent

		void runProcesses(const Uint8* keystate, float timeInterval) override {

			float attraction = 0.3;
			float totalMass = 0;
			for (int i = 0; i < objectCount; i++) {
				totalMass = totalMass + (*objectPointers[i]).motion.mass;
			}
			if (totalMass > 0) {
				attraction = attraction / totalMass;
			}

			for (int i = 0; i < objectCount; i++) {

				Object* attractorPointer = objectPointers[i];
				for (int j = 0; j < objectCount; j++) {

					if (i != j) {
						Object* attracteePointer = objectPointers[j];

						// get the direction of the force
						Vector2 negativePosition = (*attracteePointer).motion.position.scalarTransform(-1);
						Vector2 attractorCenter = (*attractorPointer).motion.position.translate(negativePosition);
						Vector2 forceDirection = attractorCenter.normalize();
						float mass = (*objectPointers[i]).motion.mass;

						if (attractorCenter.magnitude() < attraction) {
							// forceDirection.scalarTransform(-1);
							attractorCenter.scalarTransform(0);
						}


						int processIndex = (*attracteePointer).motion.processIndex;
						Vector2 processedForce = (*attracteePointer).motion.forces[processIndex];
						float factor = pow((attractorCenter.magnitude() / 100) ,2);
						Vector2 newForce = processedForce.translate(forceDirection.normalize().scalarTransform(factor * mass * attraction / timeInterval));

						(*attracteePointer).motion.setForce(processIndex, newForce);
					}
					

				}
			}

		} 

};

class Orbiter : public Processor {

	public:
		// note : always process with respect to time interval in order to
		// keep game play consistent

		void runProcesses(const Uint8* keystate, float timeInterval) override {


			Object* attractorPointer = objectPointers[0];
			float mass = (*attractorPointer).motion.mass;

			for (int j = 1; j < objectCount; j++) {

				Object* attracteePointer = objectPointers[j];

				// get the direction of the force
				Vector2 negativePosition = (*attracteePointer).motion.position.scalarTransform(-1);
				Vector2 attractorCenter = (*attractorPointer).motion.position.translate(negativePosition);
				Vector2 forceDirection = attractorCenter.normalize();

				if (attractorCenter.magnitude() < (*attractorPointer).shape.length) {
					// forceDirection.scalarTransform(-1);
					attractorCenter.scalarTransform(-1);
				}


				int processIndex = (*attracteePointer).motion.processIndex;
				Vector2 processedForce = (*attracteePointer).motion.forces[processIndex];
				float factor = pow((attractorCenter.magnitude() / 100) ,2);
				Vector2 newForce = processedForce.translate(forceDirection.normalize().scalarTransform(factor * mass / timeInterval));

				(*attracteePointer).motion.setForce(processIndex, newForce);
					
			}

		} 

};

class Trailer : public Processor {

	public:
		// note : always process with respect to time interval in order to
		// keep game play consistent

		void runProcesses(const Uint8* keystate, float timeInterval) override {

			for (int j = 1; j < objectCount; j++) {

				Object* attractorPointer = objectPointers[j-1];
				float mass = (*attractorPointer).motion.mass;

				Object* attracteePointer = objectPointers[j];

				// get the direction of the force
				Vector2 negativePosition = (*attracteePointer).motion.position.scalarTransform(-1);
				Vector2 attractorCenter = (*attractorPointer).motion.position.translate(negativePosition);
				Vector2 forceDirection = attractorCenter.normalize();

				if (attractorCenter.magnitude() < (*attractorPointer).shape.length) {
					// forceDirection.scalarTransform(-1);
					attractorCenter.scalarTransform(-1);
				}


				int processIndex = (*attracteePointer).motion.processIndex;
				Vector2 processedForce = (*attracteePointer).motion.forces[processIndex];
				float factor = attractorCenter.magnitude();
				Vector2 newForce = processedForce.translate(forceDirection.normalize().scalarTransform(factor / timeInterval));

				(*attracteePointer).motion.setForce(processIndex, newForce);
					
			}

		} 

};