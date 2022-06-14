#include "renderer.hpp"

//Renderer constructor.
int Renderer::init(RendererSettings settings){
	this->settings = settings;

	//Init SDL.
	SDL_Init(SDL_INIT_EVERYTHING);

	//Set SDL window hints.
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, settings.windowGLMajor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, settings.windowGLMinor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	//Create SDL window.
	window = SDL_CreateWindow(settings.windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, settings.windowWidth, settings.windowHeight, SDL_WINDOW_OPENGL|settings.windowFlags);

	//Create OpenGL context.
	context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);

	//Setup OpenGL extensions.
	glewExperimental = true;
	if(glewInit() != GLEW_OK){
		return -1;
	}

	//Set vertical sync.
	SDL_GL_SetSwapInterval(settings.windowVsync);

	//Create deferred rendertarget.
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	//Position and distance data.
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, settings.frameWidth, settings.frameHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	//Normal and roughness data.
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, settings.frameWidth, settings.frameHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	//Albedo and metallic data.
	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, settings.frameWidth, settings.frameHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

	Uint32 attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, attachments);

	//Depth and stencil buffer for depth testing.
	glGenTextures(1, &gDepth);
	glBindTexture(GL_TEXTURE_2D, gDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, settings.frameWidth, settings.frameHeight, false, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Final display framebuffer.
	glGenFramebuffers(1, &displayBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, displayBuffer);

	//Final display image.
	glGenTextures(1, &displayImage);
	glBindTexture(GL_TEXTURE_2D, displayImage);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, settings.frameWidth, settings.frameHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, displayImage, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Setup post processing buffer.
	glGenFramebuffers(1, &postBuffer);
	glGenTextures(1, &postImage);

	glBindFramebuffer(GL_FRAMEBUFFER, postBuffer);

	//Post processing blur framebuffers and images.
	glGenFramebuffers(2, blurBuffer);
	glGenTextures(2, blurImage);

	glBindTexture(GL_TEXTURE_2D, postImage);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, settings.frameWidth, settings.frameHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postImage, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Setup blur buffers.
	glBindFramebuffer(GL_FRAMEBUFFER, blurBuffer[0]);

	glBindTexture(GL_TEXTURE_2D, blurImage[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 256, 144, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurImage[0], 0);

	glBindFramebuffer(GL_FRAMEBUFFER, blurBuffer[1]);

	glBindTexture(GL_TEXTURE_2D, blurImage[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 256, 144, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurImage[1], 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Create empty VAO for dataless shaders (a vao MUST be bound for a draw call to succeed).
	glGenVertexArrays(1, &nullVao);

	//Setup shader programs.
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

	combineProgram.init(
		(glsl_header() + glsl_displayQuadVertex()).c_str(),
		(glsl_header() + glsl_combineFragment()).c_str()
	);

	kernelProgram.init(
		(glsl_header() + glsl_displayQuadVertex()).c_str(),
		(glsl_header() + glsl_kernelFragment()).c_str()
	);

	blurProgram.init(
		(glsl_header() + glsl_displayQuadVertex()).c_str(),
		(glsl_header() + glsl_gaussianBlurFragment()).c_str()
	);

	//Setup common uniforms.
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBlock), &uniforms, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, UBO_BINDING, ubo);

	//Setup shadow maps.
	glGenFramebuffers(1, &shadowBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);

	glGenTextures(1, &shadowImages);
	glBindTexture(GL_TEXTURE_2D_ARRAY, shadowImages);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT, settings.shadowWidth, settings.shadowHeight, NUM_SUN_CASCADES + MAX_SPOTLIGHTS, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_GEQUAL);
	float border[4] = {1.0,1.0,1.0,1.0};
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, border);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowImages, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Light queue stuff.
	numPointlights = 0;
	numSpotlights = 0;

	//Draw queue stuff.
	numRequests = 0;
	mostBones = 0;
	drawQueue = (DrawRequest*)malloc(settings.rendererDrawQueueSize * sizeof(DrawRequest));

	//Camera stuff.
	pitch = 0.0;
	yaw = 0.0;
	camDirection.x = cos(yaw) * cos(pitch);
	camDirection.z = sin(pitch);
	camDirection.y = sin(yaw) * cos(pitch);

	camFrustum = BoundingConvex(frustumCorners, 8);

	return 0;
}

