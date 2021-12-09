#pragma once

#include <vector>

#include "3Dmaths.hpp"
#include "resources.hpp"

#define GJK_MAX_ITER 10
#define EPA_MAX_ITER 10

#define GJK_THRESHOLD 0.001
#define EPA_THRESHOLD 0.001

//AABB collider for broad checks
struct AABB{
	AABB(Vec3 min, Vec3 max);
	~AABB(){};

	static bool intersect(AABB& a, AABB& b);

	Vec3 min, max;
};

//Sphere collider for GJK / EPA
struct BoundingSphere{
	BoundingSphere(){};
	BoundingSphere(Vec3 center, float radius, float aspect);
	~BoundingSphere(){};
	
	Vec3 furthest(Vec3 direction);

	Vec3 center;
	float radius;
	float verticalAspect;
};

//Convex collider for GJK / EPA
struct BoundingConvex{
	BoundingConvex(){};
	BoundingConvex(Vec3* start, unsigned int numVertices);
	~BoundingConvex(){};

	Vec3 furthest(Vec3 direction);
	AABB createBox();

	Vec3* vertices;
	unsigned int numVertices;
};

//Physics mesh
struct PhysicsMesh{
	PhysicsMesh(){};	
	void init(const char* filename);
	~PhysicsMesh();	

	Uint32 numConvexes;
	Vec3* vertices = nullptr;
	Uint16* indices;
	BoundingConvex* convexes;
};

//GJK Simplex
struct Simplex{
	Simplex();
	~Simplex(){};

	void append(Vec3 point);
	void erase(unsigned int index);
	void organize(unsigned int a, unsigned int b, unsigned int c, unsigned int d);
	void reset();

	bool sameSide(Vec3 u, Vec3 v);
	bool expand(Vec3 point, Vec3& direction);
	bool lineCase(Vec3& direction);
	bool triangleCase(Vec3& direction);
	bool tetrahedronCase(Vec3& direction);

	Vec3* ptr();

	void print();

	private:
	unsigned int numVertices;

	Vec3 vertices[4];
};

//EPA Edge
struct Polyedge{
	Polyedge(){};
	Polyedge(unsigned int a, unsigned int b);
	~Polyedge(){};

	static bool isReverse(const Polyedge& a, const Polyedge& b);

	unsigned int a, b;
};

//EPA Face
struct Polyface{
	Polyface(){};
	Polyface(unsigned int a, unsigned int b, unsigned int c);
	~Polyface(){};

	Polyedge getEdge(unsigned int edgei);

	unsigned int a, b, c;
};

//EPA Polytope
struct Polytope{
	Polytope();
	~Polytope(){};

	void reset(Vec3* points);
	void faceNormal(unsigned int facei, Vec3& normal, float& distance);
	void closestFace(unsigned int& facei, Vec3& normal, float& distance);
	void expand(Vec3 point);

	private:
	std::vector<Vec3> vertices;
	std::vector<Polyface> faces;
};

//Collision detection & resolution
struct CollisionHandler{
	CollisionHandler(){};
	~CollisionHandler(){};

	template<typename T, typename U>
	Vec3 support(T& a, U& b, Vec3 direction){
		return a.furthest(direction) - b.furthest(direction * (-1));
	}
	//GJK
	template<typename T, typename U>
	bool gjk(T& a, U& b){
		simplex.reset();

		Vec3 direction(1, 0, 0);
		Vec3 newPoint = support(a, b, direction);
		simplex.append(newPoint);

		direction = newPoint * (-1);

		for(unsigned int i=0;i<GJK_MAX_ITER;i++){
			newPoint = support(a, b, direction);
			if(Vec3::dot(newPoint, direction) <= 0){return false;}

			if(simplex.expand(newPoint, direction)){
				return true;
			}
		}

		return false;
	}
	//EPA
	template<typename T, typename U>
	void epa(T& a, U& b, Vec3& normal, float& distance){
		polytope.reset(simplex.ptr());

		unsigned int facei;
		Vec3 norm;
		float dist;
		Vec3 newPoint;
		float dot;

		for(unsigned int i=0;i<EPA_MAX_ITER;i++){
			polytope.closestFace(facei, norm, dist);
			newPoint = support(a, b, norm);
			dot = Vec3::dot(norm, newPoint);
			if(dist - EPA_THRESHOLD < dot
				&& dot < dist + EPA_THRESHOLD){
				break;
			}
			polytope.expand(newPoint);
		}
		normal = norm * (-1);
		distance = dist;
	}

	private:
	Simplex simplex;
	Polytope polytope;
};
