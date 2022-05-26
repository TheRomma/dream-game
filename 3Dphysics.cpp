#include "3Dphysics.hpp"

//Simplex for use with the gjk algorithm.
Simplex::Simplex(){
	for(unsigned int i=0;i<4;i++){
		vertices[i] = Vec3(0, 0, 0);
	}
	numVertices = 0;
}

//Append a new point for the simplex. Shifts memory; index 0 is always the newest point.
void Simplex::append(Vec3 point){
	for(unsigned int i=3;i>0;i--){
		vertices[i] = vertices[i - 1];
	}
	vertices[0] = point;

	if(numVertices < 4){
		numVertices++;
	}
}

//Erase point from simplex and join the gap.
void Simplex::erase(unsigned int index){
	for(unsigned int i=index;i<3;i++){
		vertices[i] = vertices[i + 1];
	}
	//vertices[3] = Vec3(0, 0, 0);

	if(numVertices > 0){
		numVertices--;
	}
}

//Reorder points.
void Simplex::organize(unsigned int a, unsigned int b, unsigned int c, unsigned int d){
	Vec3 verts[4];
	memcpy(verts, vertices, 4 * sizeof(Vec3));

	vertices[0] = verts[a];
	vertices[1] = verts[b];
	vertices[2] = verts[c];
	vertices[3] = verts[d];
}

//Reset internal state.
void Simplex::reset(){
	numVertices = 0;
}

//Checks if dot product is positive.
bool Simplex::sameSide(Vec3 u, Vec3 v){
	return Vec3::dot(u, v) > 0;
}

//Expand function dispatcher.
bool Simplex::expand(Vec3 point, Vec3& direction){
	append(point);
	switch(numVertices){
		case 2:
			return lineCase(direction);

		case 3:
			return triangleCase(direction);

		case 4:
			return tetrahedronCase(direction);

		default:
			std::cout<<"WARNING: Simplex somehow has less than two points. This should not be possible."<<std::endl;
			return false;
	}

	return false;
}

//Handles the line case.
bool Simplex::lineCase(Vec3& direction){
	Vec3 ab = vertices[1] - vertices[0];
	Vec3 ao = vertices[0] * (-1);
	
	if(sameSide(ab, ao)){
		direction = Vec3::tripleCross(ab, ao, ab);
	}else{
		erase(1);
		direction = ao;
	}

	return false;

}

//Handles the triangle case.
bool Simplex::triangleCase(Vec3& direction){
	Vec3 ab = vertices[1] - vertices[0];
	Vec3 ac = vertices[2] - vertices[0];
	Vec3 ao = vertices[0] * (-1);
	Vec3 abc = Vec3::cross(ab, ac);

	if(sameSide(Vec3::cross(abc, ac), ao)){
		if(sameSide(ac, ao)){
			erase(1);
			direction = Vec3::tripleCross(ac, ao, ac);
		}else{
			erase(2);
			return lineCase(direction);
		}
	}else{
		if(sameSide(Vec3::cross(ab, abc), ao)){
			erase(2);
			return lineCase(direction);	
		}else{
			if(sameSide(abc, ao)){
				direction = abc;
			}else{
				organize(0, 2, 1, 3);
				numVertices = 3;
				direction = abc * (-1);
			}
		}
	}

	return false;
}

//Handles the tetrahedron case.
bool Simplex::tetrahedronCase(Vec3& direction){
	Vec3 ab = vertices[1] - vertices[0];
	Vec3 ac = vertices[2] - vertices[0];
	Vec3 ad = vertices[3] - vertices[0];
	Vec3 ao = vertices[0] * (-1);

	Vec3 abc = Vec3::cross(ab, ac);
	Vec3 acd = Vec3::cross(ac, ad);
	Vec3 adb = Vec3::cross(ad, ab);

	if(sameSide(abc, ao)){
		erase(3);
		return triangleCase(direction);
	}if(sameSide(acd, ao)){
		organize(0, 2, 3, 1);
		numVertices = 3;
		return triangleCase(direction);
	}if(sameSide(adb, ao)){
		organize(0, 3, 1, 2);
		numVertices = 3;
		return triangleCase(direction);
	}

	return true;
}

Vec3* Simplex::ptr(){
	return &vertices[0];
}

//Print values from simplex.
void Simplex::print(){
	for(unsigned int i=0;i<4;i++){
		vertices[i].print();
	}
}

//------------------------------------------------------------------------------------

//Polytope edge constructor.
Polyedge::Polyedge(unsigned int a, unsigned int b){
	this->a = a;
	this->b = b;
}

//Check if edges are reverses of eachother.
bool Polyedge::isReverse(const Polyedge& a, const Polyedge& b){
	if(a.a == b.b && a.b == b.a){return true;}
	else{return false;}
}

//Polytope face constructor.
Polyface::Polyface(unsigned int a, unsigned int b, unsigned int c){
	this->a = a;
	this->b = b;
	this->c = c;
}

//Return an edge from face.
Polyedge Polyface::getEdge(unsigned int edgei){
	switch(edgei){
		case 0:
			return Polyedge(a, b);
		case 1:
			return Polyedge(b, c);
		case 2:
			return Polyedge(c, a);
		default:
			return Polyedge(a, b);
	}
}

//Polytope constructor. For in use with the EPA algorithm.
Polytope::Polytope(){
	vertices.reserve(10);
	faces.reserve(20);
}

//Re initialize the polytope.
void Polytope::reset(Vec3* points){
	vertices.clear();
	vertices.insert(vertices.end(), points, points+4);

	faces.clear();
	faces.push_back(Polyface(0, 1, 2));
	faces.push_back(Polyface(0, 3, 1));
	faces.push_back(Polyface(0, 2, 3));
	faces.push_back(Polyface(1, 3, 2));
}

