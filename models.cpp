#include "models.hpp"
#include "loaders.hpp"

//Create a drawable 3d model.
void StaticModel::init(const char* filename){
	StaticModelLoader file(filename);

	gProgram.init(
		(glsl_header() + glsl_commonUniforms() + glsl_deferredStaticModelVertex()).c_str(),
		(glsl_header() + glsl_deferredStaticModelFragment()).c_str()
	);

	shadowProgram.init(
		(glsl_header() + glsl_staticModelShadowVertex()).c_str(),
		(glsl_header() + glsl_commonLightStructs() + glsl_staticModelShadowGeometry()).c_str(),
		(glsl_header() + glsl_emptyShader()).c_str()
	);

	buffer.init(9 * sizeof(float), file.attribLength, file.attributes);
	buffer.setAttribute(0, 3);
	buffer.setAttribute(1, 3);
	buffer.setAttribute(2, 3);

	material.init(file.texWidth, file.texHeight, file.texDepth, GL_REPEAT, GL_LINEAR, file.diffuse);
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
void StaticModel::drawShadow(Mat4 model){
	shadowProgram.use();
	glUniformMatrix4fv(0, 1, false, model.ptr());
	buffer.bind();
	material.bind(0);

	glDrawArrays(GL_TRIANGLES, 0, buffer.numVertices);
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
