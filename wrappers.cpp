#include "wrappers.hpp"
//Rendertarget wrapper init.
void Rendertarget::init(Uint32 width, Uint32 height){
	this->width = width;
	this->height = height;

	glGenFramebuffers(1, &frame);
	glBindFramebuffer(GL_FRAMEBUFFER, frame);

	glGenTextures(1, &color);
	glBindTexture(GL_TEXTURE_2D, color);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, false, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);

	glGenTextures(1, &depthStencil);
	glBindTexture(GL_TEXTURE_2D, depthStencil);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, false, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencil, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	const char* vertex = R"(#version 430 core
	layout(location = 0) in vec2 POSITION;
	layout(location = 1) in vec2 UV_COORD;

	layout(std140, binding = 0) uniform U{
		mat4 projView;
		float time;
	};

	out VS_OUT{
		vec2 uv_coord;
	} F;

	void main(){
		gl_Position = vec4(POSITION, 0.0, 1.0);

		F.uv_coord = UV_COORD;
	}
	)";
	const char* fragment = R"(#version 430 core
	out vec4 outColor;

	in VS_OUT{
		vec2 uv_coord;
	}F;

	layout(binding = 0) uniform sampler2D u_texture;
	layout(binding = 1) uniform sampler2D u_depthStencil;

	void main(){
		vec4 color = texture(u_texture, F.uv_coord);
		//vec4 depth = texture(u_depthStencil, F.uv_coord);
		outColor = color;
	}
	)";
	program.init(vertex, fragment);

	float vertices[24] = {
		-1.0, -1.0, 0.0, 0.0,
		 1.0, -1.0, 1.0, 0.0,
		 1.0,  1.0, 1.0, 1.0,

		 1.0,  1.0, 1.0, 1.0,
		-1.0,  1.0, 0.0, 1.0,
		-1.0, -1.0, 0.0, 0.0
	};
	buffer.init(4 * sizeof(float), 24 * sizeof(float), (char*)vertices);
	buffer.setAttribute(0, 2);
	buffer.setAttribute(1, 2);
}

//Rendertarget destructor.
Rendertarget::~Rendertarget(){
	if(frame){
		glDeleteFramebuffers(1, &frame);
		glDeleteTextures(1, &color);
		glDeleteTextures(1, &depthStencil);
	}
}

//Bind Rendertarget.
void Rendertarget::bind(){
	glBindFramebuffer(GL_FRAMEBUFFER, frame);
	glViewport(0, 0, width, height);
}

//Draw contents onto the screen.
void Rendertarget::draw(){
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	program.use();
	buffer.bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthStencil);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}


//--------------------------------------------------------------------------------------------------------

//SSBO wrapper init.
void SSBO::init(Uint32 base, Uint32 length, char* data){
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);

	glBufferData(GL_SHADER_STORAGE_BUFFER, length, data, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, base, buffer);
}

//SSBO destructor.
SSBO::~SSBO(){
	if(buffer){
		glDeleteBuffers(1, &buffer);
	}
}

//Bind SSBO.
void SSBO::bind(){
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
}

//Write to SSBO.
void SSBO::write(Uint32 offset, Uint32 length, char* data){
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, length, data);
}

//Rebind SSBO base.
void SSBO::bindBase(Uint32 base){
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, base, buffer);
}

//--------------------------------------------------------------------------------------------------------

//UBO wrapper init.
void UniformBuffer::init(Uint32 base){
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);

	glBufferData(GL_UNIFORM_BUFFER, sizeof(block), nullptr, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, base, buffer);
}

//UBO destructor.
UniformBuffer::~UniformBuffer(){
	if(buffer){
		glDeleteBuffers(1, &buffer);
	}
}

//Bind UBO.
void UniformBuffer::bind(){
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
}

//Write to UBO.
void UniformBuffer::write(){
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(block), &block);
}

//Rebind UBO base.
void UniformBuffer::bindBase(Uint32 base){
	glBindBufferBase(GL_UNIFORM_BUFFER, base, buffer);
}

//--------------------------------------------------------------------------------------------------------

//Point light data construcotr.
PointLightData::PointLightData(Vec3 ambient, Vec3 diffuse, Vec3 position, float radius){
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->position = position;
	this->radius = radius;
}

