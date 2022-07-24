#ifndef PHYSICSOBJECTS_H
#define PHYSICSOBJECTS_H


enum shape {
	circle,
	square,
	triangle
};

struct VerletObject {

	shape type;

	vec3 curPosition;
	vec3 prevPosition;
	vec3 acceleration;
	float scale; //scale
	float width; //along x-axis
	float length; //along y-axis
	vec3 color;

	VerletObject(vec3 startPos, float startScale, vec3 startAcc) {
		type = circle;
		curPosition = startPos;
		prevPosition = (curPosition - vec3(0.0f, 0.01f, 0.0f));
		scale = startScale;
		float r = 1.0 * rand() / RAND_MAX;
		float g = 1.0 * rand() / RAND_MAX;
		float b = 1.0 * rand() / RAND_MAX;
		//cout << r << g << b << endl;
		color = vec3(r, g, b);		//random
		acceleration = startAcc;
	}

	VerletObject(vec3 startPos, float startWidth, float startLength ,vec3 startAcc) {
		scale = startWidth; //for the limited time being, we will assume we only deal with sqaures, where the scale = length = width
		type = square;
		curPosition = startPos;
		prevPosition = (curPosition - vec3(0.0f, 0.01f, 0.0f));
		width = startWidth;
		length = startLength;
		float r = 1.0 * rand() / RAND_MAX;
		float g = 1.0 * rand() / RAND_MAX;
		float b = 1.0 * rand() / RAND_MAX;
		//cout << r << g << b << endl;
		color = vec3(r, g, b);		//random
		acceleration = startAcc;
	}

	void updatePosition(float dt) {
		vec3 temp = curPosition;
		//verlet integration
		//cout << "before: " << curPosition.x << ", " << curPosition.y << endl;
		curPosition = (2.0f * curPosition) - prevPosition + (acceleration * dt * dt);
		//cout << "after: " << curPosition.x << ", " << curPosition.y << endl;

		prevPosition = temp;
		//reset acceleration?!
		acceleration = vec3(0.0f);
	}

	void updateAcc(vec3 acc) {
		acceleration += acc;
	}

};

//chains
struct Link {
	VerletObject* obj1;
	VerletObject* obj2;
	float linkDist; //link distance

	Link(VerletObject* obj1, VerletObject* obj2, float linkDist) {
		this->obj1 = obj1;
		this->obj2 = obj2;
		this->linkDist = linkDist;
	}



	void apply() {
		vec3 axis = (*obj1).curPosition - (*obj2).curPosition;
		float dist = length(axis);
		vec3 n = normalize(axis);
		float delta = linkDist - dist;
		obj1->curPosition += 0.5f * delta * n;
		obj2->curPosition -= 0.5f * delta * n;
	}

};


struct Constraint {

	int type; //circle or Square
	vec3 pos; //position of the constraint in world coordinates.

};

struct Circle : Constraint {

	float radius; //scale or radius of the circle constraint
	
	Circle() {
		type = 0;
	}

	Circle(float r, vec3 p) {
		type = 0;
		radius = r;
		pos = p;
	}

/*	Circle(const Circle& C) {
		type = C.type;
		pos = C.pos;
		radius = C.radius;
	}*/

};

struct Square : Constraint {

	float width; //is along x axis
	float length; //is along y axis

	Square() {
		type = 1;
	}

	Square(float w, float l, vec3 p) {
		type = 1;
		width = w;
		length = l;
		pos = p;
	}

};


#endif // !1
