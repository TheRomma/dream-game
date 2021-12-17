#include "model.hpp"

//-----------------------------------------------------------------------------------------------------------

//Static model constructor
void StaticModel::init(const char* filename){
	StaticModelFile file(filename);

	const char* vertex = R"(#version 430 core
	layout(location = 0) in vec3 POSITION;
	layout(location = 1) in vec2 UV_COORD;
	layout(location = 2) in vec3 NORMAL;

	layout(std140, binding = 0) uniform U{
		mat4 projView;
		float time;
	};

	layout(location = 0) uniform mat4 u_model;

	out VS_OUT{
		vec2 uv_coord;
		vec3 normal;
	} F;

	void main(){
		gl_Position = projView * u_model * vec4(POSITION, 1.0);

		F.uv_coord = UV_COORD;
		F.normal = NORMAL;
	}
	)";
	const char* fragment = R"(#version 430 core
	out vec4 outColor;

	in VS_OUT{
		vec2 uv_coord;
		vec3 normal;
	}F;

	layout(binding = 0) uniform sampler2D u_texture;

	void main(){
		outColor = texture(u_texture, F.uv_coord);
	}
	)";	

	shader.init(vertex, fragment);

	buffer.init(8 * sizeof(float), file.attribLength, file.attributes);
	buffer.setAttribute(0, 3);
	buffer.setAttribute(1, 2);
	buffer.setAttribute(2, 3);

	texture.init(file.texWidth, file.texHeight, GL_NEAREST, (char*)file.texture);
}

//Static model single draw.
void StaticModel::draw(Mat4 model){
	shader.use();
	glUniformMatrix4fv(0, 1, false, model.ptr());
	buffer.bind();
	texture.bind(0);

	glDrawArrays(GL_TRIANGLES, 0, buffer.numVertices);
}

//------------------------------------------------------------------------------------------------------

//Static model constructor
void AnimatedModel::init(const char* filename){
	AnimatedModelFile file(filename);

	std::string vertex = R"(#version 430 core
	#define UNIFORM_COMMON_BASE [COMMON_BASE]

	#define NUM_BONES [NUM_BONES]

	layout(location = 0) in vec3 POSITION;
	layout(location = 1) in vec2 UV_COORD;
	layout(location = 2) in vec3 NORMAL;
	layout(location = 3) in vec4 BONES;
	layout(location = 4) in vec4 WEIGHTS;

	layout(std140, binding = 0) uniform U{
		mat4 projView;
		vec4 posTime;
	};

	layout(location = 0) uniform mat4 u_model;
	layout(location = 1) uniform mat4 u_joints[NUM_BONES];

	out VS_OUT{
		vec3 position;
		vec2 uv_coord;
		vec3 normal;
		float distance;
	} F;

	void main(){
		vec4 transform = u_model * (
			u_joints[int(BONES.r)] * vec4(POSITION, 1.0) * WEIGHTS.r +
			u_joints[int(BONES.g)] * vec4(POSITION, 1.0) * WEIGHTS.g +
			u_joints[int(BONES.b)] * vec4(POSITION, 1.0) * WEIGHTS.b +
			u_joints[int(BONES.a)] * vec4(POSITION, 1.0) * WEIGHTS.a
		);
		gl_Position = projView * transform;

		F.position = transform.rgb;
		F.uv_coord = UV_COORD;
		//F.normal = normalize(mat3(transpose(inverse(u_model))) * NORMAL.xyz);
		F.normal = normalize(mat3(transpose(inverse(u_model))) * (
			mat3(transpose(inverse(u_joints[int(BONES.r)]))) * NORMAL.xyz * WEIGHTS.r +
			mat3(transpose(inverse(u_joints[int(BONES.g)]))) * NORMAL.xyz * WEIGHTS.g +
			mat3(transpose(inverse(u_joints[int(BONES.b)]))) * NORMAL.xyz * WEIGHTS.b +
			mat3(transpose(inverse(u_joints[int(BONES.a)]))) * NORMAL.xyz * WEIGHTS.a
		));
		F.distance = transform.z;
	}
	)";
	findAndReplace(vertex, "[COMMON_BASE]", std::to_string(UNIFORM_COMMON_BASE));
	findAndReplace(vertex, "[NUM_BONES]", std::to_string(file.numBones));
	std::string fragment = R"(#version 430 core
	#define UNIFORM_COMMON_BASE [COMMON_BASE]

	layout(location = 0) out vec3 gPosition;
	layout(location = 1) out vec4 gNormal;
	layout(location = 2) out vec3 gAlbedo;

	in VS_OUT{
		vec3 position;
		vec2 uv_coord;
		vec3 normal;
		float distance;
	}F;

	layout(std140, binding = UNIFORM_COMMON_BASE) uniform U{
		mat4 projView;
		vec4 posTime;
	};

	layout(binding = 0) uniform sampler2D u_texture;

	void main(){
		gPosition = F.position;
		gNormal = vec4(normalize(F.normal), F.distance);
		gAlbedo = texture(u_texture, F.uv_coord).rgb;
	}
	)";	
	findAndReplace(fragment, "[COMMON_BASE]", std::to_string(UNIFORM_COMMON_BASE));

	shader.init(vertex.c_str(), fragment.c_str());

	buffer.init(16 * sizeof(float), file.attribLength, file.attributes);
	buffer.setAttribute(0, 3);
	buffer.setAttribute(1, 2);
	buffer.setAttribute(2, 3);
	buffer.setAttribute(3, 4);
	buffer.setAttribute(4, 4);

	texture.init(file.texWidth, file.texHeight, GL_LINEAR, (char*)file.texture);

	numBones = file.numBones;
	joints = (Mat4*)malloc(numBones * sizeof(Mat4));
	for(unsigned int i=0;i<numBones;i++){
		joints[i] = Mat4::identity();
	}
}

