//#pragma once
// compile with g++ scene.cpp -o scene -lSDL2
#define SDL_MAIN_HANDLED
#include <iostream>
#include <math.h>
#include <SDL.h>
#include <C:\Users\git6f\arcadeEngine\arcade.cpp>
#include <C:\Users\git6f\arcadeEngine\physics.cpp>
#include <C:\Users\git6f\arcadeEngine\control.cpp>
#include <C:\Users\git6f\arcadeEngine\object.cpp>

using namespace std;
using namespace Arcade;

int ScreenWidth = int(1680 / 2);
int ScreenHeight = int(920 / 2);

class Arcade::Scene {

	public:
		bool isRunning;
		Object objects[100];
		int objectCount = 0;
		Vector2 gravity{ 0, 100 };
		SDL_Renderer* renderer;
		int gravityEnum = 0;
		bool _renderNormals = false;
		bool _renderMotion = false;
		Process processor = Process{};

		void run() {

			isRunning = true;

			// initializes the window
			SDL_Init(SDL_INIT_EVERYTHING);
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
			SDL_Window* window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN);
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

			// initializes the time parameters
			Uint32 startTime = SDL_GetTicks();
			Uint32 previousTime = SDL_GetTicks();
			Uint32 currentTime = previousTime;
			float timeInterval = 0;
			float totalTime = 0;

			// tracks the events
			SDL_Event event;
			// pass the address of the first object
			processor.add(&objects[0]);
			processor.isProcessing = true;

			if (gravityEnum == 0) {
				setGravity();
			}
			else if (gravityEnum == 1) {
				setAltGravity();
			}

