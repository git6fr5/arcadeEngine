// Compile with g++ visual.cpp -o visual -lSDL2 // -lSDL2_image

// Standard Modules.
#include <iostream>
#include <math.h>
using namespace std;

// Engine Modules.
#include "../visual.h"
using namespace Engine::Visual;

int main(int argc, char** argv) {

	Joint spine = Skeleton::Spine();
	spine.PrintHierarchy();
	int vertexCount = 0;
	vertexCount = spine.UpdateVertices(spine.m_Vertices, &vertexCount);

	int jointCount = 0;
	string jointNames[4096];
	jointCount = spine.FlattenHierarchy(jointNames, &jointCount);

	for (int i = 0; i < vertexCount; i++) {
		cout << jointNames[i] << " : " << spine.m_Vertices[i].ToString() << "\n";
	}

	return 0;
};