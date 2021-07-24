#pragma once
#include <limits>
#include <iostream>

#include <C:\Users\git6f\arcadeEngine\arcade.cpp>
#include <C:\Users\git6f\arcadeEngine\physics.cpp>

using namespace std;
using namespace Arcade;

class Arcade::Object {

	public:
		Shape shape;
		Motion motion;
		bool isStatic = false;

		void onUpdate(float timeInterval) {
			// updates the motion of the object
			// with respect to time passed since last update
			motion.onUpdate(timeInterval);

			// collision energy dissipation
			motion.forces[1] = Vector2{ 0,0 };
		}

		void onCollision(Object objectB, float timeInterval) {
			// causes some cool behavior
			/*Vector2 negativePosition = motion.position.scalarTransform(-1);
			Vector2 otherObjectCenter = objectB.motion.position.translate(negativePosition);
			motion.setForce(1, otherObjectCenter.scalarTransform(100));*/

			/*Vector2 otherObjectCenter = objectB.motion.position.translate(motion.position);
			motion.setForce(1, otherObjectCenter.scalarTransform(-1));*/

			Vector2 negativePosition = motion.position.scalarTransform(-1);
			Vector2 objectBCenter = objectB.motion.position.translate(negativePosition);
			Vector2 forceDirection = objectBCenter.scalarTransform(-1).normalize();

			if (objectBCenter.magnitude() < (shape.length + objectB.shape.length)/2) {
				float diff = ((shape.length + objectB.shape.length)/2 - objectBCenter.magnitude());
				motion.position.translate(forceDirection.normalize().scalarTransform(diff));
			}

			// force = impulse = m * a
			// do i need to account for angle of collision?
			// yes, its the velocity in the direction of the collision
			// therefore

			/*Vector2 accelerationProjection = objectB.motion.acceleration.project(forceDirection);
			// force seems to be a bit too intense :(
			Vector2 collisionForce = accelerationProjection.scalarTransform(objectB.motion.mass);*/

			// THE VELOCITY IN THE FRAME OF THIS OBJECT IS THE RELATIVE VELOCITY?
			Vector2 negativeVelocity = motion.velocity.scalarTransform(-1);
			Vector2 objectBRelativeVelocity = negativeVelocity.translate(negativeVelocity);

			Vector2 velocityProjection = objectBRelativeVelocity.project(forceDirection);
			Vector2 collisionForce = velocityProjection.scalarTransform(objectB.motion.mass / timeInterval);

			cout << collisionForce.toString() << "\n";

			// f = m * -v' / t
			// new v =  v + (f / m) * t
			// new v = v + (-v' * m / t * m) * t
			// new v = v - v'

			// Vector2 collisionMomentum = velocityProjection.scalarTransform(objectB.motion.mass);

			// force seems to be a bit too intense :(
			// Vector2 collisionForce = velocityProjection.scalarTransform(objectB.motion.mass);

			// force applied = m * delta v / t
			// what is the delta v???

			// add it to the current collision force
			// Vector2 currentCollisionForce = motion.forces[1];
			motion.setForce(1, collisionForce);
			//motion.onUpdate(timeInterval);

			// motion.velocity = motion.velocity.translate(collisionMomentum.scalarTransform(1 / motion.mass));

		}