//Renderer destructor.
Renderer::~Renderer(){
	free(drawQueue);

	glDeleteFramebuffers(1, &shadowBuffer);

	glDeleteBuffers(1, &ubo);

	glDeleteTextures(1, &gPosition);
	glDeleteTextures(1, &gNormal);
	glDeleteTextures(1, &gAlbedo);
	glDeleteTextures(1, &gDepth);
	glDeleteTextures(1, &displayImage);
	glDeleteTextures(2, blurImage);
	glDeleteTextures(1, &postImage);
	glDeleteFramebuffers(1, &gBuffer);
	glDeleteFramebuffers(1, &displayBuffer);
	glDeleteFramebuffers(2, blurBuffer);
	glDeleteFramebuffers(1, &postBuffer);

	glDeleteVertexArrays(1, &nullVao);

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

//Toggle window in and out of fullscreen.
void Renderer::toggleWindowFullscreen(){
	if(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP){
		SDL_SetWindowFullscreen(window, 0);
	}else{
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
}

//Bind display buffer.
void Renderer::bindDisplay(){
	glBindFramebuffer(GL_FRAMEBUFFER, displayBuffer);
	glViewport(0, 0, settings.frameWidth, settings.frameHeight);
}

//Deferred lighting pass.
void Renderer::deferredPass(){
	//Calculate shadow projections.
	float scale = 8.0;
	for(int i=0;i<NUM_SUN_CASCADES;i++){
		uniforms.lights.sun.projViewCSM[i] = Mat4::lookAt(
		Vec3::normalize(uniforms.lights.sun.direction) * 200 + (uniforms.common.camPosition),
		(uniforms.common.camPosition), Vec3(0,0,1)) * 
		Mat4::orthographic(-scale, scale, -scale, scale, 0.01, 400.0);
		scale *= 2.0;
	}

	for(int i=0;i<uniforms.lights.numSpotlights;i++){
		uniforms.lights.spotlights[i].projViewCSM = Mat4::lookAt(
			uniforms.lights.spotlights[i].position,
			Vec3::normalize(uniforms.lights.spotlights[i].direction) + uniforms.lights.spotlights[i].position,
			Vec3(0,0,1)
		) * Mat4::perspective(acos(uniforms.lights.spotlights[i].cutOff) * 2.0, 1, 0.01, 100.0);
	}

	//Set all remaining uniforms.
	int width, height;
	SDL_GetWindowSize(window, &width, &height);

	float aspect = (float)width/(float)height;
	//float vFov = 2 * atan(tan(settings.cameraFov*0.5)*aspect);

	uniforms.common.projView = Mat4::lookAt(
			uniforms.common.camPosition, uniforms.common.camPosition + camDirection, Vec3(0.0, 0.0, 1.0)
		) * Mat4::perspective(settings.cameraFov, aspect, 0.1, 100.0);

	uniforms.lights.numPointlights = (float)numPointlights;
	uniforms.lights.numSpotlights = (float)numSpotlights;

	//Update uniforms.
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UniformBlock), &uniforms);

	//Update camera frustum.
	frustumCorners[0] = Vec3(-1, -1, -1);
	frustumCorners[1] = Vec3( 1, -1, -1);
	frustumCorners[2] = Vec3( 1,  1, -1);
	frustumCorners[3] = Vec3(-1,  1, -1);
	frustumCorners[4] = Vec3(-1, -1,  1);
	frustumCorners[5] = Vec3( 1, -1,  1);
	frustumCorners[6] = Vec3( 1,  1,  1);
	frustumCorners[7] = Vec3(-1,  1,  1);

	Mat4 invProjView = uniforms.common.projView.inverse();
	float w = 0;
	for(unsigned int i=0;i<8;i++){
		frustumCorners[i] = invProjView.transform(frustumCorners[i], 1.0, w);
		frustumCorners[i] = frustumCorners[i] / w;
	}

	//Draw queued models.
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glViewport(0, 0, settings.frameWidth, settings.frameHeight);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	Mat4 joints[mostBones];
	BoundingSphere cullSphere(Vec3(0,0,0), 1.0, 1.0);

	for(int i=0;i<numRequests;i++){
		cullSphere.center = drawQueue[i].centroid;
		cullSphere.radius = drawQueue[i].cullRadius;
		if(gjk(camFrustum, cullSphere)){
			glUseProgram(drawQueue[i].gProgram);
			glUniformMatrix4fv(0, 1, false, drawQueue[i].model.ptr());

			glBindVertexArray(drawQueue[i].vao);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D_ARRAY, drawQueue[i].diffuse);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D_ARRAY, drawQueue[i].metalRough);

			if(drawQueue[i].anim != nullptr){
				drawQueue[i].anim->calcJointTransforms(joints, drawQueue[i].animTime);
				glUniformMatrix4fv(2, drawQueue[i].numBones, false, joints[0].ptr());
			}

			glDrawArrays(GL_TRIANGLES, 0, drawQueue[i].numVertices);
		}
	}

	//Clear shadow maps.
	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
	glViewport(0, 0, settings.shadowWidth, settings.shadowHeight);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	for(int i=0;i<NUM_SUN_CASCADES + uniforms.lights.numSpotlights;i++){
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowImages, 0, i);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	BoundingSphere lightSphere(Vec3(0,0,0), 1.0, 1.0);

	//Draw shadow maps.
	for(int i=0;i<numRequests;i++){
		glUseProgram(drawQueue[i].shadowProgram);
		glUniformMatrix4fv(0, 1, false, drawQueue[i].model.ptr());

		if(drawQueue[i].anim != nullptr){
			drawQueue[i].anim->calcJointTransforms(joints, drawQueue[i].animTime);
			glUniformMatrix4fv(2, drawQueue[i].numBones, false, joints[0].ptr());
		}

		glBindVertexArray(drawQueue[i].vao);

		for(int j=0;j<NUM_SUN_CASCADES;j++){
			glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowImages, 0, j);
			glUniformMatrix4fv(1, 1, false, uniforms.lights.sun.projViewCSM[j].ptr());

			glDrawArrays(GL_TRIANGLES, 0, drawQueue[i].numVertices);
		}

		for(int j=0;j<uniforms.lights.numSpotlights;j++){
			lightSphere.center = uniforms.lights.spotlights[j].position;
			lightSphere.radius = uniforms.lights.spotlights[j].radius;
			if(gjk(camFrustum, lightSphere)){
				glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowImages, 0, j + NUM_SUN_CASCADES);
				glUniformMatrix4fv(1, 1, false, uniforms.lights.spotlights[j].projViewCSM.ptr());

				glDrawArrays(GL_TRIANGLES, 0, drawQueue[i].numVertices);
			}
		}
	}

	numRequests = 0;
	mostBones = 0;

	//Deferred pass.
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, settings.frameWidth, settings.frameHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, displayBuffer);

	deferredProgram.use();
	glBindVertexArray(nullVao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D_ARRAY, shadowImages);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Reset light uniforms.
	numPointlights = 0;
	numSpotlights = 0;
}

