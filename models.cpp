#include "models.hpp"
#include "loaders.hpp"

bool Animation::init(const char* filename){
	AnimationLoader file(filename);
	if(!file.loaded){
		return false;
	}

	numFrames = file.numFrames;
	numBones = file.numBones;
	animRate = file.animRate;

	transforms = (Joint*)malloc(file.animLength);
	memcpy(transforms, file.animation, file.animLength);

	duration = (numFrames - 1) / (24.0f / animRate);

	return true;
}

Animation::~Animation(){
	if(transforms){
		free(transforms);
	}
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

//---------------------------------------------------------------------------------------------

//Create a drawable 3d model.
bool StaticModel::init(const char* filename){
	StaticModelLoader file(filename);
	if(!file.loaded){
		return false;
	}

	gProgram.init(
		(glsl_header() + glsl_commonUniforms() + glsl_deferredStaticModelVertex()).c_str(),
		(glsl_header() + glsl_commonUniforms() + glsl_deferredAllModelFragment()).c_str()
	);

	shadowProgram.init(
		(glsl_header() + glsl_staticModelShadowVertex()).c_str(),
		(glsl_header() + glsl_commonUniforms() + glsl_allModelShadowFragment()).c_str()
	);

	numVertices = file.attribLength / (9 * sizeof(float));

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, file.attribLength, file.attributes, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 9 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, 9 * sizeof(float), (void*)(6 * sizeof(float)));

	glGenTextures(1, &diffuse);
	glBindTexture(GL_TEXTURE_2D_ARRAY, diffuse);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA32F, file.texWidth, file.texHeight, file.texDepth, false, GL_RGBA, GL_FLOAT, file.diffuse);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &metalRough);
	glBindTexture(GL_TEXTURE_2D_ARRAY, metalRough);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RG32F, 1, 1, file.texDepth, false, GL_RG, GL_FLOAT, file.metalRough);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

//Static model destructor.
StaticModel::~StaticModel(){
	glDeleteTextures(1, &metalRough);
	glDeleteTextures(1, &diffuse);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

//Draw the 3d model onto g-buffers.
void StaticModel::draw(Mat4 model){
	gProgram.use();
	glUniformMatrix4fv(0, 1, false, model.ptr());

	glBindVertexArray(vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, diffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, metalRough);

	glDrawArrays(GL_TRIANGLES, 0, numVertices);
}

//Draw the 3d model onto a shadowmap.
void StaticModel::drawShadow(Mat4 model, Mat4 view){
	shadowProgram.use();
	glUniformMatrix4fv(0, 1, false, model.ptr());
	glUniformMatrix4fv(1, 1, false, view.ptr());

	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLES, 0, numVertices);
}

//------------------------------------------------------------------------------------

//Create a drawable 3d model.
bool AnimatedModel::init(const char* filename){
	AnimatedModelLoader file(filename);
	if(!file.loaded){
		return false;
	}

	numBones = file.numBones;

	gProgram.init(
		(glsl_header() + glsl_commonUniforms() + glsl_deferredAnimatedModelVertex(numBones)).c_str(),
		(glsl_header() + glsl_commonUniforms() + glsl_deferredAllModelFragment()).c_str()
	);

	shadowProgram.init(
		(glsl_header() + glsl_animatedModelShadowVertex(numBones)).c_str(),
		(glsl_header() + glsl_emptyShader()).c_str()
	);

	numVertices = file.attribLength / (17 * sizeof(float));

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, file.attribLength, file.attributes, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 17 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 17 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, 17 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, false, 17 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, false, 17 * sizeof(float), (void*)(13 * sizeof(float)));

	glGenTextures(1, &diffuse);
	glBindTexture(GL_TEXTURE_2D_ARRAY, diffuse);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA32F, file.texWidth, file.texHeight, file.texDepth, false, GL_RGBA, GL_FLOAT, file.diffuse);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &metalRough);
	glBindTexture(GL_TEXTURE_2D_ARRAY, metalRough);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RG32F, 1, 1, file.texDepth, false, GL_RG, GL_FLOAT, file.metalRough);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	joints = (Mat4*)malloc(numBones * sizeof(Mat4));
	for(unsigned int i=0;i<numBones;i++){
		joints[i] = Mat4::identity();
	}

	return true;
}

//Animated model destructor.
AnimatedModel::~AnimatedModel(){
	free(joints);
	glDeleteTextures(1, &metalRough);
	glDeleteTextures(1, &diffuse);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

//Pose animated model.
void AnimatedModel::pose(Animation& anim, float time){
	for(unsigned int i=0;i<numBones;i++){
		joints[i] = anim.calcJoint(i, time);
	}
}	

//Draw the 3d model onto g-buffers.
void AnimatedModel::draw(Mat4 model){
	gProgram.use();
	glUniformMatrix4fv(0, 1, false, model.ptr());
	glUniformMatrix4fv(2, numBones, false, joints[0].ptr());

	glBindVertexArray(vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, diffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, metalRough);

	glDrawArrays(GL_TRIANGLES, 0, numVertices);
}

//Draw the 3d model onto a shadowmap.
void AnimatedModel::drawShadow(Mat4 model, Mat4 view){
	shadowProgram.use();
	glUniformMatrix4fv(0, 1, false, model.ptr());
	glUniformMatrix4fv(1, 1, false, view.ptr());
	glUniformMatrix4fv(2, numBones, false, joints[0].ptr());

	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLES, 0, numVertices);
}

//------------------------------------------------------------------------------------

//Physics mesh init.
bool PhysicsMesh::init(const char* filename){
	PhysicsMeshLoader file(filename);
	if(!file.loaded){
		return false;
	}

	numConvexes = file.numConvexes;
	
	vertices = (Vec3*)malloc(file.vertsLength);
	memcpy(vertices, file.vertices, file.vertsLength);

	indices = (Uint16*)malloc(file.indsLength);
	memcpy(indices, file.indices, file.indsLength);

	convexes = (BoundingConvex*)malloc(numConvexes * sizeof(BoundingConvex));
	Uint32 counter = 0;
	for(unsigned int i=0;i<numConvexes;i++){
		convexes[i] = BoundingConvex(&vertices[counter], indices[i]);
		counter += indices[i];
	}

	return true;
}

//Physics mesh destructor.
PhysicsMesh::~PhysicsMesh(){
	if(vertices){
		free(convexes);
		free(vertices);
		free(indices);
	}
}

//------------------------------------------------------------------------------------------------------