			while (isRunning) {
				// updates the times
				previousTime = currentTime;
				currentTime = SDL_GetTicks();
				timeInterval = (currentTime - previousTime) / 1000.0;
				totalTime = totalTime + timeInterval;

				while (SDL_PollEvent(&event)) {
					if (event.type == SDL_QUIT) {
						isRunning = false;
						processor.isProcessing = false;
					}
				}

				const Uint8* keystate = SDL_GetKeyboardState(NULL);

				// do we need to update if timeInterval == 0?
				if (timeInterval != 0) {
					if (gravityEnum == 2) {
						setCentralGravity();
					}
					processor.processContInput(keystate, timeInterval);
					onUpdate(timeInterval);
					checkCollisions(timeInterval);
				}

				renderFrame();

			}
		}

		void onUpdate(float timeInterval) {

			// updates all the objects in the scene
			// with respect to the time passed since last update
			for (int i = 0; i < objectCount; i++) {
				objects[i].onUpdate(timeInterval);
			}
		}

		void checkCollisions(float timeInterval) {
			// iterates through each pair of objects
			// to check for collisions between them
			for (int i = 0; i < objectCount; i++) {
				for (int j = 0; j < objectCount; j++) {
					if (i != j) {
						bool collided = objects[i].checkCollision(objects[j], timeInterval);
						if (collided) {
							objects[i].shape.color[0] = 0xFF;
						}
						else {
							objects[i].shape.color[0] = 0x00;
						}
					}
				}
			}

			for (int i = 0; i < objectCount; i++) {
				objects[i].checkBounds(ScreenWidth, ScreenHeight);
			}
		}

		void renderFrame() {
			// clears the frame
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(renderer);

			// add each object to the frame
			for (int i = 0; i < objectCount; i++) {
				renderPolygon(objects[i]);
				if (_renderNormals) {
					renderNormals(objects[i]);
				}
				if (_renderMotion) {
					renderMotion(objects[i]);
				}
			}

			// renders the frame
			SDL_RenderPresent(renderer);
		}

		void renderPolygon(Object object) {

			// set the color
			unsigned char* color = object.shape.color;
			SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);

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

		void setGravity() {
			// add gravity to all the initialized objects
			for (int i = 0; i < objectCount; i++) {
				objects[i].motion.setForce(objects[i].motion.gravityIndex, gravity.scalarTransform(objects[i].motion.mass));
			}
		}

		void setAltGravity() {
			// add alternating gravity to all the initialized objects
			for (int i = 0; i < objectCount; i++) {
				if (i % 2 == 0) {
					objects[i].motion.setForce(objects[i].motion.gravityIndex, gravity.scalarTransform(objects[i].motion.mass * 1)); // sin(M_PI / 2 + totalTime)
				}
				else {
					objects[i].motion.setForce(objects[i].motion.gravityIndex, gravity.scalarTransform(objects[i].motion.mass * -1)); // sin(M_PI / 2 + totalTime)
				}
			}
		}

		void setCentralGravity() {
			for (int i = 0; i < objectCount; i++) {
				Vector2 centralForce = objects[i].motion.position.scalarTransform(-1).translate(Vector2{ float(ScreenWidth / 2), float(ScreenHeight / 2) }).scalarTransform(gravity.magnitude());
				objects[i].motion.setForce(objects[i].motion.gravityIndex, centralForce);
			}
		}

		Object createSquareObject(
			float length = 10, 
			float mass = 1, float elasticity = 1,
			Vector2 position = Vector2{}, Vector2 velocity = Vector2{}, 
			bool isDynamic = true, bool isSolid = true) {
			// creates an object with the given shape and motion
			// don't know why I can't pass the polygon through here
			Object object;

			// create the shape
			Shape shape { length, 0, isSolid };
			object.shape = shape;
			object.shape.createSquare();
			object.shape.calculateAllNormals();

			// create its motion
			Motion motion{ mass, elasticity, position, velocity, Vector2{}, isDynamic };
			object.motion = motion;

			// add it to the scene
			objects[objectCount] = object;
			objectCount++;

			// print to terminal
			cout << object.shape.toString();

			return object;
		}

		Object createCircleObject(
			float length = 10, 
			float mass = 1, float elasticity = 1,
			Vector2 position = Vector2{}, Vector2 velocity = Vector2{}, 
			bool isDynamic = true, bool isSolid = true) {
			// creates an object with the given shape and motion
			// don't know why I can't pass the polygon through here
			Object object;

			// create the shape
			Shape shape{ length, 0, isSolid };
			object.shape = shape;
			object.shape.createCircle();
			object.shape.calculateAllNormals();

			// create its motion
			Motion motion{ mass, elasticity, position, velocity, Vector2{}, isDynamic };
			object.motion = motion;

			// add it to the scene
			objects[objectCount] = object;
			objectCount++;

			return object;
		}

		Object createPolygonObject(int polyCount, float length = 10, 
			float mass = 1, float elasticity = 1,
			Vector2 position = Vector2{}, Vector2 velocity = Vector2{}, 
			bool isDynamic = true, bool isSolid = true) {
			// creates an object with the given shape and motion
			// don't know why I can't pass the polygon through here
			Object object;

			// create the shape
			Shape shape{ length, 0, isSolid };
			object.shape = shape;
			object.shape.createRegularPolygon(polyCount);
			object.shape.calculateAllNormals();

			// create its motion
			Motion motion{ mass, elasticity, position, velocity, Vector2{}, isDynamic };
			object.motion = motion;

			// add it to the scene
			objects[objectCount] = object;
			objectCount++;

			return object;
		}

		Object createRectangleObject(
			float length = 20, float breadth = 10,
			float mass = 1, float elasticity = 1,
			Vector2 position = Vector2{}, Vector2 velocity = Vector2{},
			bool isDynamic = true, bool isSolid = true) {

			// creates an object with the given shape and motion
			// don't know why I can't pass the polygon through here
			Object object;

			// create the shape
			Shape shape{ length, breadth, isSolid };
			object.shape = shape;
			object.shape.createRectangle();
			object.shape.calculateAllNormals();

			// create its motion
			Motion motion{ mass, elasticity, position, velocity, Vector2{}, isDynamic };
			object.motion = motion;

			// add it to the scene
			objects[objectCount] = object;
			objectCount++;

			return object;
		}
};