//Animated model destructor.
AnimatedModel::~AnimatedModel(){
	free(joints);
}

//Pose animated model.
void AnimatedModel::pose(Animation& anim, float time){
	for(unsigned int i=0;i<numBones;i++){
		joints[i] = anim.calcJoint(i, time);
	}
}

//Static model single draw.
void AnimatedModel::draw(Mat4 model){
	shader.use();
	glUniformMatrix4fv(0, 1, false, model.ptr());
	glUniformMatrix4fv(1, numBones, false, joints[0].ptr());
	buffer.bind();
	texture.bind(0);

	glDrawArrays(GL_TRIANGLES, 0, buffer.numVertices);
}

//---------------------------------------------------------------------------------------------------

//Static multi mesh model constructor
void MultiModel::init(const char* filename){
	MultiModelFile file(filename);

	std::string vertex = R"(#version 430 core
	#define UNIFORM_COMMON_BASE [COMMON_BASE]

	layout(location = 0) in vec3 POSITION;
	layout(location = 1) in vec3 UV_COORD;
	layout(location = 2) in vec3 NORMAL;

	out VS_OUT{
		vec3 position;
		vec3 uv_coord;
		vec3 normal;
		float distance;
	} F;

	layout(std140, binding = UNIFORM_COMMON_BASE) uniform U{
		mat4 projView;
		vec4 posTime;
	};

	layout(location = 0) uniform mat4 u_model;

	void main(){
		vec4 result = projView * u_model * vec4(POSITION, 1.0);
		gl_Position = result;

		F.position = (u_model * vec4(POSITION, 1.0)).rgb;
		F.uv_coord = UV_COORD;
		F.normal = normalize(mat3(transpose(inverse(u_model))) * NORMAL.xyz);
		F.distance = result.z;
	}
	)";
	findAndReplace(vertex, "[COMMON_BASE]", std::to_string(UNIFORM_COMMON_BASE));
	std::string fragment = R"(#version 430 core
	#define UNIFORM_COMMON_BASE [COMMON_BASE]

	layout(location = 0) out vec3 gPosition;
	layout(location = 1) out vec4 gNormal;
	layout(location = 2) out vec3 gAlbedo;

	in VS_OUT{
		vec3 position;
		vec3 uv_coord;
		vec3 normal;
		float distance;
	}F;

	layout(std140, binding = UNIFORM_COMMON_BASE) uniform U{
		mat4 projView;
		vec4 posTime;
	};

	layout(binding = 0) uniform sampler2DArray u_texture;

	void main(){
		gPosition = F.position;
		gNormal = vec4(normalize(F.normal), F.distance);
		gAlbedo = texture(u_texture, F.uv_coord).rgb;
	}
	)";	
	findAndReplace(fragment, "[COMMON_BASE]", std::to_string(UNIFORM_COMMON_BASE));
	shader.init(vertex.c_str(), fragment.c_str());

	const char* shadowVertex = R"(#version 430 core
	layout(location = 0) in vec3 POSITION;

	layout(location = 0) uniform mat4 u_model;

	void main(){
		gl_Position = u_model * vec4(POSITION, 1.0);
	}
	)";
	std::string shadowGeometry = R"(#version 430 core
	#define UNIFORM_LIGHTING_BASE [LIGHTING_BASE]

	#define SUN_NUM_SHADOW_CASCADES [SUN_CASCADES]

	#define MAX_POINTLIGHTS [MAX_POINTS]
	#define MAX_SPOTLIGHTS [MAX_SPOTS]

	layout(triangles, invocations = SUN_NUM_SHADOW_CASCADES) in;
	layout(triangle_strip, max_vertices = 3) out;

	struct Sun{
		vec3 direction;
		vec3 ambient;
		vec3 diffuse;
		mat4 view[SUN_NUM_SHADOW_CASCADES];
	};

	struct Pointlight{
		vec4 position;
		vec3 ambient;
		vec3 diffuse;
	};

	struct Spotlight{
		vec4 position;
		vec4 direction;
		vec3 ambient;
		vec3 diffuse;
	};

	layout(std140, binding = UNIFORM_LIGHTING_BASE) uniform L{
		Sun sun;
		ivec4 numLights;//r = numPointlights, b = numSpotlights
		Pointlight pointlights[MAX_POINTLIGHTS];
		Spotlight spotlights[MAX_SPOTLIGHTS];
	};

	void main(){
		for(unsigned int i=0;i<SUN_NUM_SHADOW_CASCADES;i++){
			gl_Position = 
				sun.view[gl_InvocationID] * gl_in[i].gl_Position;
			gl_Layer = gl_InvocationID;
			EmitVertex();
		}
		EndPrimitive();
	}
	)";
	findAndReplace(shadowGeometry, "[LIGHTING_BASE]", std::to_string(UNIFORM_LIGHTING_BASE));
	findAndReplace(shadowGeometry, "[SUN_CASCADES]", std::to_string(SUN_NUM_SHADOW_CASCADES));
	findAndReplace(shadowGeometry, "[MAX_POINTS]", std::to_string(MAX_POINTLIGHTS));
	findAndReplace(shadowGeometry, "[MAX_SPOTS]", std::to_string(MAX_SPOTLIGHTS));
	const char* shadowFragment = R"(#version 430 core

	void main(){
		//Empty
	}
	)";
	sunShadowProgram.init(shadowVertex, shadowGeometry.c_str(), shadowFragment);

	buffer.init(9 * sizeof(float), file.attribLength, file.attributes);
	buffer.setAttribute(0, 3);
	buffer.setAttribute(1, 3);
	buffer.setAttribute(2, 3);

	texture.init(file.texWidth, file.texHeight, file.texDepth, GL_LINEAR, (char*)file.texture);
}