//Light storage init.
void LightStorage::init(Uint32 base, Uint32 mapWidth, Uint32 mapHeight){
	lights.numPoints = 0;
	staticPoints = 0;
	storage.init(base, sizeof(LightData), nullptr);
	shadowMap.init(mapWidth, mapHeight, 3);
}

//Push permanent static lights.
void LightStorage::staticPush(PointLightData* data, Uint32 numLights){
	if(numLights <= MAX_POINT_LIGHTS){
		memcpy(lights.points, data, numLights * sizeof(PointLightData));
		staticPoints = numLights;
	}
}

//Reset dynamic light counter after writing.
void LightStorage::reset(){
	lights.numPoints = staticPoints;
}

//Push new light into storage.
void LightStorage::push(PointLightData data){
	if(lights.numPoints < MAX_POINT_LIGHTS){
		lights.points[(Uint32)lights.numPoints] = data;
		lights.numPoints++;
	}
}

//Bind light storage.
void LightStorage::bind(){
	storage.bind();
}

//Write into storage.
void LightStorage::write(){
	storage.write(0, sizeof(LightData), (char*)&lights);
	this->reset();
}

//--------------------------------------------------------------------------------------------------------

//Lighting pass render target init.
void DTarget::init(Uint32 width, Uint32 height){
	this->width = width;
	this->height = height;

	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

	Uint32 attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, attachments);

	glGenTextures(1, &depthStencil);
	glBindTexture(GL_TEXTURE_2D, depthStencil);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, false, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencil, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	const char* vertex = R"(#version 430 core
	layout(location = 0) in vec2 POSITION;
	layout(location = 1) in vec2 UV_COORD;

	layout(std140, binding = 0) uniform U{
		mat4 projView;
		float time;
	};

	out VS_OUT{
		vec2 uv_coord;
	} F;

	void main(){
		gl_Position = vec4(POSITION, 0.0, 1.0);

		F.uv_coord = UV_COORD;
	}
	)";
	const char* fragment = R"(#version 430 core
	out vec4 outColor;

	in VS_OUT{
		vec2 uv_coord;
	}F;

	layout(std140, binding = 0) uniform U{
		mat4 projView;
		float time;
	};

	layout(binding = 0) uniform sampler2D u_position;
	layout(binding = 1) uniform sampler2D u_normal;
	layout(binding = 2) uniform sampler2D u_albedo;
	layout(binding = 3) uniform sampler2DArray u_shadow;

	struct SunLight{
		vec3 direction;
		vec3 ambient;
		vec3 diffuse;
		mat4 view[3];
	};

	struct PointLight{
		vec3 ambient;
		vec3 diffuse;

		vec4 position;
	};

	layout(std430, binding = 4) buffer LightData{
		SunLight sun;
		vec4 numLights;
		PointLight points[64];
	};

	vec3 calcSunLight(SunLight light, vec4 position, vec3 normal, vec3 albedo, sampler2DArray shadowMap);
	vec3 calcPointLight(PointLight light, vec4 position, vec3 normal, vec3 albedo);
	float calcShadow(SunLight light, vec4 position, sampler2DArray shadowMap);

	void main(){
		vec3 normal = texture(u_normal, F.uv_coord).rgb;

		if(normal != vec3(0.0, 0.0, 0.0)){
			vec4 position = texture(u_position, F.uv_coord).rgba;
			vec3 albedo = texture(u_albedo, F.uv_coord).rgb;

			vec3 result  = vec3(0.0);

			result += calcSunLight(sun, position, normal, albedo, u_shadow);

			for(int i=0;i<numLights.r;i++){
				result += calcPointLight(points[i], position, normal, albedo);
			}

			outColor = vec4(result, 1.0);
		}else{
			outColor = vec4(0.1, 0.0, 0.1, 1.0);
		}

	}

	vec3 calcSunLight(SunLight light, vec4 position, vec3 normal, vec3 albedo, sampler2DArray shadowMap){
		if(light.diffuse == vec3(0.0, 0.0, 0.0)){
			return light.ambient * albedo;
		}

		vec3 direction = normalize(light.direction);

		float diffRatio = max(dot(normal, direction), 0.0);

		vec3 ambient = light.ambient * albedo;
		vec3 diffuse = light.diffuse * diffRatio * albedo;
		float shadow = calcShadow(sun, position, shadowMap);

		return ambient + (1.0 - shadow) * diffuse;
	}

	vec3 calcPointLight(PointLight light, vec4 position, vec3 normal, vec3 albedo){
		vec3 ab = light.position.rgb - position.rgb;
		vec3 direction = normalize(ab);
		float distSquare = dot(ab, ab);

		float diffRatio = max(dot(normal, direction), 0.0);
		float attenuation = clamp(1.0 - distSquare / (light.position.a*light.position.a), 0.0, 1.0);

		vec3 ambient = light.ambient * albedo * attenuation;
		vec3 diffuse = light.diffuse * diffRatio * albedo * attenuation;

		return ambient + diffuse;
	}

	float calcShadow(SunLight light, vec4 position, sampler2DArray shadowMap){
		vec2 coord = F.uv_coord.xy;
		coord = coord - vec2(0.5,0.5);

		int mapIndex = 0;
		if(position.a + abs(coord.x) * 8 + abs(coord.y) * 5 < 8){mapIndex = 0;}
		else if(position.a + abs(coord.x) * 16 + abs(coord.y) * 5 < 16){mapIndex = 1;}
		else{mapIndex = 2;}

		vec4 lightSpaceFrag = light.view[mapIndex] * vec4(position.rgb, 1.0);
		vec3 projPos = lightSpaceFrag.xyz / lightSpaceFrag.w;
		projPos = projPos * 0.5 + 0.5;
		float currentDepth = projPos.z;
		
		/*
		float closestDepth = texture(shadowMap, vec3(projPos.xy, float(mapIndex))).r;
		float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
		*/

		float shadow = 0.0;
		float pcfDepth = 0.0;
		vec2 texelSize = 1.0 / textureSize(shadowMap, 0).xy;

		for(int i=-1;i<=1;i++){
			for(int j=-1;j<=1;j++){
				pcfDepth = texture(shadowMap, vec3((projPos.xy + vec2(i,j) * texelSize), float(mapIndex))).r;
				shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
			}
		}
		
		return shadow / 9;
	}
	)";
	program.init(vertex, fragment);

	float vertices[24] = {
		-1.0, -1.0, 0.0, 0.0,
		 1.0, -1.0, 1.0, 0.0,
		 1.0,  1.0, 1.0, 1.0,

		 1.0,  1.0, 1.0, 1.0,
		-1.0,  1.0, 0.0, 1.0,
		-1.0, -1.0, 0.0, 0.0
	};
	buffer.init(4 * sizeof(float), 24 * sizeof(float), (char*)vertices);
	buffer.setAttribute(0, 2);
	buffer.setAttribute(1, 2);
}

