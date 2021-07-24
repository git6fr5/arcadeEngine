#pragma once
#define _USE_MATH_DEFINES

#include <iostream>
#include <SDL.h>
#include <math.h>
#include <C:\Users\git6f\arcadeEngine\arcade.cpp>

using namespace std;
using namespace Arcade;

typedef Physics::Vector2 Vector2;
typedef Physics::Shape Shape;
typedef Physics::Motion Motion;

class Physics::Vector2 {

	public: 
		float x;
		float y;

		string toString() {
			// returns a readable output of this vector
			return "(" + to_string(x).substr(0, 4) + ", " + to_string(y).substr(0, 4) + ")";
		}


		float magnitude(Vector2 v = Vector2{}) {
			float xSeperation = x - v.x;
			float ySeperation = y - v.y;
			return sqrt(xSeperation * xSeperation + ySeperation * ySeperation);
		}

		float dot(Vector2 v) {
			return x * v.x + y * v.y;
		}

		Vector2 normalize() {
			float xNormalized = x / magnitude();
			float yNormalized = y / magnitude();
			return Vector2{ xNormalized, yNormalized };
		}

		Vector2 rotate(float angle) {
			angle = angle / 180 * M_PI;
			float xRotated = x * cos(angle) - y * sin(angle);
			float yRotated = x * sin(angle) + y * cos(angle);
			return Vector2{ xRotated, yRotated };
		}

		Vector2 project(Vector2 v) {
			float scalar = Vector2{ x, y }.dot(v);
			Vector2 normalizedV = v.normalize();
			return normalizedV.scalarTransform(scalar);
		}

		Vector2 translate(Vector2 v) {
			return Vector2{ x + v.x, y + v.y };
		}

		Vector2 scalarTransform(float scalar) {
			return Vector2{ x * scalar, y * scalar };
		}

		Vector2 stretch(Vector2 v) {
			return Vector2{ x * v.x, y * v.y };
		}

		bool checkOverlap(Vector2* polygon1, Vector2* polygon2) {
			// project each polygon onto the axis
			return false;
		}

};

class Physics::Shape {
	public:
		float length;
		float breadth;
		Vector2 polygon[50];
		Vector2 normals[50];
		SDL_Point points[50];
		SDL_Point normalPoints[100];
		int polyCount;
		unsigned char color[4] = { 0x00, 0xFF, 0x00, 0xFF };

		void calculateAllNormals() {
			for (int i = 0; i < polyCount; i++) {
				Vector2 negativePolygon = polygon[i + 1].scalarTransform(-1);
				Vector2 polygonFace = polygon[i].translate(negativePolygon);
				Vector2 polygonNormal = polygonFace.rotate(90);
				normals[i] = polygonNormal;
			}
		}

		void createSquare() {
			createRegularPolygon(4);
		}

		void createRectangle() {
			createSquare();
			stretchShape(Vector2{ breadth / length, 1 });
		}

		void createCircle() {
			createRegularPolygon(49);
		}

		int createRegularPolygon(int _polyCount) {
			// set the polygon count
			polyCount = _polyCount;

			// construct the polygon vectors
			for (int i = 0; i < polyCount; i++) {
				polygon[i] = Vector2{ length / 2, 0 }.rotate(360 * i / polyCount);
			}

			// set the last polygon back to the first so it renders correctly
			polygon[polyCount] = polygon[0];

			// rotate the shape to the correct orientation
			if (polyCount % 2 == 0) {
				rotateShape(45);
			}

			return polyCount;
		}

		void rotateShape(float angle) {
			for (int i = 0; i < polyCount + 1; i++) {
				polygon[i] = polygon[i].rotate(angle);
			}
		}

		void stretchShape(Vector2 stretchVector) {
			for (int i = 0; i < polyCount + 1; i++) {
				polygon[i] = polygon[i].stretch(stretchVector);
			}
		}