//Static multi mesh model single draw.
void MultiModel::draw(Mat4 model){
	shader.use();
	glUniformMatrix4fv(0, 1, false, model.ptr());
	buffer.bind();
	texture.bind(0);

	glDrawArrays(GL_TRIANGLES, 0, buffer.numVertices);
}

//Static multi mesh model single draw.
void MultiModel::drawSunShadows(Mat4 model){
	sunShadowProgram.use();
	glUniformMatrix4fv(0, 1, false, model.ptr());
	buffer.bind();
	texture.bind(0);

	glDrawArrays(GL_TRIANGLES, 0, buffer.numVertices);
}

//------------------------------------------------------------------------------------------------------

void Skybox::init(const char* filename){
	std::string vertex = R"(#version 430 core
	#define UNIFORM_COMMON_BASE [COMMON_BASE]

	layout(location = 0) in vec3 POSITION;
	layout(location = 1) in vec2 UV_COORD;

	out VS_OUT{
		vec2 uv_coord;
	} F;

	layout(std140, binding = UNIFORM_COMMON_BASE) uniform U{
		mat4 projView;
		vec4 posTime;
	};

	void main(){
		gl_Position = projView * vec4(POSITION, 1.0);
		F.uv_coord = UV_COORD;
	}
	)";
	findAndReplace(vertex, "[COMMON_BASE]", std::to_string(UNIFORM_COMMON_BASE));
	std::string fragment = R"(#version 430 core
	#define UNIFORM_COMMON_BASE [COMMON_BASE]

	out vec4 outColor;

	in VS_OUT{
		vec2 uv_coord;
	}F;

	layout(std140, binding = UNIFORM_COMMON_BASE) uniform U{
		mat4 projView;
		vec4 posTime;
	};

	layout(binding = 0) uniform sampler2D u_texture;

	void main(){
		outColor = texture(u_texture, F.uv_coord);
	}
	)";	
	findAndReplace(fragment, "[COMMON_BASE]", std::to_string(UNIFORM_COMMON_BASE));
	program.init(vertex.c_str(), fragment.c_str());

	float vertices[180] = {
		-1.0,  1.0, -1.0, 0.00, 0.33,
		 1.0,  1.0, -1.0, 0.25, 0.33,
		 1.0,  1.0,  1.0, 0.25, 0.67,

		 1.0,  1.0,  1.0, 0.25, 0.67,
		-1.0,  1.0,  1.0, 0.00, 0.67,
		-1.0,  1.0, -1.0, 0.00, 0.33,


		 1.0,  1.0, -1.0, 0.25, 0.33,
		 1.0, -1.0, -1.0, 0.50, 0.33,
		 1.0, -1.0,  1.0, 0.50, 0.67,

		 1.0, -1.0,  1.0, 0.50, 0.67,
		 1.0,  1.0,  1.0, 0.25, 0.67,
		 1.0,  1.0, -1.0, 0.25, 0.33,


		 1.0, -1.0, -1.0, 0.50, 0.33,
		-1.0, -1.0, -1.0, 0.75, 0.33,
		-1.0, -1.0,  1.0, 0.75, 0.67,
		
		-1.0, -1.0,  1.0, 0.75, 0.67,
		 1.0, -1.0,  1.0, 0.50, 0.67,
		 1.0, -1.0, -1.0, 0.50, 0.33,


		-1.0, -1.0, -1.0, 0.75, 0.33,
		-1.0,  1.0, -1.0, 1.00, 0.33,
		-1.0,  1.0,  1.0, 1.00, 0.67,

		-1.0,  1.0,  1.0, 1.00, 0.67,
		-1.0, -1.0,  1.0, 0.75, 0.67,
		-1.0, -1.0, -1.0, 0.75, 0.33,


		 1.0,  1.0,  1.0, 0.25, 0.67,
		 1.0, -1.0,  1.0, 0.50, 0.67,
		-1.0, -1.0,  1.0, 0.50, 1.00,

		-1.0, -1.0,  1.0, 0.50, 1.00,
		-1.0,  1.0,  1.0, 0.25, 1.00,
		 1.0,  1.0,  1.0, 0.25, 0.67,


		-1.0,  1.0, -1.0, 0.25, 0.00,
		-1.0, -1.0, -1.0, 0.50, 0.00,
		 1.0, -1.0, -1.0, 0.50, 0.33,

		 1.0, -1.0, -1.0, 0.50, 0.33,
		 1.0,  1.0, -1.0, 0.25, 0.33,
		-1.0,  1.0, -1.0, 0.25, 0.00
	};
	buffer.init(5 * sizeof(float), 180 * sizeof(float), (char*)vertices);
	buffer.setAttribute(0, 3);
	buffer.setAttribute(1, 2);

	TextureFile file(filename);
	texture.init(file.texWidth, file.texHeight, GL_LINEAR, (char*)file.texture);
}

