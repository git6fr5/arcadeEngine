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
		}

		bool checkCollision(Object objectB) {
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
			Vector2 otherObjectCenter = objectB.motion.position.translate(negativePosition);

			// translate other objects polygon points to relative space
			Vector2 objectBRelativePolgygon[50];
			//Vector2 otherObjectRelativeNormals[50]; // not sure if i need to do this

			for (int i = 0; i < objectB.shape.polyCount; i++) {
				objectBRelativePolgygon[i] = objectB.shape.polygon[i].translate(otherObjectCenter);
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

			return true;
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