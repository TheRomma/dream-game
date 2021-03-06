#include "graphics.hpp"
//Create a common vertex buffer.
void VertexBuffer::init(Uint32 stride, Uint32 length, float* data){
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, length, data, GL_STATIC_DRAW);

	this->stride = stride;
	attribLength = 0;
	numVertices = length / stride;
}

//Destructor for vertex buffer.
VertexBuffer::~VertexBuffer(){
	if(vao){
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}
}

//Bind vertex array.
void VertexBuffer::bind(){
	glBindVertexArray(vao);
}

//Set vertex array attribute length.
void VertexBuffer::setAttribute(Uint32 location, Uint32 size){
	if(attribLength < stride){
		glVertexAttribPointer(location, size, GL_FLOAT, false, stride, (void*)attribLength);
		glEnableVertexAttribArray(location);
		attribLength += size * sizeof(float);
	}else{
		std::cout<<"WARNING: Vertex buffer attributes over stride!"<<std::endl;
	}
}
/*
//-------------------------------------------------------------------------------------------

//Create a common material.
void Material::init(Uint32 width, Uint32 height, Uint32 depth, Uint32 wrap,
		Uint32 filter, float* diffSource, float* metalRoughSource){

	glGenTextures(1, &diffuse);
	glBindTexture(GL_TEXTURE_2D_ARRAY, diffuse);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA32F, width, height, depth, false, GL_RGBA, GL_FLOAT, diffSource);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, filter);

	glGenTextures(1, &metalRough);
	glBindTexture(GL_TEXTURE_2D_ARRAY, metalRough);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RG32F, 1, 1, depth, false, GL_RG, GL_FLOAT, metalRoughSource);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, filter);
}

//Destructor for materials..
Material::~Material(){
	if(diffuse){
		glDeleteTextures(1, &diffuse);
		glDeleteTextures(1, &metalRough);
	}
}

//Bind materials.
void Material::bind(Uint32 slot){
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D_ARRAY, diffuse);
	glActiveTexture(GL_TEXTURE1 + slot);
	glBindTexture(GL_TEXTURE_2D_ARRAY, metalRough);
}
//------------------------------------------------------------------------

//Create a shader buffer such as UBO or SSBO.
void ShaderBuffer::init(Uint32 type, Uint32 base, Uint32 length, char* data){
	this->type = type;
	this->base = base;

	glGenBuffers(1, &buffer);
	glBindBuffer(type, buffer);
	glBufferData(type, length, data, GL_DYNAMIC_DRAW);
	glBindBufferBase(type, base, buffer);
}

//ShaderBuffer dest.
ShaderBuffer::~ShaderBuffer(){
	if(buffer){
		glDeleteBuffers(1, &buffer);
	}
}

//Bind ShaderBuffer.
void ShaderBuffer::bind(){
	glBindBuffer(type, buffer);
}

//Write to ShaderBuffer.
void ShaderBuffer::write(Uint32 offset, Uint32 length, char* data){
	glBindBuffer(type, buffer);
	glBufferSubData(type, offset, length, data);
}

//Rebind the base of ShaderBuffer.
void ShaderBuffer::bindBase(Uint32 base){
	glBindBufferBase(type, base, buffer);
}
*/
//-------------------------------------------------------------------------------------------
/*
//Create a ubo for common uniforms.
CommonUniforms::CommonUniforms(){
	ubo.init(GL_UNIFORM_BUFFER, UBO_COMMON_BASE, sizeof(CommonBlock), nullptr);
}

//Bind ubo.
void CommonUniforms::bind(){
	ubo.bind();
}

//Write to ubo.
void CommonUniforms::write(){
	ubo.write(0, sizeof(CommonBlock), (char*)&block);
}
*/
//-------------------------------------------------------------------------------------------
/*
//CSM init.
void CSM::init(Uint32 width, Uint32 height, Uint32 numMaps){
	this->width = width;
	this->height = height;
	this->numMaps = numMaps;

	glGenFramebuffers(1, &frame);
	glBindFramebuffer(GL_FRAMEBUFFER, frame);

	glGenTextures(1, &depth);
	glBindTexture(GL_TEXTURE_2D_ARRAY, depth);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT, width, height, numMaps, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float border[4] = {1.0,1.0,1.0,1.0};
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, border);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//CSM dest,
CSM::~CSM(){
	if(frame){
		glDeleteTextures(1, &depth);
		glDeleteFramebuffers(1, &frame);
	}
}

//Bind framebuffer.
void CSM::bind(){
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, frame);
}

//Bind depth map.
void CSM::bindTexture(Uint32 base){
	glActiveTexture(GL_TEXTURE0 + base);
	glBindTexture(GL_TEXTURE_2D_ARRAY, depth);
}

//Bind depth map.
void CSM::bindLayer(Uint32 layer){
	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth, 0, layer);
}

//Clear shadow map.
void CSM::clear(Uint32 numSun, Uint32 numSpot){
	Uint32 offset = 0;
	for(unsigned int i=0;i<numSun;i++){
		bindLayer(i);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	offset += numSun;
	for(unsigned int i=0;i<numSpot;i++){
		bindLayer(offset + i);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
}
*/
//-------------------------------------------------------------------------

