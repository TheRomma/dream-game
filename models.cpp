#include "models.hpp"
#include "loaders.hpp"

void Animation::init(const char* filename){
	AnimationLoader file(filename);

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

//---------------------------------------------------------------------------------------------

//Create a drawable 3d model.
void StaticModel::init(const char* filename){
	StaticModelLoader file(filename);

	gProgram.init(
		(glsl_header() + glsl_commonUniforms() + glsl_deferredStaticModelVertex()).c_str(),
		(glsl_header() + glsl_deferredAllModelFragment()).c_str()
	);

	shadowProgram.init(
		(glsl_header() + glsl_staticModelShadowVertex()).c_str(),
		//(glsl_header() + glsl_commonUniforms() + glsl_commonLightStructs() + glsl_allModelShadowGeometry()).c_str(),
		(glsl_header() + glsl_emptyShader()).c_str()
	);

	buffer.init(9 * sizeof(float), file.attribLength, file.attributes);
	buffer.setAttribute(0, 3);
	buffer.setAttribute(1, 3);
	buffer.setAttribute(2, 3);

	material.init(file.texWidth, file.texHeight, file.texDepth, GL_REPEAT, GL_LINEAR, file.diffuse, file.metalRough);
}

//Draw the 3d model onto g-buffers.
void StaticModel::draw(Mat4 model){
	gProgram.use();
	glUniformMatrix4fv(0, 1, false, model.ptr());
	buffer.bind();
	material.bind(0);

	glDrawArrays(GL_TRIANGLES, 0, buffer.numVertices);
}

//Draw the 3d model onto g-buffers.
void StaticModel::drawShadow(Mat4 model, LightUniforms& lights){
	shadowProgram.use();
	glUniformMatrix4fv(0, 1, false, model.ptr());
	buffer.bind();
	material.bind(0);

	Uint32 offset = 0;
	for(unsigned int i=0;i<SUN_NUM_SHADOW_CASCADES;i++){
		lights.shadows.bindLayer(i);
		glUniformMatrix4fv(1, 1, false, lights.block.sun.projViewCSM[i].ptr());

		glDrawArrays(GL_TRIANGLES, 0, buffer.numVertices);
	}

	offset += SUN_NUM_SHADOW_CASCADES;
	for(unsigned int i=0;i<lights.block.numSpotlights;i++){
		lights.shadows.bindLayer(offset + i);
		glUniformMatrix4fv(1, 1, false, lights.block.spotlights[i].projViewCSM.ptr());

		glDrawArrays(GL_TRIANGLES, 0, buffer.numVertices);
	}
}

//------------------------------------------------------------------------------------

//Create a drawable 3d model.
void AnimatedModel::init(const char* filename){
	AnimatedModelLoader file(filename);

	numBones = file.numBones;

	gProgram.init(
		(glsl_header() + glsl_commonUniforms() + glsl_deferredAnimatedModelVertex(numBones)).c_str(),
		(glsl_header() + glsl_deferredAllModelFragment()).c_str()
	);

	shadowProgram.init(
		(glsl_header() + glsl_animatedModelShadowVertex(numBones)).c_str(),
		//(glsl_header() + glsl_commonUniforms() + glsl_commonLightStructs() + glsl_allModelShadowGeometry()).c_str(),
		(glsl_header() + glsl_emptyShader()).c_str()
	);

	buffer.init(17 * sizeof(float), file.attribLength, file.attributes);
	buffer.setAttribute(0, 3);
	buffer.setAttribute(1, 3);
	buffer.setAttribute(2, 3);
	buffer.setAttribute(3, 4);
	buffer.setAttribute(4, 4);

	material.init(file.texWidth, file.texHeight, file.texDepth, GL_REPEAT, GL_LINEAR, file.diffuse, file.metalRough);

	joints = (Mat4*)malloc(numBones * sizeof(Mat4));
	for(unsigned int i=0;i<numBones;i++){
		joints[i] = Mat4::identity();
	}
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
	glUniformMatrix4fv(1, numBones, false, joints[0].ptr());
	buffer.bind();
	material.bind(0);

	glDrawArrays(GL_TRIANGLES, 0, buffer.numVertices);
}

//Draw the 3d model onto g-buffers.
void AnimatedModel::drawShadow(Mat4 model, LightUniforms& lights){
	shadowProgram.use();
	glUniformMatrix4fv(0, 1, false, model.ptr());
	glUniformMatrix4fv(2, numBones, false, joints[0].ptr());
	buffer.bind();
	material.bind(0);

	Uint32 offset = 0;
	for(unsigned int i=0;i<SUN_NUM_SHADOW_CASCADES;i++){
		lights.shadows.bindLayer(i);
		glUniformMatrix4fv(1, 1, false, lights.block.sun.projViewCSM[i].ptr());

		glDrawArrays(GL_TRIANGLES, 0, buffer.numVertices);
	}

	offset += SUN_NUM_SHADOW_CASCADES;
	for(unsigned int i=0;i<lights.block.numSpotlights;i++){
		lights.shadows.bindLayer(offset + i);
		glUniformMatrix4fv(1, 1, false, lights.block.spotlights[i].projViewCSM.ptr());

		glDrawArrays(GL_TRIANGLES, 0, buffer.numVertices);
	}
}

//------------------------------------------------------------------------------------

//Physics mesh init.
void PhysicsMesh::init(const char* filename){
	PhysicsMeshLoader file(filename);

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