		bool checkCollision(Object objectB, float timeInterval) {
			// checks if the shapes of this object
			// intersects with the shape of the given object

			//circularCollision(otherObject);

			// prep the shapes
			shape.calculateAllNormals();
			objectB.shape.calculateAllNormals();

			// get the position of the center of the other object, relative to the center of this object
			// need for projecting onto the normals
			// we need some sense of the space between the two
			Vector2 negativePosition = motion.position.scalarTransform(-1);
			Vector2 objectBCenter = objectB.motion.position.translate(negativePosition);

			// translate other objects polygon points to relative space
			Vector2 objectBRelativePolgygon[50];
			//Vector2 otherObjectRelativeNormals[50]; // not sure if i need to do this

			for (int i = 0; i < objectB.shape.polyCount; i++) {
				objectBRelativePolgygon[i] = objectB.shape.polygon[i].translate(objectBCenter);
				//otherObjectRelativeNormals[i] = otherObject.shape.normals[i].translate(otherObjectCenter);
			}
			// note : we don't need to set the last polygon to the first because we aren't rendering these

			// run through the projection of the polygon onto each normal and check for overlaps
			// if even one of them does not overlap, then there is no collision (i think)
			bool isSeperated = true;
			int gapCount = 0;
			for (int i = 0; i < shape.polyCount; i++) {
				// check overlap of projected line onto normal as axis
				isSeperated = axisCollision(objectBRelativePolgygon, objectB.shape.polyCount,  shape.normals[i]);
				if (isSeperated) {
					return false;
					gapCount++;
				}
			}

			for (int i = 0; i < objectB.shape.polyCount; i++) {
				// check overlap of projected line onto normal as axis
				isSeperated = axisCollision(objectBRelativePolgygon, objectB.shape.polyCount, objectB.shape.normals[i]);
				if (isSeperated) {
					return false;
					//gapCount++;
				}
			}
			
			//cout << "GAP COUNT:" << gapCount << "\n";
			// if the function hasn't returned by here, it means there was a collision
			onCollision(objectB, timeInterval);

			return true;
		}

		bool checkBounds(float horizontalBound, float verticalBound) {
			float boundaryHit = false;
			if (motion.position.x > horizontalBound - shape.length /2 ) {
				motion.position.x = horizontalBound - shape.length /2;
				boundaryHit = true;
			}
			else if (motion.position.x < shape.length/2) {
				motion.position.x = shape.length/2;
				boundaryHit = true;
			}
			if (motion.position.y > verticalBound - shape.length/2) {
				motion.position.y = verticalBound - shape.length /2;
				boundaryHit = true;
			}
			else if (motion.position.y < shape.length / 2) {
				motion.position.y = shape.length / 2;
				boundaryHit = true;
			}
			if (boundaryHit) {
				motion.velocity.scalarTransform(-1);
			}
			if (motion.velocity.magnitude() > 300) {
				motion.velocity = motion.velocity.normalize().scalarTransform(300);
			}
			return boundaryHit;
		}

		bool axisCollision(Vector2* polygonB, int polyCountB, Vector2 axis) {
			// get the projected points of this shape on to the normal

			// for shape 1
			Vector2 setA = getMinMax(shape.polygon, shape.polyCount, axis);

			// for shape 2
			Vector2 setB = getMinMax(polygonB, polyCountB, axis);

			// if min of B greater max of A then there is a gap
			// if min of A greater than max of B then there is a gap
			// cout << "MIN MAXES: " << setA.toString() + ", " << setB.toString() << "\n";
			// cout << setB.x << ">" << setA.y << "          " << setA.x << ">" << setB.y;
			// cout << (setB.x > setA.y) << (setA.x > setB.y) << "\n";
			return (setB.x > setA.y) || (setA.x > setB.y);
		}

		Vector2 getMinMax(Vector2* polygon, int polyCount, Vector2 axis) {

			// cout << "poly count: " << polyCount << "\n";
			float max = -numeric_limits<float>::infinity();
			float min = numeric_limits<float>::infinity();

			for (int i = 0; i < polyCount; i++) {

				// dot products reduces a projected polygon to a scalar
				// skips getting an independent projection vector
				// that accounts for direction!

				float val = polygon[i].dot(axis);
				if (val > max) {
					max = val;
				}
				if (val < min) {
					min = val;
				}
			}

			return Vector2{ min, max };
		}

		bool circularCollision(Object otherObject) {

			float distance = motion.position.magnitude(otherObject.motion.position);
			if (distance < shape.length + otherObject.shape.length) {
				return true;
			}
			return false;
		}

};