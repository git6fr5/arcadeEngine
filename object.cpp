#pragma once
#include <limits>
#include <iostream>
#include <SDL.h>

#include <C:\Users\git6f\arcadeEngine\arcade.cpp>
#include <C:\Users\git6f\arcadeEngine\physics.cpp>

using namespace std;
using namespace Arcade;

class Arcade::Object {

	public:
		Shape shape;
		Motion motion;

		void onUpdate(float timeInterval) {
			// updates the motion of the object
			// with respect to time passed since last update
			motion.onUpdate(timeInterval);

			// reset collision force
			motion.removeForce(motion.collisionIndex);

			// damp the input force
			// motion.forces[motion.processIndex] = motion.forces[motion.processIndex].scalarTransform(0.975);
			motion.removeForce(motion.processIndex);
		}

		void onCollision(Object objectB, float timeInterval, bool isFirst = true) {

			// get the direction of the force
			Vector2 negativePosition = motion.position.scalarTransform(-1);
			Vector2 objectBCenter = objectB.motion.position.translate(negativePosition);
			Vector2 forceDirection = objectBCenter.scalarTransform(-1).normalize();

			// check that the boundaries are set properly
			if (objectBCenter.magnitude() <= (shape.length + objectB.shape.length)/2) {
				float diff = ((shape.length + objectB.shape.length)/2 - objectBCenter.magnitude()) / 2;
				motion.deltaPosition = forceDirection.normalize().scalarTransform(diff);
				// cout << "Adjusting boundary" << "\n";
			}

			// get the relative velocities
			Vector2 negativeVelocity = motion.velocity.scalarTransform(-1);
			Vector2 objectBRelativeVelocity = objectB.motion.velocity.translate(negativeVelocity);
			
			// get the velocity in the direction of the force
			Vector2 velocityProjection = objectBRelativeVelocity.project(forceDirection);
			
			// calculate the impulse, accounting for elasticity
			Vector2 collisionForce = velocityProjection.scalarTransform(objectB.motion.mass / timeInterval);
			collisionForce = collisionForce.scalarTransform(motion.elasticity);

			// add the collision force
			// cout << "collision \n"; // this seems to affect the actual gameplay?

			motion.setForce(motion.collisionIndex, collisionForce);

		}

		// checks if the shape of this object intersects with the shape of the given object
		bool checkCollision(Object objectB, float timeInterval) {

			bool isColliding = false;
			if (shape.isSolid == false || objectB.shape.isSolid == false) {
				return isColliding;
			}
			if (shape.isCircle && objectB.shape.isCircle) {
				isColliding = circularCollision(objectB);
			}
			else {
				isColliding = polygonCollision(objectB);
			}
			
			if (isColliding) {
				onCollision(objectB, timeInterval);
			}
			return isColliding;
		}

		bool polygonCollision(Object objectB) {
			// prep the shapes normals
			shape.calculateAllNormals();
			objectB.shape.calculateAllNormals();

			// get the position of the center of the other object, relative to the center of this object
			// in order to make sense of the physical space between the two objects
			Vector2 negativePosition = motion.position.scalarTransform(-1);
			Vector2 objectBCenter = objectB.motion.position.translate(negativePosition);

			// translate other objects shape into relative space
			Vector2 objectBRelativePolgygon[50];
			for (int i = 0; i < objectB.shape.polyCount; i++) {
				objectBRelativePolgygon[i] = objectB.shape.polygon[i].translate(objectBCenter);
			}
			// note : we don't need to set the last polygon to the first because we aren't rendering these

			// run through the projection of the polygon onto each normal and check for overlaps
			// if even one of them does not overlap, then there is no collision

			bool isSeperated = true;
			int gapCount = 0;
			for (int i = 0; i < shape.polyCount; i++) {
				// check overlap of projected line onto normal as axis
				isSeperated = axisCollision(objectBRelativePolgygon, objectB.shape.polyCount, shape.normals[i]);
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

			return true;
		}

		// checks if the shapes collide along a given axis
		bool axisCollision(Vector2* polygonB, int polyCountB, Vector2 axis) {

			// for shape 1
			Vector2 setA = getMinMax(shape.polygon, shape.polyCount, axis);

			// for shape 2
			Vector2 setB = getMinMax(polygonB, polyCountB, axis);

			// if min of B greater max of A then there is a gap
			// if min of A greater than max of B then there is a gap
			return (setB.x > setA.y) || (setA.x > setB.y);
		}

		Vector2 getMinMax(Vector2* polygon, int polyCount, Vector2 axis) {

			// prep the variables
			float max = -numeric_limits<float>::infinity();
			float min = numeric_limits<float>::infinity();

			for (int i = 0; i < polyCount; i++) {

				// dot product gets a scalar representation of the point
				// in the direction of the axis
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

		// a simple circular check for collisions
		bool circularCollision(Object objectB) {
			float distance = motion.position.magnitude(objectB.motion.position);
			if (distance < (shape.length + objectB.shape.length) / 2) {
				return true;
			}
			return false;
		}

		bool checkBounds(float horizontalBound, float verticalBound) {
			float boundaryHit = false;
			if (motion.position.x > horizontalBound - shape.length / 2) {
				motion.position.x = horizontalBound - shape.length / 2;
				motion.velocity.x = -motion.velocity.x;
				boundaryHit = true;
			}
			else if (motion.position.x < shape.length / 2) {
				motion.position.x = shape.length / 2;
				motion.velocity.x = -motion.velocity.x;
				boundaryHit = true;
			}
			if (motion.position.y > verticalBound - shape.length / 2) {
				motion.position.y = verticalBound - shape.length / 2;
				motion.velocity.y = -motion.velocity.y;
				boundaryHit = true;
			}
			else if (motion.position.y < shape.length / 2) {
				motion.position.y = shape.length / 2;
				motion.velocity.y = -motion.velocity.y;
				boundaryHit = true;
			}
			if (boundaryHit) {
				// how does this work
				motion.velocity = motion.velocity.scalarTransform(motion.elasticity);
			}
			if (motion.velocity.magnitude() > 500) {
				motion.velocity = motion.velocity.normalize().scalarTransform(500);
			}
			return boundaryHit;
		}

};