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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, settings.frameWidth, settings.frameHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	//Albedo and metallic data.
	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, settings.frameWidth, settings.frameHeight, 0, GL_RGBA, GL_FLOAT, NULL);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, settings.frameWidth, settings.frameHeight, 0, GL_RGB, GL_FLOAT, NULL);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, settings.frameWidth, settings.frameHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postImage, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Setup blur buffers.
	glBindFramebuffer(GL_FRAMEBUFFER, blurBuffer[0]);

	glBindTexture(GL_TEXTURE_2D, blurImage[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 256, 144, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurImage[0], 0);

	glBindFramebuffer(GL_FRAMEBUFFER, blurBuffer[1]);

	glBindTexture(GL_TEXTURE_2D, blurImage[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 256, 144, 0, GL_RGB, GL_FLOAT, NULL);
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

	drawQueue = (DrawRequest*)malloc(settings.rendererDrawQueueSize * sizeof(DrawRequest));

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

//Calculate a correct perspective projection for the window.
Mat4 Renderer::getWindowProjection(float hFov){
	int width, height;
	SDL_GetWindowSize(window, &width, &height);

	float aspect = (float)width/(float)height;
	float vFov = 2 * atan(tan(hFov*0.5)*aspect);

	return Mat4::perspective(hFov, aspect, 0.1, 100.0);
}

//Bind g buffer.
void Renderer::bindGBuffer(){
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glViewport(0, 0, settings.frameWidth, settings.frameHeight);
}

//Bind display buffer.
void Renderer::bindDisplay(){
	glBindFramebuffer(GL_FRAMEBUFFER, displayBuffer);
	glViewport(0, 0, settings.frameWidth, settings.frameHeight);
}

//Deferred lighting pass.
void Renderer::deferredPass(){
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
	uniforms.lights.numPointlights = 0.0;
	uniforms.lights.numSpotlights = 0.0;
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

//Update common uniforms.
void Renderer::updateCommons(){
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CommonUniforms), &uniforms.common);
}

//Update common uniforms and calculate shadow projections.
void Renderer::updateLights(){
	float scale = 8.0;
	for(int i=0;i<NUM_SUN_CASCADES;i++){
		uniforms.lights.sun.projViewCSM[i] = Mat4::lookAt(
		Vec3::normalize(uniforms.lights.sun.direction) * 200 + (uniforms.common.camPosition),
		(uniforms.common.camPosition), Vec3(0,0,1)) * 
		Mat4::orthographic(-scale, scale, -scale, scale, 0.1, 400.0);
		scale *= 2.0;
	}

	for(int i=0;i<uniforms.lights.numSpotlights;i++){
		uniforms.lights.spotlights[i].projViewCSM = Mat4::lookAt(
			uniforms.lights.spotlights[i].position,
			Vec3::normalize(uniforms.lights.spotlights[i].direction) + uniforms.lights.spotlights[i].position,
			Vec3(0,0,1)
		) * Mat4::perspective(acos(uniforms.lights.spotlights[i].cutOff) * 2.0, 1, 0.01, 100.0);
	}

	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(CommonUniforms), sizeof(LightUniforms), &uniforms.lights);
}

//Add a pointlight.
void Renderer::pushLight(Pointlight light){
	if(uniforms.lights.numPointlights < MAX_POINTLIGHTS){
		uniforms.lights.pointlights[(Uint32)uniforms.lights.numPointlights] = light;
		uniforms.lights.numPointlights++;
	}else{
		std::cout<<"WARNING: Pointlight capacity full. Cannot add more."<<std::endl;
	}
}

//Add a spotlight.
void Renderer::pushLight(Spotlight light){
	if(uniforms.lights.numSpotlights < MAX_SPOTLIGHTS){
		uniforms.lights.spotlights[(Uint32)uniforms.lights.numSpotlights] = light;
		uniforms.lights.numSpotlights++;
	}else{
		std::cout<<"WARNING: Spotlight capacity full. Cannot add more."<<std::endl;
	}
}

//Bind shadow map framebuffer.
void Renderer::bindShadowFrame(){
	glViewport(0, 0, settings.shadowWidth, settings.shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
}

//Bind shadow map layer for drawing.
void Renderer::bindShadowLayer(Uint32 layer){
	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowImages, 0, layer);
}

//Clear shadow maps.
void Renderer::clearShadows(){
	for(int i=0;i<NUM_SUN_CASCADES + uniforms.lights.numSpotlights;i++){
		bindShadowLayer(i);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
}

/*
//Draw a static model.
void Renderer::drawModel(StaticModel* mesh, Mat4 model){
	glBindVertexArray(mesh->vao);

	//Draw to g buffer.
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	mesh->gProgram.use();
	glUniformMatrix4fv(0, 1, false, model.ptr());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, mesh->diffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, mesh->metalRough);

	glDrawArrays(GL_TRIANGLES, 0, mesh->numVertices);

	//Draw to shadow map.
	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
}
*/
