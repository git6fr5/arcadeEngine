//#pragma once
// compile with g++ scene.cpp -o scene -lSDL2
#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL.h>
#include <C:\Users\git6f\arcadeEngine\arcade.cpp>
#include <C:\Users\git6f\arcadeEngine\physics.cpp>
#include <C:\Users\git6f\arcadeEngine\object.cpp>

using namespace std;
using namespace Arcade;

int ScreenWidth = 640;
int ScreenHeight = 480;

class Arcade::Scene {

	public:
		bool isRunning;
		Object objects[50];
		int objectCount = 0;
		Vector2 gravity{ 0, 0 };
		SDL_Renderer* renderer;

		void run() {

			isRunning = true;

			// initializes the window
			SDL_Init(SDL_INIT_EVERYTHING);
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
			SDL_Window* window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN);
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

			// initializes the time parameters
			Uint32 previousTime = SDL_GetTicks();
			Uint32 currentTime = previousTime;
			float timeInterval = 0;

			// add gravity to all the initialized objects
			for (int i = 0; i < objectCount; i++) {
				objects[i].motion.addForce(gravity.scalarTransform(objects[i].motion.mass));
			}

			// tracks the events
			SDL_Event event;

			while (isRunning)
			{
				// updates the times
				previousTime = currentTime;
				currentTime = SDL_GetTicks();
				timeInterval = (currentTime - previousTime) / 1000.0;

				while (SDL_PollEvent(&event))
				{
					if (event.type == SDL_QUIT)
						isRunning = false;
				}

				// do we need to update if timeInterval == 0?
				onUpdate(timeInterval);

			}
		}

		void onUpdate(float timeInterval) {

			// updates all the objects in the scene
			// with respect to the time passed since last update
			for (int i = 0; i < objectCount; i++) {
				objects[i].onUpdate(timeInterval);
			}

			// iterates through each pair of objects
			// to check for collisions between them
			for (int i = 0; i < objectCount; i++) {
				for (int j = 0; j < objectCount; j++) {
					if (i != j) {
						objects[i].checkCollision(objects[j]);
					}
				}
			}

			// clears the frame
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(renderer);

			// add each object to the frame
			for (int i = 0; i < objectCount; i++) {
				renderPolygon(objects[i]);
				renderNormals(objects[i]);
				renderMotion(objects[i]);
			}

			// renders the frame
			SDL_RenderPresent(renderer);
		}

		void renderPolygon(Object object) {

			// set the color
			SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);

			// draw the shape
			SDL_RenderDrawLines(renderer, object.shape.polygonToPoints(object.motion.position), object.shape.polyCount + 1);

			return;
		}

		void renderNormals(Object object) {

			// set the color
			SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);

			// get the normals
			SDL_Point* normalsPoints;
			normalsPoints = object.shape.normalsToPoints(object.motion.position);

			// draw each normal
			for (int i = 0; i < object.shape.polyCount; i++) {

				SDL_Point* _normalPoints[2];
				_normalPoints[0] = &normalsPoints[2 * i];
				_normalPoints[1] = &normalsPoints[2 * i + 1];

				SDL_RenderDrawLines(renderer, *_normalPoints, 2);

			}

			return;
		}

		void renderMotion(Object object) {

			// set the color
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);

			// get the normals
			SDL_Point position[2];
			position[0] = { 0, 0 };
			position[1] = { (int)object.motion.position.x, (int)object.motion.position.y };
			SDL_RenderDrawLines(renderer, position, 2);

			SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0xFF, 0xFF);

			SDL_Point velocity[2];
			velocity[0] = position[1];
			velocity[1] = { (int)object.motion.velocity.x + (int)object.motion.position.x, (int)object.motion.velocity.y + (int)object.motion.position.y };
			SDL_RenderDrawLines(renderer, velocity, 2);

			SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0xFF);

			SDL_Point acceleration[2];
			acceleration[0] = velocity[1];
			acceleration[1] = { 
				(int)object.motion.acceleration.x + (int)object.motion.velocity.x + (int)object.motion.position.x, 
				(int)object.motion.acceleration.y + (int)object.motion.velocity.y + (int)object.motion.position.y };
			SDL_RenderDrawLines(renderer, acceleration, 2);

			return;
		}

		Object createSquareObject(float length = 10, float mass = 1, float bounciness = 0, Vector2 position = Vector2{}, Vector2 velocity = Vector2{}, bool isDynamic = true) {
			// creates an object with the given shape and motion
			// don't know why I can't pass the polygon through here
			Object object;

			// create the shape
			Shape shape { length };
			object.shape = shape;
			object.shape.createSquare();
			object.shape.calculateAllNormals();

			// create its motion
			Motion motion{ mass, bounciness, position, velocity, Vector2{}, isDynamic };
			object.motion = motion;

			// add it to the scene
			objects[objectCount] = object;
			objectCount++;

			// print to terminal
			cout << object.shape.toString();

			return object;
		}

		Object createCircleObject(float length = 10, float mass = 1, float bounciness = 0, Vector2 position = Vector2{}, Vector2 velocity = Vector2{}, bool isDynamic = true) {
			// creates an object with the given shape and motion
			// don't know why I can't pass the polygon through here
			Object object;

			// create the shape
			Shape shape{ length };
			object.shape = shape;
			object.shape.createCircle();
			object.shape.calculateAllNormals();

			// create its motion
			Motion motion{ mass, bounciness, position, velocity, Vector2{}, isDynamic };
			object.motion = motion;

			// add it to the scene
			objects[objectCount] = object;
			objectCount++;

			return object;
		}

		Object createPolygonObject(int polyCount, float length = 10, float mass = 1, float bounciness = 0, Vector2 position = Vector2{}, Vector2 velocity = Vector2{}, bool isDynamic = true) {
			// creates an object with the given shape and motion
			// don't know why I can't pass the polygon through here
			Object object;

			// create the shape
			Shape shape{ length };
			object.shape = shape;
			object.shape.createRegularPolygon(polyCount);
			object.shape.calculateAllNormals();

			// create its motion
			Motion motion{ mass, bounciness, position, velocity, Vector2{}, isDynamic };
			object.motion = motion;

			// add it to the scene
			objects[objectCount] = object;
			objectCount++;

			return object;
		}

		Object createRectangleObject(float length = 20, float breadth = 10, float mass = 1, float bounciness = 0, Vector2 position = Vector2{}, Vector2 velocity = Vector2{}, bool isDynamic = true) {
			// creates an object with the given shape and motion
			// don't know why I can't pass the polygon through here
			Object object;

			// create the shape
			Shape shape{ length, breadth };
			object.shape = shape;
			object.shape.createRectangle();
			object.shape.calculateAllNormals();

			// create its motion
			Motion motion{ mass, bounciness, position, velocity, Vector2{}, isDynamic };
			object.motion = motion;

			// add it to the scene
			objects[objectCount] = object;
			objectCount++;

			return object;
		}
};

int main() {

	Scene scene;
	scene.createSquareObject(50, 1, 0, Vector2{ float(ScreenWidth)/2, 50 }, Vector2{}, true);
	// scene.createSquareObject(100, 1, 0, Vector2{ float(ScreenWidth) / 2, 400 }, Vector2{}, false);

	scene.createCircleObject(50, 1, 0, Vector2{ float(ScreenWidth)/2 + 100, 50 }, Vector2{}, true);
	scene.createPolygonObject(7, 50, 1, 0, Vector2{ float(ScreenWidth) / 2 + 250, 50 }, Vector2{}, true);
	// scene.createRectangleObject(50, 100, 1, 0, Vector2{ float(ScreenWidth) / 2 - 250, 50 }, Vector2{}, false);

	scene.createRectangleObject(20, ScreenWidth * 2, 1, 0, Vector2{ float(ScreenWidth) / 2, 450 }, Vector2{}, false);

	cout << scene.objectCount << "\n";

	scene.run();

	return 0;
};