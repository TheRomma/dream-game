#include "3Dmaths.hpp"

//Vec3 default constructor.
Vec3::Vec3(){
	x = 0;
	y = 0;
	z = 0;
}

//Vec3 constructor taking three floats.
Vec3::Vec3(float x=0, float y=0, float z=0){
	this->x = x;
	this->y = y;
	this->z = z;
}

/*
//Vec3 copy constructor.
Vec3::Vec3(const Vec3& other){
	x = other.x;
	y = other.y;
	z = other.z;
}
*/

//Vec3 destructor.
Vec3::~Vec3(){
	
}

//Vec3 + operator against Vec3.
Vec3 Vec3::operator+(const Vec3 u){
	return Vec3(x + u.x, y + u.y, z + u.z);
}

//Vec3 - operator against Vec3.
Vec3 Vec3::operator-(const Vec3 u){
	return Vec3(x - u.x, y - u.y, z - u.z);
}

//Vec3 * operator against float.
Vec3 Vec3::operator*(float s){
	return Vec3(x * s, y * s, z * s);
}

//Vec3 / operator against float.
Vec3 Vec3::operator/(float s){
	return Vec3(x / s, y / s, z / s);
}

//Vec3 == operator againt Vec3.
bool Vec3::operator==(const Vec3 u){
	if(x == u.x && y == u.y && z == u.z){
		return true;
	}else{
		return false;
	}
}