//Get a faces normal and distance form origin.
void Polytope::faceNormal(unsigned int facei, Vec3& normal, float& distance){
	Vec3 ab = vertices[faces[facei].b] - vertices[faces[facei].a];
	Vec3 ac = vertices[faces[facei].c] - vertices[faces[facei].a];

	Vec3 norm = Vec3::cross(ab, ac);
	norm.normalize();
	float dist = Vec3::dot(norm, vertices[faces[facei].a]);

	if(dist < 0){
		normal = norm * (-1);
		distance = dist * (-1);
	}else{
		normal = norm;
		distance = dist;
	}
}

//Determine the face closest to origin and get its normal and distance.
void Polytope::closestFace(unsigned int& facei, Vec3& normal, float& distance){
	unsigned int topFacei = 0;
	Vec3 norm, topNorm;
	float dist, topDist;
	faceNormal(0, topNorm, topDist);
	for(unsigned int i=1;i<faces.size();i++){
		faceNormal(i, norm, dist);
		if(dist < topDist){
			topFacei = i;
			topNorm = norm;
			topDist = dist;
		}
	}
	facei = topFacei;
	normal = topNorm;
	distance = topDist;
}

//Append a new point onto the polygon and repair all affected faces.
void Polytope::expand(Vec3 point){
	Vec3 norm;
	float dist;
	Polyedge tempEdge;
	std::vector<Polyedge> uniqueEdges;
	uniqueEdges.reserve(12);
	
	for(int i=faces.size()-1;i>=0;i--){
		faceNormal(i, norm, dist);
		if(Vec3::dot(norm, point) > 0){
			for(unsigned int j=0;j<3;j++){
				tempEdge = faces[i].getEdge(j);
				bool exists = false;
				for(int k=uniqueEdges.size()-1;k>=0;k--){
					if(Polyedge::isReverse(tempEdge, uniqueEdges[k])){
						uniqueEdges.erase(uniqueEdges.begin() + k);
						exists = true;
					}
				}
				if(!exists){
					uniqueEdges.push_back(tempEdge);
				}
			}
			faces.erase(faces.begin() + i);
		}
	}

	vertices.push_back(point);
	for(unsigned int i=0;i<uniqueEdges.size();i++){
		faces.push_back(Polyface(uniqueEdges[i].a, uniqueEdges[i].b, vertices.size()-1));
	}
}

//------------------------------------------------------------------------------------

//AABB
AABB::AABB(Vec3 min, Vec3 max){
	this->min = min;
	this->max = max;
}

//AABB intersect AABB
bool AABB::intersect(AABB& a, AABB& b){
	return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
		(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
		(a.min.z <= b.max.z && a.max.z >= b.min.z);
}

//------------------------------------------------------------------------------------

//Sphere collider
BoundingSphere::BoundingSphere(Vec3 center, float radius, float aspect){
	this->center = center;
	this->radius = radius;
	verticalAspect = aspect;
}

//Sphere furthest point.
Vec3 BoundingSphere::furthest(Vec3 direction){
	Vec3 result = Vec3::normalize(direction) * radius;
	result.z = result.z * verticalAspect;
	return center + result;
}

//Swept sphere collider
SweptSphere::SweptSphere(Vec3 center, float radius, float aspect){
	colliders[0].center = center;
	colliders[0].radius = radius;
	colliders[0].verticalAspect = aspect;

	colliders[1].center = center;
	colliders[1].radius = radius;
	colliders[1].verticalAspect = aspect;

	toggle = 1;
}

//Swept sphere furthest point.
Vec3 SweptSphere::furthest(Vec3 direction){
	Vec3 dir = Vec3::normalize(direction);

	Vec3 result0 = dir * colliders[0].radius;
	result0.z = result0.z * colliders[0].verticalAspect;
	result0 = result0 + colliders[0].center;

	Vec3 result1 = dir * colliders[1].radius;
	result1.z = result1.z * colliders[1].verticalAspect;
	result1 = result1 + colliders[1].center;

	if(Vec3::dot(dir, result0) >= Vec3::dot(dir, result1)){
		return result0;
	}else{
		return result1;
	}
}

BoundingSphere* SweptSphere::getNext(){
	return &colliders[toggle];
}

BoundingSphere* SweptSphere::getPrev(){
	return &colliders[(1 - toggle)];
}

void SweptSphere::swapSpheres(){
	toggle += 1;
	if(toggle > 1){
		toggle = 0;
	}
}

//------------------------------------------------------------------------------------

//Convex collider.
BoundingConvex::BoundingConvex(Vec3* start, unsigned int numVertices){
	vertices = start;
	this->numVertices = numVertices;
}

//Convex furthest point.
Vec3 BoundingConvex::furthest(Vec3 direction){
	float dist = Vec3::dot(direction, vertices[0]);
	float topDist = dist;
	unsigned int top = 0;
	for(unsigned int i=1;i<numVertices;i++){
		dist = Vec3::dot(direction, vertices[i]);
		if(dist > topDist){
			topDist = dist;
			top = i;
		}
	}
	return vertices[top];
}

//Create an approximate AABB for convex.
AABB BoundingConvex::createBox(){
	Vec3 min = vertices[0];
	Vec3 max = vertices[0];

	for(unsigned int i=1;i<numVertices;i++){
		min.makeMin(vertices[i]);
		max.makeMax(vertices[i]);
	}

	return AABB(min, max);
}