#include "loaders.hpp"
//A cube map for environment mapping and skyboxes.
void EnvironmentMap::init(const char* filename){
	EnvironmentMapLoader file(filename);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	Uint32 offset = 0;
	/*
	for(unsigned int i=0;i<6;i++){
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, file.texWidth, file.texHeight, 0, GL_RGB, GL_FLOAT, file.diffuse + offset);
		offset += file.sideLength;
	}
	*/
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB32F, file.texWidth, file.texHeight, 0, GL_RGB, GL_FLOAT, file.diffuse + offset);

	offset += file.sideLength;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB32F, file.texWidth, file.texHeight, 0, GL_RGB, GL_FLOAT, file.diffuse + offset);

	offset += file.sideLength;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB32F, file.texWidth, file.texHeight, 0, GL_RGB, GL_FLOAT, file.diffuse + offset);

	offset += file.sideLength;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB32F, file.texWidth, file.texHeight, 0, GL_RGB, GL_FLOAT, file.diffuse + offset);

	offset += file.sideLength;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB32F, file.texWidth, file.texHeight, 0, GL_RGB, GL_FLOAT, file.diffuse + offset);

	offset += file.sideLength;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB32F, file.texWidth, file.texHeight, 0, GL_RGB, GL_FLOAT, file.diffuse + offset);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	program.init(
		(glsl_header() + glsl_commonUniforms() + glsl_displayCubeVertex()).c_str(),
		(glsl_header() + glsl_commonUniforms() + glsl_environmentFragment()).c_str()
	);

	float vertices[108] = {
		-1.0,  1.0, -1.0,
		-1.0, -1.0, -1.0,
		 1.0, -1.0, -1.0,
		 1.0, -1.0, -1.0,
		 1.0,  1.0, -1.0,
		-1.0,  1.0, -1.0,

		-1.0, -1.0,  1.0,
		-1.0, -1.0, -1.0,
		-1.0,  1.0, -1.0,
		-1.0,  1.0, -1.0,
		-1.0,  1.0,  1.0,
		-1.0, -1.0,  1.0,

		 1.0, -1.0, -1.0,
		 1.0, -1.0,  1.0,
		 1.0,  1.0,  1.0,
		 1.0,  1.0,  1.0,
		 1.0,  1.0, -1.0,
		 1.0, -1.0, -1.0,

		-1.0, -1.0,  1.0,
		-1.0,  1.0,  1.0,
		 1.0,  1.0,  1.0,
		 1.0,  1.0,  1.0,
		 1.0, -1.0,  1.0,
		-1.0, -1.0,  1.0,

		-1.0,  1.0, -1.0,
		 1.0,  1.0, -1.0,
		 1.0,  1.0,  1.0,
		 1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		-1.0,  1.0, -1.0,

		-1.0, -1.0, -1.0,
		-1.0, -1.0,  1.0,
		 1.0, -1.0, -1.0,
		 1.0, -1.0, -1.0,
		-1.0, -1.0,  1.0,
		 1.0, -1.0,  1.0
	};
	
	buffer.init(3*sizeof(float), 108*sizeof(float), vertices);
	buffer.setAttribute(0, 3);
}