//Vec3 dot product.
float Vec3::dot(const Vec3 u, const Vec3 v){
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

//Vec3 cross product.
Vec3 Vec3::cross(const Vec3 u, const Vec3 v){
	return Vec3(
		u.y * v.z - u.z * v.y,
		(-1) * (u.x * v.z - u.z * v.x),
		u.x * v.y - u.y * v.x);
}

//Vec3 triple cross product.
Vec3 Vec3::tripleCross(const Vec3 u, const Vec3 v, const Vec3 w){
	Vec3 a = Vec3::cross(u, v);
	return Vec3::cross(a, w);
}

//Normalize Vec3.
Vec3 Vec3::normalize(const Vec3 u){
	float l = sqrt(u.x * u.x + u.y * u.y + u.z * u.z);
	return Vec3(u.x / l, u.y / l, u.z / l);
}

//Interpolate between two Vec3s.
Vec3 Vec3::interpolate(Vec3 u, Vec3 v, float t){
	float time = t;
	if(time > 1){time = 1;}
	else if(time < 0){time = 0;}
	return u * (1 - time) + v * time;
}

Vec3 Vec3::max(const Vec3 u, const Vec3 v){
	return Vec3(std::max(u.x, v.x), std::max(u.y, v.y), std::max(u.z, v.z));
}

Vec3 Vec3::min(const Vec3 u, const Vec3 v){
	return Vec3(std::min(u.x, v.x), std::min(u.y, v.y), std::min(u.z, v.z));
}

//Internally normalize Vec3.
void Vec3::normalize(){
	float l = this->length();
	x = x / l;
	y = y / l;
	z = z / l;
}

void Vec3::makeMax(Vec3 u){
	x = std::max(x, u.x);
	y = std::max(y, u.y);
	z = std::max(z, u.z);
}

void Vec3::makeMin(Vec3 u){
	x = std::min(x, u.x);
	y = std::min(y, u.y);
	z = std::min(z, u.z);
}

//Return pointer to the beginning of Vec3 members.
float* Vec3::ptr(){
	return &x;
}

//Return Vec3 euclidean length.
float Vec3::length(){
	return sqrt(x * x + y * y + z * z);
}

//Return Vec3 cityblock length.
float Vec3::manhattan(){
	return x + y + z;
}

//Prints the three values.
void Vec3::print(){
	float* p = &x;
	for(int i=0;i<3;i++){
		std::cout<<p[i]<<" ";
	}
	std::cout<<std::endl;
}

//----------------------------------------------------------------------------------

//Mat4 constructor.
Mat4::Mat4(){

}

//Mat4 constructor that takes a buffer.
Mat4::Mat4(float* arr){
	memcpy(this->m, arr, 16 * sizeof(float));
}

//Mat4 copy constructor.
Mat4::Mat4(const Mat4& other){
	memcpy(this->m, other.m, 16 * sizeof(float));
}

//Mat4 destructor.
Mat4::~Mat4(){

}

//Mat4 operator *.
Mat4 Mat4::operator*(const Mat4& b){
	float a[16] = {};
	for(unsigned int i=0;i<4;i++){
		for(unsigned int j=0;j<4;j++){
			a[i * 4 + j] = m[i][0]*b.m[0][j] + m[i][1]*b.m[1][j] + m[i][2]*b.m[2][j] + m[i][3]*b.m[3][j]; 
		}
	}
	return Mat4(a);
}

//Mat4 * Vec3
Vec3 Mat4::operator*(const Vec3& u){
	Vec3 a(0, 0, 0);
	float* p = a.ptr();
	for(unsigned int i=0;i<3;i++){
		p[i] = m[0][i] * u.x + m[1][i] * u.y + m[2][i] * u.z + m[3][i];
	}
	return a;
}

//Mat4 * Vec3 also calculates w
Vec3 Mat4::transform(const Vec3& u, float W, float& w){
	Vec3 a(0, 0, 0);
	float* p = a.ptr();
	for(unsigned int i=0;i<3;i++){
		p[i] = m[0][i] * u.x + m[1][i] * u.y + m[2][i] * u.z + m[3][i] * W;
		w = m[0][3] * u.x + m[1][3] * u.y + m[2][3] * u.z + m[3][3] * W;
	}
	return a;
}

//Mat4 * float.
Mat4 Mat4::operator*(const float& b){
	float a[16] = {};
	for(unsigned int i=0;i<4;i++){
		for(unsigned int j=0;j<4;j++){
			a[i * 4 + j] = m[i][j] * b; 
		}
	}
	return Mat4(a);
}

//Creates an identity matrix.
Mat4 Mat4::identity(){
	float a[16] = {
		1.0, 0.0, 0.0, 0.0,	
		0.0, 1.0, 0.0, 0.0,	
		0.0, 0.0, 1.0, 0.0,	
		0.0, 0.0, 0.0, 1.0	
	};
	return Mat4(a);
}

//Creates a translation matrix from floats.
Mat4 Mat4::translation(float x, float y, float z){
	float a[16] = {
		1.0, 0.0, 0.0, 0.0,	
		0.0, 1.0, 0.0, 0.0,	
		0.0, 0.0, 1.0, 0.0,	
		x, y, z, 1.0	
	};
	return Mat4(a);
}

//Creates a translation matrix from a vector.
Mat4 Mat4::translation(Vec3 u){
	float a[16] = {
		1.0, 0.0, 0.0, 0.0,	
		0.0, 1.0, 0.0, 0.0,	
		0.0, 0.0, 1.0, 0.0,	
		u.x, u.y, u.z, 1.0	
	};
	return Mat4(a);
}

//Creates a scaling matrix from floats.
Mat4 Mat4::scale(float w, float h, float d){
	float a[16] = {
		w, 0.0, 0.0, 0.0,	
		0.0, h, 0.0, 0.0,	
		0.0, 0.0, d, 0.0,	
		0.0, 0.0, 0.0, 1.0	
	};
	return Mat4(a);
}

//Creates a scaling matrix from a vector.
Mat4 Mat4::scale(Vec3 u){
	float a[16] = {
		u.x, 0.0, 0.0, 0.0,	
		0.0, u.y, 0.0, 0.0,	
		0.0, 0.0, u.z, 0.0,	
		0.0, 0.0, 0.0, 1.0	
	};
	return Mat4(a);
}

//Creates an orthographic projection matrix.
Mat4 Mat4::orthographic(float left, float right, float bottom, float top, float near, float far){
	
	float a[16] = {
		2 / (right - left), 0.0, 0.0, 0.0,
		0.0, 2 / (top - bottom), 0.0, 0.0,
		0.0, 0.0, -2 / (far - near), 0.0,
		-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1.0
	};
	return Mat4(a);
}

//Creates a perspective projection matrix.
Mat4 Mat4::perspective(float fov, float aspect, float near, float far){
	float f = 1.0 / tan(fov / 2);
	float inv = 1.0 / (near - far);
	float a[16] = {
		f / aspect, 0.0, 0.0, 0.0,	
		0.0, f, 0.0, 0.0,	
		0.0, 0.0, (near + far) * inv, -1.0,	
		0.0, 0.0, near * far * inv * 2, 1.0	
	};
	return Mat4(a);
}

//Creates a camera matrix.
Mat4 Mat4::lookAt(Vec3 position, Vec3 target, Vec3 up){
	Vec3 d = position - target;
	d.normalize();

	Vec3 r = Vec3::cross(up, d);
	r.normalize();

	Vec3 u = Vec3::cross(d, r);
	u.normalize();
	
	float a[16] = {
		r.x, u.x, d.x, 0.0,	
		r.y, u.y, d.y, 0.0,	
		r.z, u.z, d.z, 0.0,	
		0.0, 0.0, 0.0, 1.0	
	};
	return Mat4::translation(position * (-1)) * Mat4(a);
}

//Transposes a matrix.
Mat4 Mat4::transpose(const Mat4& M){
	float arr[16];
	for(unsigned int i=0;i<4;i++){
		for(unsigned int j=0;j<4;j++){
			arr[i * 4 + j] = M.m[j][i];
		}
	}
	return Mat4(arr);
}

//Standard matrix multiplication, but changes the current matrix.
void Mat4::transform(const Mat4& M){
	float a[16] = {};
	for(unsigned int i=0;i<4;i++){
		for(unsigned int j=0;j<4;j++){
			a[i * 4 + j] = m[i][0]*M.m[0][j] + m[i][1]*M.m[1][j] + m[i][2]*M.m[2][j] + m[i][3]*M.m[3][j]; 
		}
	}
	memcpy(this->m, a, 16 * sizeof(float));
}

//Transposes a matrix.
void Mat4::transpose(){
	float arr[4][4];
	for(unsigned int i=0;i<4;i++){
		for(unsigned int j=0;j<4;j++){
			arr[i][j] = m[j][i];
		}
	}
	memcpy(this->m, arr, 16 * sizeof(float));
}

//Pointer to the beginning of members.
float* Mat4::ptr(){
	return &m[0][0];
}

//Calculate determinant.
float Mat4::determinant(){
	return m[0][0]*(
		m[1][1]*(m[2][2] * m[3][3] - m[3][2] * m[2][3])
		-m[1][2]*(m[2][1] * m[3][3] - m[3][1] * m[2][3])
		+m[1][3]*(m[2][1] * m[3][2] - m[3][1] * m[2][2]))
	-m[0][1]*(
		m[1][0]*(m[2][2] * m[3][3] - m[3][2] * m[2][3])
		-m[1][2]*(m[2][0] * m[3][3] - m[3][0] * m[2][3])
		+m[1][3]*(m[2][0] * m[3][2] - m[3][0] * m[2][2]))
	+m[0][2]*(
		m[1][0]*(m[2][1] * m[3][3] - m[3][1] * m[2][3])
		-m[1][1]*(m[2][0] * m[3][3] - m[3][0] * m[2][3])
		+m[1][3]*(m[2][0] * m[3][1] - m[3][0] * m[2][1]))
	-m[0][3]*(
		m[1][0]*(m[2][1] * m[3][2] - m[3][1] * m[2][2])
		-m[1][1]*(m[2][0] * m[3][2] - m[3][0] * m[2][2])
		+m[1][2]*(m[2][0] * m[3][1] - m[3][0] * m[2][1]));
}

//Calculate cofactor matrix.
Mat4 Mat4::cofactor(){
	float result[16] = {
		m[1][1]*(m[2][2] * m[3][3] - m[3][2] * m[2][3])
		-m[1][2]*(m[2][1] * m[3][3] - m[3][1] * m[2][3])
		+m[1][3]*(m[2][1] * m[3][2] - m[3][1] * m[2][2]),
		
		-1*(m[1][0]*(m[2][2] * m[3][3] - m[3][2] * m[2][3])
		-m[1][2]*(m[2][0] * m[3][3] - m[3][0] * m[2][3])
		+m[1][3]*(m[2][0] * m[3][2] - m[3][0] * m[2][2])),

		m[1][0]*(m[2][1] * m[3][3] - m[3][1] * m[2][3])
		-m[1][1]*(m[2][0] * m[3][3] - m[3][0] * m[2][3])
		+m[1][3]*(m[2][0] * m[3][1] - m[3][0] * m[2][1]),

		-1*(m[1][0]*(m[2][1] * m[3][2] - m[3][1] * m[2][2])
		-m[1][1]*(m[2][0] * m[3][2] - m[3][0] * m[2][2])
		+m[1][2]*(m[2][0] * m[3][1] - m[3][0] * m[2][1])),

		-1*(m[0][1]*(m[2][2] * m[3][3] - m[3][2] * m[2][3])
		-m[0][2]*(m[2][1] * m[3][3] - m[3][1] * m[2][3])
		+m[0][3]*(m[2][1] * m[3][2] - m[3][1] * m[2][2])),
		
		m[0][0]*(m[2][2] * m[3][3] - m[3][2] * m[2][3])
		-m[0][2]*(m[2][0] * m[3][3] - m[3][0] * m[2][3])
		+m[0][3]*(m[2][0] * m[3][2] - m[3][0] * m[2][2]),

		-1*(m[0][0]*(m[2][1] * m[3][3] - m[3][1] * m[2][3])
		-m[0][1]*(m[2][0] * m[3][3] - m[3][0] * m[2][3])
		+m[0][3]*(m[2][0] * m[3][1] - m[3][0] * m[2][1])),

		m[0][0]*(m[2][1] * m[3][2] - m[3][1] * m[2][2])
		-m[0][1]*(m[2][0] * m[3][2] - m[3][0] * m[2][2])
		+m[0][2]*(m[2][0] * m[3][1] - m[3][0] * m[2][1]),

		m[0][1]*(m[1][2] * m[3][3] - m[3][2] * m[1][3])
		-m[0][2]*(m[1][1] * m[3][3] - m[3][1] * m[1][3])
		+m[0][3]*(m[1][1] * m[3][2] - m[3][1] * m[1][2]),
		
		-1*(m[0][0]*(m[1][2] * m[3][3] - m[3][2] * m[1][3])
		-m[0][2]*(m[1][0] * m[3][3] - m[3][0] * m[1][3])
		+m[0][3]*(m[1][0] * m[3][2] - m[3][0] * m[1][2])),

		m[0][0]*(m[1][1] * m[3][3] - m[3][1] * m[1][3])
		-m[0][1]*(m[1][0] * m[3][3] - m[3][0] * m[1][3])
		+m[0][3]*(m[1][0] * m[3][1] - m[3][0] * m[1][1]),

		-1*(m[0][0]*(m[1][1] * m[3][2] - m[3][1] * m[1][2])
		-m[0][1]*(m[1][0] * m[3][2] - m[3][0] * m[1][2])
		+m[0][2]*(m[1][0] * m[3][1] - m[3][0] * m[1][1])),

		-1*(m[0][1]*(m[1][2] * m[2][3] - m[2][2] * m[1][3])
		-m[0][2]*(m[1][1] * m[2][3] - m[2][1] * m[1][3])
		+m[0][3]*(m[1][1] * m[2][2] - m[2][1] * m[1][2])),
		
		m[0][0]*(m[1][2] * m[2][3] - m[2][2] * m[1][3])
		-m[0][2]*(m[1][0] * m[2][3] - m[2][0] * m[1][3])
		+m[0][3]*(m[1][0] * m[2][2] - m[2][0] * m[1][2]),

		-1*(m[0][0]*(m[1][1] * m[2][3] - m[2][1] * m[1][3])
		-m[0][1]*(m[1][0] * m[2][3] - m[2][0] * m[1][3])
		+m[0][3]*(m[1][0] * m[2][1] - m[2][0] * m[1][1])),

		m[0][0]*(m[1][1] * m[2][2] - m[2][1] * m[1][2])
		-m[0][1]*(m[1][0] * m[2][2] - m[2][0] * m[1][2])
		+m[0][2]*(m[1][0] * m[2][1] - m[2][0] * m[1][1])
	};
	return Mat4(result);
}

//Calculate inverse of matrix.
Mat4 Mat4::inverse(){
	float det = determinant();
	Mat4 cof = cofactor();
	cof.transpose();
	return cof * (1 / det);
}

//Prints all values.
void Mat4::print(){
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			std::cout<<m[i][j]<<" ";
		}
		std::cout<<std::endl;
	}
	std::cout<<std::endl;
}

