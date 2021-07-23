#pragma once
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

		bool checkCollision(Object otherObject) {
			// checks if the shapes of this object
			// intersects with the shape of the given object

			circularCollision(otherObject);

			// prep the shapes
			shape.calculateAllNormals();
			otherObject.shape.calculateAllNormals();

			// get the position of the center of the other object, relative to the center of this object
			Vector2 negativePosition = motion.position.scalarTransform(-1);
			Vector2 otherObjectCenter = otherObject.motion.position.translate(negativePosition);

			// translate other objects polygon points to relative space
			Vector2 otherObjectRelativePolygon[50];
			Vector2 otherObjectRelativeNormals[50];

			for (int i = 0; i < otherObject.shape.polyCount; i++) {
				otherObjectRelativePolygon[i] = otherObject.shape.polygon[i].translate(otherObjectCenter);
				otherObjectRelativeNormals[i] = otherObject.shape.normals[i].translate(otherObjectCenter);
			}
			// note : we don't need to set the last polygon to the first because we aren't rendering these

			// run through the projection of the polygon onto each normal and check for overlaps
			// if even one of them does not overlap, then there is no collision (i think)

			for (int i = 0; i < shape.polyCount; i++) {
				// get the projected points of this shape on to the normal
				/*Vector2 projectedNormal = normals[i];
				for (int j = 0; j < shape.polyCount; j++) {
					
				}*/
			}

			for (int i = 0; i < otherObject.shape.polyCount; i++) {
				// check overlap of projected line onto normal

			}

			return false;
		}

		bool circularCollision(Object otherObject) {

			float distance = motion.position.magnitude(otherObject.motion.position);
			if (distance < shape.length + otherObject.shape.length) {
				return true;
			}
			return false;
		}

};