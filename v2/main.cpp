// Compile with g++ main.cpp -o main -lSDL2 // -lSDL2_image

// Standard Modules.
#include <iostream>
#include <math.h>
using namespace std;

// Engine Modules.
#include "engine.cpp"
using namespace Engine;


int main(int argc, char** argv) {

	Engine::Entity entity = Engine::Entity{ 0, Engine::Visual::Skeleton::Spine(), Engine::Physics::Motion{} };
	Engine::Scene scene = Engine::Scene{};
	scene.AddEntity(entity);
	scene.Run();

	return 0;
};