//Apply bloom to current image.
void Renderer::applyBloom(Uint32 blurPasses){
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, settings.frameBlurWidth, settings.frameBlurHeight);
	glBindVertexArray(nullVao);
	
	moveProgram.use();

	glBindFramebuffer(GL_FRAMEBUFFER, blurBuffer[0]);	

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, displayImage);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Heavily blur resulting image.
	blurProgram.use();
	
	int horizontal = 0;
	for(int i=0;i<blurPasses*2;i++){
		glBindFramebuffer(GL_FRAMEBUFFER, blurBuffer[1 - horizontal]);
		glUniform1i(1, horizontal);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, blurImage[horizontal]);

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

	glViewport(0, 0, settings.frameWidth, settings.frameHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, postBuffer);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, displayImage);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, blurImage[0]);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	//Push final image into display buffer.
	glBindFramebuffer(GL_FRAMEBUFFER, displayBuffer);

	moveProgram.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postImage);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

//Display the final image on screen.
void Renderer::displayFrame(){
	deferredPass();

	if(settings.frameBloom > 0)
		applyBloom(settings.frameBloom);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	int width, height;
	SDL_GetWindowSize(window, &width, &height);

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	displayProgram.use();
	glBindVertexArray(nullVao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, displayImage);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	SDL_GL_SwapWindow(window);
}

