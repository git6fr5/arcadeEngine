// Standard Modules.
#include <iostream>

// Rendering Modules.
#define SDL_MAIN_HANDLED
#include <SDL.h>
// #include <SDL_image.h>

// Engine Modules.
#include "engine.h"
// #include "engine/physics.h"
#include "engine/visual.h"
#include "engine/audio.h"

using namespace std;
using namespace Engine;

// Controls the objects and processes of a single entity.
class Engine::Entity {
	public:
		// Entity ID.
		int ID;
		// The collision shape data.
		Engine::Visual::Joint m_Root;
		// The motion data.
		Engine::Physics::Motion m_Motion;

		void Update(float deltaTime) {
			m_Root.Update(deltaTime);
			m_Motion.Update(deltaTime);
		}
};

class Engine::Scene {
	public:
		// The rendering paramaters.
		int m_screenWidth = int(1680 / 2);
		int m_screenHeight = int(920 / 2);
		float m_TimeScale = 0.005;
		// Flags.
		bool isRunning = false;

		// Runs the scene.
		void Run() {
			isRunning = true;

			RunRenderer();
			
			Uint32 ticks = SDL_GetTicks();
			while (isRunning) {
				ticks = Update(ticks);
				// cout << ticks << "\n";
			}
		}

		void AddEntity(Engine::Entity entity) {
			m_Entities[m_EntityCount] = entity;
			m_EntityCount++;
		}

	private:
		// The entities in the scene.
		Engine::Entity m_Entities[2048];
		int m_EntityCount = 0;

		SDL_Renderer* renderer;
		SDL_Window* window;

		// Updates the scene.
		Uint32 Update(Uint32 previousTicks) {

			Uint32 currentTicks = SDL_GetTicks();
			float deltaTime = (currentTicks - previousTicks) * m_TimeScale;

			for (int i = 0; i < m_EntityCount; i++) {
				m_Entities[i].Update(deltaTime);
			}

			cout << m_Entities[0].m_Motion.m_Position.ToString() << '\n';

			UpdateRenderer();

			return currentTicks;
		}

		void RunRenderer() {
			// initializes the window
			SDL_Init(SDL_INIT_EVERYTHING);
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
			window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_screenWidth, m_screenHeight, SDL_WINDOW_SHOWN);
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		}

		void UpdateRenderer() {
			// clears the frame
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(renderer);

			for (int i = 0; i < m_EntityCount; i++) {
				RenderEntity(m_Entities[i]);
			}

			// renders the frame
			SDL_RenderPresent(renderer);
		}

		void RenderEntity(Engine::Entity entity) {

			SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);

			Engine::Physics::Vector screenPosition = Engine::Physics::Vector{ 
				entity.m_Motion.m_Position.x + (float)m_screenWidth / 2, 
				(float)m_screenHeight / 2 - entity.m_Motion.m_Position.y 
			};

			SDL_Point* points = entity.m_Root.VerticesToPoints(screenPosition);
			for (int i = 0; i < entity.m_Root.m_VertexCount; i = i + 2) {
				SDL_Point line[] = { points[i], points[i + 1] };
				SDL_RenderDrawLines(renderer, line, 2);
			}

			return;

		}
};