		SDL_Point* polygonToPoints(Vector2 center) {
			// SDL_Point points[polyCount];
			for (int i = 0; i < polyCount + 1; i++) {
				points[i] = { (int)polygon[i].x + (int)center.x, (int)polygon[i].y + (int)center.y};
			}
			return points;
		}

		SDL_Point* normalsToPoints(Vector2 center) {
			// SDL_Point points[polyCount];
			for (int i = 0; i < polyCount; i++) {

				// midpoint of the face
				// should maybe consider caching faces
				// (point 1 + point 2) / 2
				Vector2 normalOrigin = polygon[i].translate(polygon[i + 1]).scalarTransform(0.5);
				normalPoints[2*i] = { (int)normalOrigin.x + (int)center.x, (int)normalOrigin.y + (int)center.y };

				// extending out the normal with a length of 50
				Vector2 normalEnd = normals[i].normalize().scalarTransform(50);
				normalPoints[2*i + 1] = { (int)normalEnd.x + (int)center.x, (int)normalEnd.y + (int)center.y };

			}
			return normalPoints;
		}

		string toString() {
			string output = "Vertices: ";
			for (int i = 0; i < polyCount; i++) {
				output += polygon[i].toString() + ", ";
			}
			output += "\n";
			output += "Normals: ";
			for (int i = 0; i < polyCount; i++) {
				output += normals[i].toString() + ", ";
			}
			output += "\n";
			return output;
		}

};

class Physics::Motion {

	public:
		float mass = 1;
		float bounciness;
		Vector2 position;
		Vector2 velocity;
		Vector2 acceleration;
		bool isDynamic = true;
	// protected:
		Vector2 forces[50];
		int forceCount;

		Vector2 setPosition(Vector2 newPosition) {
			// sets the position of this motion
			position.x = newPosition.x;
			position.y = newPosition.y;
			return position;
		}

		Vector2 setVelocity(Vector2 newVelocity) {
			// sets the velocity of this motion
			velocity.x = newVelocity.x;
			velocity.y = newVelocity.y;
			return velocity;
		}

		int setForce(int index, Vector2 newForce) {
			// add a force to act on this motion
			if (index < forceCount) {
				forces[index] = newForce;
			}
			else {
				addForce(newForce);
			}
			return forceCount;
		}

		int addForce(Vector2 newForce) {
			// add a force to act on this motion
			forces[forceCount] = newForce;
			forceCount++;
			cout << "Adding a force" << "\n";
			cout << forceCount << "\n";
			return forceCount;
		}

		// check this works properly
		int removeForce(int forceIndex) {
			// remove a forcing acting on this motion
			for (int i = forceIndex; i < forceCount-1; i++) {
				forces[i] = forces[i+1];
			}
			forceCount = forceCount - 1;
			return forceCount;
		}

		void onUpdate(float timeInterval) {

			// don't move static objects with time
			if (!isDynamic) {
				return;
			}

			// updates the vectors of this motion
			// with respect to the time passed since last update
			updateAcceleration();
			updateVelocity(timeInterval);
			updatePosition(timeInterval);
		}

		string toString() {
			// returns a readable output of this motion
			return "Pos : " + position.toString() + ", || Vel : " + velocity.toString() + ", || Acc : " + acceleration.toString();
		}

	protected:
		Vector2 updatePosition(float timeInterval) {
			// updates the position of this motion
			// with respect to the time passed since last update
			position.x = position.x + timeInterval * velocity.x;
			position.y = position.y + timeInterval * velocity.y;
			return position;
		}

		Vector2 updateVelocity(float timeInterval) {
			// updates the velocity of this motion
			// with respect to the time passed since last update
			velocity.x = velocity.x + timeInterval * acceleration.x;
			velocity.y = velocity.y + timeInterval * acceleration.y;
			return velocity;
		}

		Vector2 updateAcceleration() {
			// account for drag
			// but basically
			acceleration = Vector2{ 0, 0 };
			for (int i = 0; i < forceCount; i++) {
				acceleration = acceleration.translate(forces[i].scalarTransform(1 / mass));
			}
			return acceleration;
		}

};
