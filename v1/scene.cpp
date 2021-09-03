//#pragma once
// compile with g++ scene.cpp -o scene -lSDL2 -lSDL2_image
#define SDL_MAIN_HANDLED
#include <iostream>
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>

#include <C:\Users\git6f\arcadeEngine\arcade.cpp>
#include <C:\Users\git6f\arcadeEngine\physics.cpp>
#include <C:\Users\git6f\arcadeEngine\control.cpp>
#include <C:\Users\git6f\arcadeEngine\visual.cpp>
#include <C:\Users\git6f\arcadeEngine\object.cpp>

using namespace std;
using namespace Arcade;

int k_screenWidth = int(1680 / 2);
int k_screenHeight = int(920 / 2);
string k_spritePath = "C:/Users/git6f/arcadeEngine/visual/sprites/";

class Arcade::Scene {

	public:
		bool _renderPolygons = false;
		bool _renderNormals = false;
		bool _renderMotion = false;
		Vector2 gravity = Vector2{ 0, 0 };
		bool isRunning;
		Object objects[500];
		int objectCount = 0;
		Character* characters[50];
		int characterCount = 0;

		SDL_Renderer* renderer;
		SDL_Window* window;

		SDL_Rect bkgRect;
		SDL_Surface* bkgSurface;
		SDL_Texture* bkgTexture;
		const char* bkgPath;


		void run() {

			isRunning = true;

			// initializes the window
			SDL_Init(SDL_INIT_EVERYTHING);
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
			window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, k_screenWidth, k_screenHeight, SDL_WINDOW_SHOWN);
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

			// initializes the time parameters
			Uint32 startTime = SDL_GetTicks();
			Uint32 previousTime = SDL_GetTicks();
			Uint32 currentTime = previousTime;
			float timeInterval = 0;
			float totalTime = 0;

			const Uint8* keystate = SDL_GetKeyboardState(NULL);

			bkgPath = (k_spritePath + "background.bmp").c_str();
			bkgRect.w = k_screenWidth;
			bkgRect.h = k_screenHeight;
			bkgRect.x = 0;
			bkgRect.y = 0;

			// tracks the events
			SDL_Event event;

			for (int i = 0; i < characterCount; i++) {
				(*characters[i]).initializeSprites(renderer);
			}

			cout << characterCount;