void Skybox::draw(){
	program.use();
	buffer.bind();
	texture.bind(0);

	glDrawArrays(GL_TRIANGLES, 0, buffer.numVertices);
}

//------------------------------------------------------------------------------

void Animation::init(const char* filename){
	AnimationFile file(filename);

	numFrames = file.numFrames;
	numBones = file.numBones;
	animRate = file.animRate;

	transforms = (Joint*)malloc(file.animLength);
	memcpy(transforms, file.animation, file.animLength);

	duration = (numFrames - 1) / (24.0f / animRate);
}

Animation::~Animation(){
	free(transforms);
}

Mat4 Animation::calcJoint(Uint32 boneIndex, float time){
	if(boneIndex >= numBones){
		return Mat4::identity();
	}

	float frametime = time * (24.0f / animRate);
	if(frametime >= numFrames - 1){
		return Mat4::identity();
	}

	double keyIndex = 0;
	float fraction = modf(frametime, &keyIndex);

	Uint32 last = (Uint32)keyIndex * numBones + boneIndex;
	Uint32 next = (Uint32)(keyIndex + 1) * numBones + boneIndex;

	Vec3 position = Vec3::interpolate(transforms[last].translation, transforms[next].translation, fraction);
	Quat rotation = Quat::slerp(transforms[last].rotation, transforms[next].rotation, fraction);
	Vec3 scale = Vec3::interpolate(transforms[last].scaling, transforms[next].scaling, fraction);

	Mat4 transMat = Mat4::translation(position);
	Mat4 rotMat = rotation.toMatrix();
	Mat4 scaleMat = Mat4::scale(scale);

	Mat4 result = scaleMat * rotMat * transMat;

	return result;
}