Scene twoBalls(Scene scene, int gravityEnum, float gravityMagnitude, bool _renderNormals, bool _renderMotion) {
	float size = 50;
	scene.createCircleObject(size, 5, 1.0,
		Vector2{ float(ScreenWidth / 2) , float(ScreenHeight / 2) - 150 },
		Vector2{ 0, 50 }, true, true);
	scene.createCircleObject(size, 1, 1.0,
		Vector2{ float(ScreenWidth / 2) , float(ScreenHeight / 2) + 150 },
		Vector2{ 0, -50 }, true, true);
	scene._renderNormals = _renderNormals;
	scene._renderMotion = _renderMotion;
	scene.gravity = scene.gravity.normalize().scalarTransform(gravityMagnitude);
	scene.gravityEnum = gravityEnum;
	return scene;
};

Scene twoShapes(Scene scene, int gravityEnum, float gravityMagnitude, bool _renderNormals, bool _renderMotion) {
	float size = 50;
	scene.createCircleObject(size, 1, 1.0,
		Vector2{ float(ScreenWidth / 2) , float(ScreenHeight / 2) - 150 },
		Vector2{ 0, 50 }, true, true);
	scene.createPolygonObject(6, size, 1, 1.0,
		Vector2{ float(ScreenWidth / 2) - 70, float(ScreenHeight / 2) - 150 },
		Vector2{ 0, 50 }, true, true);
	scene._renderNormals = _renderNormals;
	scene._renderMotion = _renderMotion;
	scene.gravity = scene.gravity.normalize().scalarTransform(gravityMagnitude);
	scene.gravityEnum = gravityEnum;
	return scene;
};

Scene fivePolyons(Scene scene, int gravityEnum, float gravityMagnitude, bool _renderNormals, bool _renderMotion) {
	float size = 50;
	scene.createPolygonObject(6, size, 1, 1.0,
		Vector2{ float(ScreenWidth / 2) - 70, float(ScreenHeight / 2) - 150 },
		Vector2{ 0, 50 }, true, true);
	scene.createPolygonObject(6, size, 1, 1.0,
		Vector2{ float(ScreenWidth / 2) + 20, float(ScreenHeight / 2) + 150 },
		Vector2{ 0, 50 }, true, true);
	scene.createPolygonObject(6, size, 1, 1.0,
		Vector2{ float(ScreenWidth / 2) + 30, float(ScreenHeight / 2) - 150 },
		Vector2{ 0, 50 }, true, true);
	scene.createPolygonObject(6, size, 1, 1.0,
		Vector2{ float(ScreenWidth / 2) + 100, float(ScreenHeight / 2) + 150 },
		Vector2{ 0, 50 }, true, true);
	scene.createPolygonObject(6, size, 1, 1.0,
		Vector2{ float(ScreenWidth / 2) + 200, float(ScreenHeight / 2) - 150 },
		Vector2{ 0, 50 }, true, true);
	scene._renderNormals = _renderNormals;
	scene._renderMotion = _renderMotion;
	scene.gravity = scene.gravity.normalize().scalarTransform(gravityMagnitude);
	scene.gravityEnum = gravityEnum;
	return scene;
};

Scene empty(Scene scene, int gravityEnum, float gravityMagnitude, bool _renderNormals, bool _renderMotion) {
	scene._renderNormals = _renderNormals;
	scene._renderMotion = _renderMotion;
	scene.gravity = scene.gravity.normalize().scalarTransform(gravityMagnitude);
	scene.gravityEnum = gravityEnum;
	return scene;
};