//-------------------------------------------------------------------------------

//Quaternion default constructor.
Quat::Quat(){
	w = 0;
	x = 0;
	y = 0;
	z = 0;
}

//Quaternion constructor that takes floats.
Quat::Quat(float w=0, float x=0, float y=0, float z=0){
	this->w = w;
	this->x = x;
	this->y = y;
	this->z = z;
}

//Quaternion constructor representing an angle of rotation around an axis.
Quat::Quat(float angle, Vec3 axis){
	w = cos(angle / 2);
	x = sin(angle / 2) * axis.x;
	y = sin(angle / 2) * axis.y;
	z = sin(angle / 2) * axis.z;
}

//Quaternion copy constructor.
Quat::Quat(const Quat& other){
	w = other.w;
	x = other.x;
	y = other.y;
	z = other.z;
}

//Quaternion destructor.
Quat::~Quat(){

}

//Interpolate between quaternions.
Quat Quat::slerp(const Quat& a, const Quat& b, float t){
	float time = t;
	if(time > 1){time = 1;}
	else if(time < 0){time = 0;}

	float dot = fmin(a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z, 1.0);
	float theta = acos(dot);
	float sTheta = sin(theta);

	if(fabs(sTheta) < 0.001){
		Quat result0 = Quat(
			0.5 * a.w + 0.5 * b.w,
			0.5 * a.x + 0.5 * b.x,
			0.5 * a.y + 0.5 * b.y,
			0.5 * a.z + 0.5 * b.z
		);
		result0.normalize();

		return result0;
	}

	float ratioA = sin((1 - time) * theta) / sTheta;
	float ratioB = sin(time * theta) / sTheta;
	
	Quat result = Quat(
		ratioA * a.w + ratioB * b.w,
		ratioA * a.x + ratioB * b.x,
		ratioA * a.y + ratioB * b.y,
		ratioA * a.z + ratioB * b.z
	);
	result.normalize();

	return result;
}