//Deferred target destructor.
DTarget::~DTarget(){
	if(gBuffer){
		glDeleteTextures(1, &gPosition);
		glDeleteTextures(1, &gNormal);
		glDeleteTextures(1, &gAlbedo);
		glDeleteFramebuffers(1, &gBuffer);
	}
}

//Bind target.
void DTarget::bind(){
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glViewport(0, 0, width, height);
}

//Deferred rendering pass.
void DTarget::draw(){
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	program.use();
	buffer.bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

//--------------------------------------------------------------------------------------------------------

//Shadow map init.
void ShadowMap::init(Uint32 width, Uint32 height, Uint32 numMaps){
	this->width = width;
	this->height = height;
	this->numMaps = numMaps;

	glGenFramebuffers(1, &frame);
	glBindFramebuffer(GL_FRAMEBUFFER, frame);

	glGenTextures(1, &depth);
	glBindTexture(GL_TEXTURE_2D_ARRAY, depth);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT, width, height, numMaps, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float border[4] = {1.0,1.0,1.0,1.0};
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, border);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//Destructor.
ShadowMap::~ShadowMap(){
	if(frame){
		glDeleteTextures(1, &depth);
		glDeleteFramebuffers(1, &frame);
	}
}

//Bind frame.
void ShadowMap::bind(){
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, frame);
}

//Bind shadow map.
void ShadowMap::bindShadow(Uint32 index){
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D_ARRAY, depth);
}

//--------------------------------------------------------------------------------------------------------