Scene basic(Scene scene, int gravityEnum, float gravityMagnitude, bool _renderNormals, bool _renderMotion) {
	scene.createCircleObject(50, 1, 0.5,
		Vector2{ float(ScreenWidth / 2), float(ScreenHeight / 2) },
		Vector2{ 0, 0 }, true, true);
	scene._renderNormals = _renderNormals;
	scene._renderMotion = _renderMotion;
	scene.gravity = scene.gravity.normalize().scalarTransform(gravityMagnitude);
	scene.gravityEnum = gravityEnum;
	return scene;
};

Scene fiftyBalls(Scene scene, int gravityEnum, float gravityMagnitude, bool _renderNormals, bool _renderMotion) {
	float size = 50;
	for (int i = 0; i < 50; i++) {
		scene.createCircleObject(size, 1, 0.5,
			Vector2{ float(rand() % ScreenWidth), float(ScreenHeight / 2) - 75 + float(rand() % 150) },
			Vector2{ 0, 50 }, true, true);
	}
	scene._renderNormals = _renderNormals;
	scene._renderMotion = _renderMotion;
	scene.gravity = scene.gravity.normalize().scalarTransform(gravityMagnitude);
	scene.gravityEnum = gravityEnum;
	return scene;
};

Scene fiftyPolyons(Scene scene, int gravityEnum, float gravityMagnitude, bool _renderNormals, bool _renderMotion) {
	float size = 50;
	for (int i = 0; i < 20; i++) {
		scene.createPolygonObject(6, size, 1, 0.5,
			Vector2{ float(rand() % ScreenWidth), float(ScreenHeight / 2) - 75 + float(rand() % 150) },
			Vector2{ 0, 50 }, true, true);
	}
	scene._renderNormals = _renderNormals;
	scene._renderMotion = _renderMotion;
	scene.gravity = scene.gravity.normalize().scalarTransform(gravityMagnitude);
	scene.gravityEnum = gravityEnum;
	return scene;
};

Scene polygonScene(Scene scene, int gravityEnum, float gravityMagnitude, bool _renderNormals, bool _renderMotion) {
	for (int i = 0; i < 99; i++) {
		float size = 5 + float(rand() % 15);
		scene.createPolygonObject(3 + rand() % 45, size, size / 10, 0.3,
			Vector2{ float(rand() % ScreenWidth) , float(rand() % ScreenHeight) },
			Vector2{ float(rand() % 200), float(rand() % 200) }, true, true);
	}
	scene._renderNormals = _renderNormals;
	scene._renderMotion = _renderMotion;
	scene.gravity = scene.gravity.normalize().scalarTransform(gravityMagnitude);
	scene.gravityEnum = gravityEnum;
	return scene;
};

Scene circularScene(Scene scene, int gravityEnum, float gravityMagnitude, bool _renderNormals, bool _renderMotion) {
	for (int i = 0; i < 75; i++) {
		float size = 5 + float(rand() % 15);
		scene.createCircleObject(size, size / 10, 0.1,
			Vector2{ float(rand() % ScreenWidth) , float(rand() % ScreenHeight) },
			Vector2{ float(rand() % 200), float(rand() % 200) }, true, true);
	}
	scene._renderNormals = _renderNormals;
	scene._renderMotion = _renderMotion;
	scene.gravity = scene.gravity.normalize().scalarTransform(gravityMagnitude);
	scene.gravityEnum = gravityEnum;
	return scene;
};


int main(int argc, char** argv) {

	Scene scene;

	// scene = basic(scene, 1, 0, false, false);
	// scene = twoBalls(scene, 0, 100, false, true);
	// scene = twoShapes(scene, 0, 100, false, true);

	// scene = fivePolyons(scene, 0, 1000, false, false);
	// scene = fiftyBalls(scene, 1, 100, false, false);
	// scene = fiftyPolyons(scene, 0, 100, false, false);
	// scene = polygonScene(scene, 2, 100, false, false);
	scene = circularScene(scene, 2, 5, false, false); // the singularity boss
	
	cout << scene.objectCount << "\n";

	scene.run();

	return 0;
};