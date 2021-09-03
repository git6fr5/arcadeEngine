// Standard Modules.
using namespace std;

// The overarching engine architecture.
namespace Engine {

	// The engine components. 
	namespace Physics {}
	namespace Visual {}
	namespace Audio {}

	// The engine functionality.
	class Entity;
	class Scene;
}

// Controls the physics of the entities.
namespace Engine::Physics {

	class Vector;
	class Motion;

}

// The visual rendering of entities.
namespace Engine::Visual {

	class Animator;
	class Animation;
	class Skeleton;
	class Joint;
	class Particle;

}

// The auditory rendering of entities.
namespace Engine::Audio {

}

