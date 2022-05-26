#include "renderer.hpp"

//Renderer constructor.
int Renderer::init(RendererSettings settings){
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
	frameWidth = settings.frameWidth;
	frameHeight = settings.frameHeight;
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
		(glsl_header() + glsl_commonUniforms() + glsl_commonLightStructs() +
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
	float border[4] = {1.0,1.0,1.0,1.0};
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, border);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowImages, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return 0;
}

//Renderer destructor.
Renderer::~Renderer(){
	glDeleteTextures(1, &shadowImages);
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

//Swap window buffers.
void Renderer::swapBuffers(){
	SDL_GL_SwapWindow(window);
}

//Bind gBuffer.
void Renderer::bindGBuffer(){
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
}

//Deferred lighting pass.
void Renderer::deferredPass(){
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, frameWidth, frameHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, displayBuffer);

	deferredProgram.use();
	glBindVertexArray(nullVao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

//Apply bloom to final image.
void Renderer::applyBloom(Uint32 blurPasses){
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, 256, 144);
	glBindVertexArray(nullVao);
	
	moveProgram.use();

	glBindFramebuffer(GL_FRAMEBUFFER, blurBuffer[0]);	

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, displayImage);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Heavily blur resulting image.
	blurProgram.use();
	
	int horizontal = 1;
	for(int i=0;i<blurPasses*2;i++){
		glBindFramebuffer(GL_FRAMEBUFFER, blurBuffer[horizontal]);
		glUniform1i(1, horizontal);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, blurImage[1-horizontal]);

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

	glViewport(0, 0, frameWidth, frameHeight);
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

//Convolve a 3x3 kernel with the final image.
void Renderer::applyKernel(float* kernel){
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, frameWidth, frameHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, postBuffer);
	
	kernelProgram.use();
	glBindVertexArray(nullVao);

	glUniformMatrix3fv(1, 1, 0, kernel);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, displayImage);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, displayBuffer);

	moveProgram.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postImage);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

//Display the final image on screen.
void Renderer::display(){
	int width, height;
	SDL_GetWindowSize(window, &width, &height);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	displayProgram.use();
	glBindVertexArray(nullVao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, displayImage);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	SDL_GL_SwapWindow(window);
}

//Write changes to common uniforms.
void Renderer::updateUniforms(){
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UniformBlock), &uniforms);
}
