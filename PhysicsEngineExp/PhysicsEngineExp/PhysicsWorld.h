#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "PhysicsObjects.h"

using namespace std;
using namespace glm;


struct Solver {

	vec3 gravity = vec3(0.0f, -2.0f, 0.0f);

	void update(float dt, vector<VerletObject>* objList, vector<Link>* linkList, Constraint* constPtr) {

		//if you want to substep, you can call these functiosn multiple times each frame or whatever.
			//but you can also decrease dt I think.
		int subStep = 1; //2 or 4 or 8
		float subDt = 1.0 * dt / subStep;
		for (int i = 0; i < subStep; i++) {
			applyGravity(objList);
			applyConstraint(objList, constPtr);
			applyCollisions(objList);
			applyLinks(linkList);
			updatePositions(dt, objList);
		}

	}

	void updatePositions(float dt, vector<VerletObject>* objList) {
		//will loop through the verlect objects and .updatePosition(dt)
		for (auto itr = (*objList).begin(); itr != (*objList).end(); ++itr)
			(*itr).updatePosition(dt);
	}

	void applyGravity(vector<VerletObject>* objList) {
		//will loop through the verlet objects and .setAcc(gravity)
		for (auto itr = (*objList).begin(); itr != (*objList).end(); ++itr)
			(*itr).updateAcc(gravity);
	}

	void applyConstraint(vector<VerletObject>* objList, Constraint *constPtr) {

		//**********This is kinda of important.
			//But there is a different between a Constraint and and Object to collide with. 
			//A constraint acts like a container, that is why we check if distMag > boundScale - objScale
			//However, an object or a wall just collides with.
			//ALSO, since a constrain acts and ensures that no objects gets out of them, you will have to modify the code if you want multiple constraints at the same time.


		//constraint position in world coordinates
		for (auto itr = (*objList).begin(); itr != (*objList).end(); ++itr) {
			//Check if its not satisfied and fix accordingly. by looping through these constraints.

			vec3 objPos = (*itr).curPosition;
			float objScale = (*itr).scale;
			
			vec3 boundPos = constPtr->pos;
			if (constPtr->type == 0) {
				//-------------------Circle Constraint

			

				Circle *circleObj;
				circleObj = (Circle*)(constPtr);
				float boundScale = circleObj->radius; //radius and scale in our case are the same thing
				//cout << boundScale << endl;
				vec3 distVec = objPos - boundPos;  //bound here means constraint
				float distMag = length(distVec);
				if (distMag > (boundScale - objScale)) { 
					vec3 n = normalize(distVec);
					(*itr).curPosition = boundPos + n * (boundScale - objScale);
				}
			} else if (constPtr->type == 1) {
				//---------------------Sqaure constraint
				/*
					Some LOGIC HERE .....
					Square* squareObj;
					squareObj = (Square*)(itr2); //pivote is at the center.
					
				*/
			
			}

		}
	}


	



	void applyCollisions(vector<VerletObject>* objList) {


		//we will loop O(n^2) through the objects and check if they overlap.
		//if they overlap then we move them along the collision axis.
		int count = (*objList).size();
		for (int i = 0; i < count; i++) {

		

			for (int j = (i + 1); j < count; j++) {

				if (((*objList)[i].type == circle) & ((*objList)[j].type == circle)) { 	//Circle V Circle Collision Detection

					circleVcircle( &(*objList)[i], &(*objList)[j]);

				
				} else if (((*objList)[i].type == circle) & ((*objList)[j].type == square)) { //Circle V Square Collision
					
					circleVsquare(&(*objList)[i], &(*objList)[j]);

				} else if (((*objList)[i].type == square) & ((*objList)[j].type == circle)) { //Square V suqare

					circleVsquare(&(*objList)[j], &(*objList)[i]);

				} else if (((*objList)[i].type == square) & ((*objList)[j].type == square)) {

					squareVsquare(&(*objList)[i], &(*objList)[j]);

				}
			}
		}

	}

	void applyLinks(vector<Link>* linkList) {
		//will loop through the verlect objects and .updatePosition(dt)
		for (auto itr = (*linkList).begin(); itr != (*linkList).end(); ++itr)
			(*itr).apply();
	}


	void circleVcircle(VerletObject* A, VerletObject* B) {

		vec3 collisionAxis = A->curPosition - B->curPosition; //Vector AB = B-A
		float dist = length(collisionAxis);
		float bound = A->scale + B->scale;
		if (dist < bound) {
			vec3 n = normalize(collisionAxis);
			float delta = (bound)-dist;
			//we will move each object half delta so that overall, they are delta away from each other and so not overlapping anymore
			A->curPosition += 0.5f * delta * n;
			B->curPosition -= 0.5f * delta * n;
		}
	}