			while (isRunning) {
				// updates the times
				previousTime = currentTime;
				currentTime = SDL_GetTicks();
				timeInterval = (currentTime - previousTime) / 1000.0;
				totalTime = totalTime + timeInterval;

				while (SDL_PollEvent(&event)) {
					if (event.type == SDL_QUIT) {
						isRunning = false;
					}
				}

				keystate = SDL_GetKeyboardState(NULL);

				// do we need to update if timeInterval == 0?
				if (timeInterval != 0) {
					processObjects(keystate, timeInterval);
					onUpdate(timeInterval);
					checkCollisions(timeInterval);
				}

				renderFrame();

			}
		}

		void processObjects(const Uint8* keystate, float timeInterval) {
			for (int i = 0; i < characterCount; i++) {
				// cout << "processing";
				(*(*characters[i]).processorPointer).runProcesses(keystate, timeInterval); // runProcesses(keystate, timeInterval);
				// cout << "processing2";
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
				objects[i].checkBounds(k_screenWidth, k_screenHeight);
			}
		}

		void renderFrame() {
			// clears the frame
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(renderer);
			
			// renderBackground();
			// renderCharacters();

			// add each object to the frame
			if (_renderPolygons || _renderNormals || _renderMotion) {
				for (int i = 0; i < objectCount; i++) {
					if (_renderPolygons) {
						renderPolygon(objects[i]);
					}
					if (_renderNormals) {
						renderNormals(objects[i]);
					}
					if (_renderMotion) {
						renderMotion(objects[i]);
					}
				}
			};

			// renders the frame
			SDL_RenderPresent(renderer);
		}

		void renderCharacters() {
			for (int i = 0; i < characterCount; i++) {
				(*characters[i]).render();
			}
		}

		void renderBackground() {
			bkgSurface = IMG_Load(bkgPath);
			bkgTexture = SDL_CreateTextureFromSurface(renderer, bkgSurface);
			SDL_RenderCopy(renderer, bkgTexture, &bkgRect, &bkgRect); // destination rect
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

Player playerSpawner(Scene* scenePointer) {

	Player processor = Player{};

	float size = 25;
	Object objectA = (*scenePointer).createCircleObject(size, 5, 1.0,
		Vector2{ float(k_screenWidth / 2) , float(k_screenHeight / 2) - 150 },
		Vector2{ 0, 0 }, true, true);
	
	processor.add(&(*scenePointer).objects[(*scenePointer).objectCount - 1]);

	processor.isProcessing = true;

	return processor;
};

Attractor attractorSpawner(Scene* scenePointer) {

	Attractor processor = Attractor{};

	// create the objects
	float size = 50;
	Object objectA = (*scenePointer).createCircleObject(size, 1, 1.0,
		Vector2{ float(k_screenWidth / 2) , float(k_screenHeight / 2) - 150 },
		Vector2{ 0, 0 }, true, true);
	processor.add(&(*scenePointer).objects[(*scenePointer).objectCount-1]);

	Object objectB = (*scenePointer).createCircleObject(size, 1, 1.0,
		Vector2{ float(k_screenWidth / 2) , float(k_screenHeight / 2) + 150 },
		Vector2{ 0, 0 }, true, true);
	processor.add(&(*scenePointer).objects[(*scenePointer).objectCount - 1]);

	processor.isProcessing = true;

	return processor;
};

Attractor singularitySpawner(Scene* scenePointer) {

	Attractor processor = Attractor{};

	for (int i = 0; i < 50; i++) {
		float size = 5 + float(rand() % 15);
		(*scenePointer).createCircleObject(size, size / 10, 0.1,
			Vector2{ float(rand() % k_screenWidth) , float(rand() % k_screenHeight) },
			Vector2{ float(rand() % 200), float(rand() % 200) }, true, true);
		processor.add(&(*scenePointer).objects[(*scenePointer).objectCount - 1]);
	}

	processor.isProcessing = true;

	return processor;
};

Orbiter saturnSpawner(Scene* scenePointer) {

	Orbiter processor = Orbiter{};

	(*scenePointer).createCircleObject(30, 1, 0.9,
		Vector2{ float(rand() % k_screenWidth) , float(rand() % k_screenHeight) },
		Vector2{ float(rand() % 200), float(rand() % 200) }, true, true);
	processor.add(&(*scenePointer).objects[(*scenePointer).objectCount - 1]);

	for (int i = 0; i < 15; i++) {
		float size = 5 + float(rand() % 15);
		(*scenePointer).createCircleObject(size, size / 10, 0.1,
			Vector2{ float(rand() % k_screenWidth) , float(rand() % k_screenHeight) },
			Vector2{ float(rand() % 200), float(rand() % 200) }, true, true);
		processor.add(&(*scenePointer).objects[(*scenePointer).objectCount - 1]);
	}

	processor.isProcessing = true;

	return processor;
};

Orbiter ghostSaturnSpawner(Scene* scenePointer) {

	Orbiter processor = Orbiter{};

	(*scenePointer).createCircleObject(30, 1, 0.1,
		Vector2{ float(rand() % k_screenWidth) , float(rand() % k_screenHeight) },
		Vector2{ float(rand() % 200), float(rand() % 200) }, true, false);
	processor.add(&(*scenePointer).objects[(*scenePointer).objectCount - 1]);

	for (int i = 0; i < 15; i++) {
		float size = 5 + float(rand() % 15);
		(*scenePointer).createCircleObject(size, size / 10, 0.1,
			Vector2{ float(rand() % k_screenWidth) , float(rand() % k_screenHeight) },
			Vector2{ float(rand() % 200), float(rand() % 200) }, true, false);
		processor.add(&(*scenePointer).objects[(*scenePointer).objectCount - 1]);
	}

	processor.isProcessing = true;

	return processor;
};

Orbiter blobSaturnSpawner(Scene* scenePointer) {

	Orbiter processor = Orbiter{};

	(*scenePointer).createCircleObject(30, 1, 0.1,
		Vector2{ float(rand() % k_screenWidth) , float(rand() % k_screenHeight) },
		Vector2{ float(rand() % 200), float(rand() % 200) }, true, true);
	processor.add(&(*scenePointer).objects[(*scenePointer).objectCount - 1]);

	for (int i = 0; i < 15; i++) {
		float size = 5 + float(rand() % 15);
		(*scenePointer).createCircleObject(size, size / 10, 0.1,
			Vector2{ float(rand() % k_screenWidth) , float(rand() % k_screenHeight) },
			Vector2{ float(rand() % 200), float(rand() % 200) }, true, false);
		processor.add(&(*scenePointer).objects[(*scenePointer).objectCount - 1]);
	}

	processor.isProcessing = true;

	return processor;
};

Trailer snakeSpawner(Scene* scenePointer) {

	Trailer processor = Trailer{};

	(*scenePointer).createCircleObject(30, 1, 0.9,
		Vector2{ float(rand() % k_screenWidth) , float(rand() % k_screenHeight) },
		Vector2{ float(rand() % 200), float(rand() % 200) }, true, true);
	processor.add(&(*scenePointer).objects[(*scenePointer).objectCount - 1]);

	for (int i = 0; i < 15; i++) {
		float size = 5 + float(rand() % 15);
		(*scenePointer).createCircleObject(size, size / 10, 0.1,
			Vector2{ float(rand() % k_screenWidth) , float(rand() % k_screenHeight) },
			Vector2{ 0, 0 }, true, true);
		processor.add(&(*scenePointer).objects[(*scenePointer).objectCount - 1]);
	}

	processor.isProcessing = true;

	return processor;
};


int main(int argc, char** argv) {

	Scene scene{ true, false, false};

	Player playerProcessor = playerSpawner(&scene);
	Character player = Character{ &playerProcessor, k_spritePath + "playerSprite.bmp", k_spritePath + "trailSprite.bmp" };
	scene.characters[scene.characterCount] = &player;
	scene.characterCount++;

	/*Attractor attractor = attractorSpawner(&scene);
	scene.processors[scene.processorCount] = &attractor;
	scene.processorCount++;*/

	/*Attractor singularity = singularitySpawner(&scene);
	scene.processors[scene.processorCount] = &singularity;
	scene.processorCount++;*/

	Orbiter saturnProcessor = saturnSpawner(&scene);
	Character saturn = Character{ &saturnProcessor,  k_spritePath + "enemySprite.bmp", k_spritePath + "trailSprite.bmp" };
	scene.characters[scene.characterCount] = &saturn;
	scene.characterCount++;

	/*Orbiter ghostSaturnProcessor = ghostSaturnSpawner(&scene);
	Character ghostSaturn = Character{ &ghostSaturnProcessor,  k_spritePath + "enemySprite.bmp", k_spritePath + "trailSprite.bmp" };
	scene.characters[scene.characterCount] = &ghostSaturn;
	scene.characterCount++;*/

	/*Orbiter blobSaturn = blobSaturnSpawner(&scene);
	scene.processors[scene.processorCount] = &blobSaturn;
	scene.processorCount++;*/

	/*Trailer snakeProcessor = snakeSpawner(&scene);
	Character snake = Character{ &snakeProcessor,  k_spritePath + "enemySprite.bmp", k_spritePath + "trailSprite.bmp" };
	scene.characters[scene.characterCount] = &snake;
	scene.characterCount++;*/

	// have an enemy with a large central mass and little orbs orbiting him
	cout << scene.objectCount << "\n";

	scene.run();

	return 0;
};