//Normalize quaternion to unit length.
Quat Quat::normalize(Quat q){
	float l = q.length();
	return Quat(
		q.w / l,
		q.x / l,
		q.y / l,
		q.z / l
	);
}

//Normalize quaternion to unit length internally.
void Quat::normalize(){
	float l = this->length();
	w = w / l;
	x = x / l;
	y = y / l;
	z = z / l;
}

//Make a rotation matrix out of the quat.
Mat4 Quat::toMatrix(){
	float arrA[16] = {
		 w, z,-y, x,
		-z, w, x, y,
		 y,-x, w, z,
		-x,-y,-z, w
	};
	Mat4 a = Mat4(arrA);

	float arrB[16] = {
		 w, z,-y,-x,
		-z, w, x,-y,
		 y,-x, w,-z,
		 x, y, z, w
	};
	Mat4 b = Mat4(arrB);

	return a * b;
}

//Return pointer to the beginning of quat members.
float* Quat::ptr(){
	return &w;
}

//Get the length of a quaternion.
float Quat::length(){
	return sqrt(w * w + x * x + y * y + z * z);
}

//Print all values of quat.
void Quat::print(){
	float* p = &w;
	for(int i=0;i<4;i++){
		std::cout<<p[i]<<" ";
	}
	std::cout<<std::endl;
}
