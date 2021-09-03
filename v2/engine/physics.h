#include "../engine.h"

using namespace std;
using namespace Engine;

class Physics::Vector {
	public:
		float x = 0;
		float y = 0;

		// Operator overloads.

		// Addition increment operator
		Vector& operator +=(const Vector& vector) {                           
			x = x + vector.x;
			y = y + vector.y;
			return *this;
		}

		// Addition operator
		friend Vector operator +(Vector vectorA, const Vector& vectorB) {
			vectorA += vectorB;
			return vectorA;
		}

		// Addition operator
		Vector& operator *=(const float scalar) {
			x *= scalar;
			y *= scalar;
			return *this;
		}

		// Addition operator
		friend Vector operator *(Vector vector, const float scalar) {
			vector *= scalar;
			return vector;
		}

		string ToString() {
			return "(" + to_string(x).substr(0, 4) + ", " + to_string(y).substr(0, 4) + ")";
		}

};

class Physics::Motion {
 
	public:
		// Vector components.
		Physics::Vector m_Position;
		Physics::Vector m_Velocity;
		Physics::Vector m_Acceleration;

		// Updates the motion based on time.
		void Update(float deltaTime) {
			UpdateAcceleration(deltaTime);
			UpdateVelocity(deltaTime);
			UpdatePosition(deltaTime);
		}

	private:
		// Updates the acceleration based on time.
		void UpdateAcceleration(float deltaTime) {
			m_Acceleration = Vector{ 0, -1 };
		};

		// Updates the velocity based on time.
		void UpdateVelocity(float deltaTime) {
			Physics::Vector deltaVelocity = m_Acceleration * deltaTime;
			m_Velocity += deltaVelocity;
		};

		// Updates the position based on time.
		void UpdatePosition(float deltaTime) {
			Physics::Vector deltaPosition = m_Velocity * deltaTime;
			m_Position += deltaPosition;
		};


};