//Add a pointlight.
void Renderer::pushLight(Pointlight light){
	if(uniforms.lights.numPointlights < MAX_POINTLIGHTS){
		uniforms.lights.pointlights[numPointlights++] = light;
	}else{
		std::cout<<"WARNING: Pointlight capacity full. Cannot add more."<<std::endl;
	}
}

//Add a spotlight.
void Renderer::pushLight(Spotlight light){
	if(uniforms.lights.numSpotlights < MAX_SPOTLIGHTS){
		uniforms.lights.spotlights[numSpotlights++] = light;
	}else{
		std::cout<<"WARNING: Spotlight capacity full. Cannot add more."<<std::endl;
	}
}

//Add static model to the draw queue.
void Renderer::drawModel(StaticModel* mesh, Mat4 model){
	if(numRequests < settings.rendererDrawQueueSize){
		DrawRequest request;
		request.gProgram = mesh->gProgram.program;
		request.shadowProgram = mesh->shadowProgram.program;
		request.vao = mesh->vao;
		request.numVertices = mesh->numVertices;
		request.diffuse = mesh->diffuse;
		request.metalRough = mesh->metalRough;
		request.anim = nullptr;
		request.numBones = 0;
		request.animTime = 0.0;
		request.model = model;
		request.centroid = model * mesh->centroid;
		request.cullRadius = mesh->cullRadius;

		drawQueue[numRequests++] = request;
	}
}

//Add static model to the draw queue.
void Renderer::drawModel(AnimatedModel* mesh, Mat4 model, Animation* anim, float animTime){
	if(numRequests < settings.rendererDrawQueueSize){
		DrawRequest request;
		request.gProgram = mesh->gProgram.program;
		request.shadowProgram = mesh->shadowProgram.program;
		request.vao = mesh->vao;
		request.numVertices = mesh->numVertices;
		request.diffuse = mesh->diffuse;
		request.metalRough = mesh->metalRough;
		request.anim = anim;
		request.numBones = mesh->numBones;
		request.animTime = animTime;
		request.model = model;
		request.centroid = model * mesh->centroid;
		request.cullRadius = mesh->cullRadius;

		drawQueue[numRequests++] = request;

		mostBones = std::max(mostBones, request.numBones);
	}
}

//Set camera heading.
void Renderer::setCameraView(float yaw, float pitch){
	this->yaw = yaw;
	this->pitch = pitch;
	
	if(pitch > 1.56){pitch = 1.56;}
	else if(pitch < -1.56){pitch = -1.56;}

	camDirection.x = cos(yaw) * cos(pitch);
	camDirection.z = sin(pitch);
	camDirection.y = sin(yaw) * cos(pitch);
}

//Update camera heading.
void Renderer::updateCameraView(float Xrelative, float Yrelative){
	yaw -= Xrelative * settings.cameraSensitivity;
	pitch -= Yrelative * settings.cameraSensitivity;
	
	if(pitch > 1.56){pitch = 1.56;}
	else if(pitch < -1.56){pitch = -1.56;}

	camDirection.x = cos(yaw) * cos(pitch);
	camDirection.z = sin(pitch);
	camDirection.y = sin(yaw) * cos(pitch);
}

//Get camera direction.
Vec3 Renderer::getCameraDirection(){
	return camDirection;
}

//Get camera right direction.
Vec3 Renderer::getCameraRight(){
	return Vec3::normalize(Vec3::cross(camDirection, Vec3(0,0,1)));
}

//Get camera front direction.
Vec3 Renderer::getCameraFront(){
	return Vec3::normalize(Vec3(camDirection.x, camDirection.y, 0.0));
}