//Environment map destructor.
EnvironmentMap::~EnvironmentMap(){
	if(texture){
		glDeleteTextures(1, &texture);
	}
}

//Bind environment map.
void EnvironmentMap::bind(Uint32 slot){
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
}

//Display map.
void EnvironmentMap::display(){
	program.use();
	buffer.bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

//-------------------------------------------------------------------------

//Pure shader only sky.
void ProceduralSky::init(){
	program.init(
		(glsl_header() + glsl_commonUniforms() + glsl_displayCubeVertex()).c_str(),
		(glsl_header() + glsl_commonUniforms() + glsl_skyFragment()).c_str()
	);

	float vertices[108] = {
		-1.0,  1.0, -0.1,
		-1.0, -1.0, -0.1,
		 1.0, -1.0, -0.1,
		 1.0, -1.0, -0.1,
		 1.0,  1.0, -0.1,
		-1.0,  1.0, -0.1,

		-1.0, -1.0,  0.1,
		-1.0, -1.0, -0.1,
		-1.0,  1.0, -0.1,
		-1.0,  1.0, -0.1,
		-1.0,  1.0,  0.1,
		-1.0, -1.0,  0.1,

		 1.0, -1.0, -0.1,
		 1.0, -1.0,  0.1,
		 1.0,  1.0,  0.1,
		 1.0,  1.0,  0.1,
		 1.0,  1.0, -0.1,
		 1.0, -1.0, -0.1,

		-1.0, -1.0,  0.1,
		-1.0,  1.0,  0.1,
		 1.0,  1.0,  0.1,
		 1.0,  1.0,  0.1,
		 1.0, -1.0,  0.1,
		-1.0, -1.0,  0.1,

		-1.0,  1.0, -0.1,
		 1.0,  1.0, -0.1,
		 1.0,  1.0,  0.1,
		 1.0,  1.0,  0.1,
		-1.0,  1.0,  0.1,
		-1.0,  1.0, -0.1,

		-1.0, -1.0, -0.1,
		-1.0, -1.0,  0.1,
		 1.0, -1.0, -0.1,
		 1.0, -1.0, -0.1,
		-1.0, -1.0,  0.1,
		 1.0, -1.0,  0.1
	};
	
	buffer.init(3*sizeof(float), 108*sizeof(float), vertices);
	buffer.setAttribute(0, 3);
}

//Draw sky.
void ProceduralSky::draw(){
	program.use();
	buffer.bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

//-------------------------------------------------------------------------
/*
//LightUniforms init.
LightUniforms::LightUniforms(){
	ubo.init(GL_UNIFORM_BUFFER, UBO_LIGHT_BASE, sizeof(LightBlock), nullptr);
	this->reset();
}

//Bind uniform buffer.
void LightUniforms::bind(){
	ubo.bind();
}

//Reset number of lights.
void LightUniforms::reset(){
	block.numPointlights = block.numStaticPointlights;
	block.numSpotlights = block.numStaticSpotlights;
}

//Add a pointlight.
void LightUniforms::push(Pointlight light){
	if(block.numPointlights < MAX_POINTLIGHTS){
		block.pointlights[(Uint32)block.numPointlights] = light;
		block.numPointlights++;
	}else{
		std::cout<<"WARNING: Pointlight capacity full. Cannot add more."<<std::endl;
	}
}

//Add a spotlight.
void LightUniforms::push(Spotlight light){
	if(block.numSpotlights < MAX_SPOTLIGHTS){
		block.spotlights[(Uint32)block.numSpotlights] = light;
		block.numSpotlights++;
	}else{
		std::cout<<"WARNING: Spotlight capacity full. Cannot add more."<<std::endl;
	}
}

//Write to uniform buffer.
void LightUniforms::write(){
	ubo.write(0, sizeof(LightBlock), (char*)&block);
	//this->reset();
}

//Calculate all shadow projections.
void LightUniforms::calcShadowProjections(Vec3 position, Vec3 direction){

	float scale = 8.0;
	for(unsigned int i=0;i<NUM_SUN_CASCADES;i++){
		block.sun.projViewCSM[i] = Mat4::lookAt(
		Vec3::normalize(block.sun.direction) * 200 + (position + direction * scale * 0.5),
		(position + direction * scale * 0.5), Vec3(0,0,1)) * 
		Mat4::orthographic(-scale, scale, -scale, scale, 0.1, 400.0);
		scale *= 2.0;
	}

	for(unsigned int i=0;i<block.numSpotlights;i++){
		block.spotlights[i].projViewCSM = Mat4::lookAt(
			block.spotlights[i].position,
			Vec3::normalize(block.spotlights[i].direction) + block.spotlights[i].position,
			Vec3(0,0,1)
		) * Mat4::perspective(acos(block.spotlights[i].cutOff) * 2.0, 1, 0.01, 100.0);
	}
}
*/
//--------------------------------------------------------------------------------------------------
/*
DeferredTarget::DeferredTarget(Uint32 width, Uint32 height){
	this->width = width;
	this->height = height;

	//Lighting pass framebuffer.
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	//Position and distance data.
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	//Normal and roughness data.
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	//Albedo and metallic data.
	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

	Uint32 attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, attachments);

	//Depth and stencil buffer for depth testing.
	glGenTextures(1, &depthStencil);
	glBindTexture(GL_TEXTURE_2D, depthStencil);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, false, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencil, 0);

	//Final display framebuffer.
	glGenFramebuffers(1, &displayBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, displayBuffer);

	//Final display image.
	glGenTextures(1, &displayImage);
	glBindTexture(GL_TEXTURE_2D, displayImage);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, displayImage, 0);

	//Post processing frame.
	glGenFramebuffers(4, postBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, postBuffer[0]);

	//Post processing image.
	glGenTextures(4, postImage);

	glBindTexture(GL_TEXTURE_2D, postImage[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postImage[0], 0);

	glBindFramebuffer(GL_FRAMEBUFFER, postBuffer[1]);

	glBindTexture(GL_TEXTURE_2D, postImage[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postImage[1], 0);

	glBindFramebuffer(GL_FRAMEBUFFER, postBuffer[2]);

	glBindTexture(GL_TEXTURE_2D, postImage[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 320, 180, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postImage[2], 0);

	glBindFramebuffer(GL_FRAMEBUFFER, postBuffer[3]);

	glBindTexture(GL_TEXTURE_2D, postImage[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 320, 180, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postImage[3], 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Setup programs.
	deferredProgram.init(
		(glsl_header() + glsl_displayQuadVertex()).c_str(),
		(glsl_header() + glsl_commonUniforms() +
			glsl_lightCalculations() + glsl_deferredLightPassFragment()).c_str()
	);

	displayProgram.init(
		(glsl_header() + glsl_displayQuadVertex()).c_str(),
		(glsl_header() + glsl_commonUniforms() + glsl_displayAndToneFragment()).c_str()
	);

	moveProgram.init(
		(glsl_header() + glsl_displayQuadVertex()).c_str(),
		(glsl_header() + glsl_displayQuadFragment()).c_str()
	);

	bloomProgram.init(
		(glsl_header() + glsl_displayQuadVertex()).c_str(),
		(glsl_header() + glsl_bloomFragment()).c_str()
	);

	kernelProgram.init(
		(glsl_header() + glsl_displayQuadVertex()).c_str(),
		(glsl_header() + glsl_kernelFragment()).c_str()
	);

	gaussianBlurProgram.init(
		(glsl_header() + glsl_displayQuadVertex()).c_str(),
		(glsl_header() + glsl_gaussianBlurFragment()).c_str()
	);

	combineProgram.init(
		(glsl_header() + glsl_displayQuadVertex()).c_str(),
		(glsl_header() + glsl_combineFragment()).c_str()
	);

	//Setup vertex buffer.
	float vertices[24] = {
		-1.0, -1.0, 0.0, 0.0,
		 1.0, -1.0, 1.0, 0.0,
		 1.0,  1.0, 1.0, 1.0,

		 1.0,  1.0, 1.0, 1.0,
		-1.0,  1.0, 0.0, 1.0,
		-1.0, -1.0, 0.0, 0.0
	};
	buffer.init(4 * sizeof(float), 24 * sizeof(float), vertices);
	buffer.setAttribute(0, 2);
	buffer.setAttribute(1, 2);
}

//Deferred target destructor.
DeferredTarget::~DeferredTarget(){
	if(gBuffer){
		glDeleteTextures(1, &gPosition);
		glDeleteTextures(1, &gNormal);
		glDeleteTextures(1, &gAlbedo);
		glDeleteTextures(1, &depthStencil);
		glDeleteTextures(1, &displayImage);
		glDeleteTextures(4, postImage);
		glDeleteFramebuffers(1, &gBuffer);
		glDeleteFramebuffers(1, &displayBuffer);
		glDeleteFramebuffers(4, postBuffer);
	}
}

//Bind deferred rendering program.
void DeferredTarget::bind(){
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glViewport(0, 0, width, height);
}

//Bind framebuffer.
void DeferredTarget::bindDisplay(){
	glBindFramebuffer(GL_FRAMEBUFFER, displayBuffer);
	glViewport(0, 0, width, height);
}

//Deferred rendering pass.
void DeferredTarget::draw(){
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, displayBuffer);

	deferredProgram.use();
	buffer.bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DeferredTarget::applyBloom(Uint32 blurPasses){
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, 320, 180);
	buffer.bind();
	
	//Separate overflowing colors.
	//bloomProgram.use();
	moveProgram.use();

	glBindFramebuffer(GL_FRAMEBUFFER, postBuffer[2]);	

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, displayImage);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Heavily blur resulting image.
	gaussianBlurProgram.use();
	
	
	unsigned int horizontal = 0;
	glBindFramebuffer(GL_FRAMEBUFFER, postBuffer[1 - horizontal]);
	glUniform1i(1, horizontal);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, displayImage);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	horizontal++;
	if(horizontal > 1){
		horizontal = 0;
	}
	

	int horizontal = 0;
	for(int i=0;i<blurPasses*2;i++){
		glBindFramebuffer(GL_FRAMEBUFFER, postBuffer[3 - horizontal]);
		glUniform1i(1, horizontal);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, postImage[2 + horizontal]);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		horizontal++;
		if(horizontal > 1){
			horizontal = 0;
		}
	}
	
	//Combine results with original image.
	combineProgram.use();
	glUniform1f(0, 0.85);
	glUniform1f(1, 0.15);

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, postBuffer[1]);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, displayImage);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, postImage[2]);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	//Push final image into display buffer.
	glBindFramebuffer(GL_FRAMEBUFFER, displayBuffer);

	moveProgram.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postImage[1]);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DeferredTarget::applyKernel(float* kernel){
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, postBuffer[0]);
	
	kernelProgram.use();
	buffer.bind();

	glUniformMatrix3fv(1, 1, 0, kernel);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, displayImage);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, displayBuffer);

	moveProgram.use();
	buffer.bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postImage[0]);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

//Display the drawn image on screen.
void DeferredTarget::display(Uint32 displayWidth, Uint32 displayHeight){
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, displayWidth, displayHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	displayProgram.use();
	buffer.bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, displayImage);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}
*/
//-----------------------------------------------------------------------------------------