	void circleVsquare(VerletObject* A, VerletObject* B) {
		
		//you check if its in the range.
				//get distance away along xaxis
				//get distance vector from each other
				//if distance along axis is <= width + radius then collide.
		
		vec3 IObjPos = A->curPosition;
		float IobjScale = A->scale;

		float dW = B->width / 2.0;  //width is along x axis
		float dL = B->length / 2.0; //length is along y axis
		vec3 JObjPos = B->curPosition;

		//If the circle object is in the range of the wall in y coordiantes,
		//Then if the distance between object and wall is less than dW + radius there is collision.
		if ((IObjPos.y < (JObjPos.y + dL + IobjScale)) & (IObjPos.y > (JObjPos.y - dL - IobjScale))) {
			vec3 collisionAxis = IObjPos - JObjPos;
			float dist = abs(JObjPos.y - IObjPos.y);
			float bound = dW + IobjScale;
			if (dist < bound) {

				vec3 n = normalize(collisionAxis);
				float delta = (bound)-dist;
				//we will move each object half delta so that overall, they are delta away from each other and so not overlapping anymore
				A->curPosition += 0.5f * delta * n;
				B->curPosition -= 0.5f * delta * n;
			}
		} else if ((IObjPos.x < (JObjPos.x + dW + IobjScale)) & (IObjPos.x > (JObjPos.x - dW - IobjScale))) {
			vec3 collisionAxis = IObjPos - JObjPos;
			float dist = abs(JObjPos.x - IObjPos.x);
			float bound = dL + IobjScale;
			if (dist < bound) {
				vec3 n = normalize(collisionAxis);
				float delta = (bound)-dist;
				//we will move each object half delta so that overall, they are delta away from each other and so not overlapping anymore
				A->curPosition += 0.5f * delta * n;
				B->curPosition -= 0.5f * delta * n;
			}
		}

	}

	void squareVsquare(VerletObject* A, VerletObject* B) {


		vec3 IObjPos = A->curPosition;
		float IdW = A->width / 2.0;  //width is along x axis
		float IdL = A->length / 2.0; //length is along y axis

		vec3 JObjPos = B->curPosition;
		float JdW = B->width / 2.0;  //width is along x axis
		float JdL = B->length / 2.0; //length is along y axis
	

		//If the circle object is in the range of the wall in y coordiantes,
		//Then if the distance between object and wall is less than dW + radius there is collision.
		if ((IObjPos.y < (JObjPos.y + JdL + IdL)) & (IObjPos.y > (JObjPos.y - JdL - IdL))) {
			vec3 collisionAxis = IObjPos - JObjPos;
			float dist = abs(JObjPos.y - IObjPos.y);
			float bound = JdW + IdW;
			if (dist < bound) {

				vec3 n = normalize(collisionAxis);
				float delta = (bound)-dist;
				//we will move each object half delta so that overall, they are delta away from each other and so not overlapping anymore
				A->curPosition += 0.5f * delta * n;
				B->curPosition -= 0.5f * delta * n;
			}
		} else if ((IObjPos.x < (JObjPos.x + JdW + IdW)) & (IObjPos.x > (JObjPos.x - JdW - IdW))) {
			vec3 collisionAxis = IObjPos - JObjPos;
			float dist = abs(JObjPos.x - IObjPos.x);
			float bound = JdL + IdL;
			if (dist < bound) {
				vec3 n = normalize(collisionAxis);
				float delta = (bound)-dist;
				//we will move each object half delta so that overall, they are delta away from each other and so not overlapping anymore
				A->curPosition += 0.5f * delta * n;
				B->curPosition -= 0.5f * delta * n;
			}
		}

	}

};

class PhysicsWorld {

public:
	 vector<VerletObject> objectList;
	 vector<Link> linkList; //links list
	 //a new type of object for constraint objects.
	 Constraint *constPtr;
	 Circle cicleConst;
	 Square squareConst;
	 Solver mySolver;

public:
	 void addObject(VerletObject obj) {
		objectList.push_back(obj);
	}

	 void addLink(int a, int b, int dist) {
		 if ( (a < objectList.size()) && (b < objectList.size())) {
			 Link tempLink(&(objectList[a]), &(objectList[b]), dist);
			 linkList.push_back(tempLink);
		 } else {
			 cout << "ERROR!" << endl;
			 return;
		 }
		
	 }

	 void addConst(vec3 pos, float radius) {
		
		Circle temp(radius, pos);
		cicleConst = temp;
		constPtr = &cicleConst;
	 }

	 void addConst(vec3 pos, int width, int length) {

		Square temp(width, length, pos);
		squareConst = temp;
		constPtr = &squareConst;
	 }

	 void removeObject(VerletObject obj) {
		//could  be done by id, loop through vector and use erase?!
	}

	 void updatePos(float dt) {
		mySolver.update(dt, &objectList, &linkList, constPtr);
	}


};

#endif