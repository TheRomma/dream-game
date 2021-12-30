#include "graphics.hpp"

//Find and replace (the FIRST OCCURENCE of) a word in a cpp string.
void findAndReplace(std::string& str, std::string before, std::string after){
	str.replace(str.find(before), before.length(), after);
}

//Shader init with vertex & fragment.
void Shader::init(const char* vertexSource, const char* fragmentSource){
    int success;
    char infoLog[512];

    Uint32 vertex = glCreateShader(GL_VERTEX_SHADER); 
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout<<"ERROR: Vertex shader compilation failed!\n"<<infoLog<<std::endl;
    }

    Uint32 fragment = glCreateShader(GL_FRAGMENT_SHADER); 
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout<<"ERROR: Fragment shader compilation failed!\n"<<infoLog<<std::endl;
    }

    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout<<"ERROR: Shader program linking failed!\n"<<infoLog<<std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

//Shader init with vertex, geometry & fragment.
void Shader::init(const char* vertexSource, const char* geometrySource, const char* fragmentSource){
    int success;
    char infoLog[512];

    Uint32 vertex = glCreateShader(GL_VERTEX_SHADER); 
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout<<"ERROR: Vertex shader compilation failed!\n"<<infoLog<<std::endl;
    }

    Uint32 geometry = glCreateShader(GL_GEOMETRY_SHADER); 
    glShaderSource(geometry, 1, &geometrySource, NULL);
    glCompileShader(geometry);
    glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(geometry, 512, NULL, infoLog);
        std::cout<<"ERROR: Geometry shader compilation failed!\n"<<infoLog<<std::endl;
    }

    Uint32 fragment = glCreateShader(GL_FRAGMENT_SHADER); 
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout<<"ERROR: Fragment shader compilation failed!\n"<<infoLog<<std::endl;
    }

    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, geometry);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout<<"ERROR: Shader program linking failed!\n"<<infoLog<<std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(geometry);
    glDeleteShader(fragment);
}

//Shader program destructor.
Shader::~Shader(){
	if(program){
		glDeleteProgram(program);
	}
}

//Bind shader program.
void Shader::use(){
	glUseProgram(program);
}

//---------------------------------------------------------------------------------------------------------

//Vertex buffer init.
void VertexBuffer::init(Uint32 stride, Uint32 length, char* data){
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, length, data, GL_STATIC_DRAW);

	this->stride = stride;
	attribLength = 0;
	numVertices = length / stride;
}

//Vertex buffer destructor.
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

//---------------------------------------------------------------------------------------------------------

//2D texture init.
void Texture::init(Uint32 width, Uint32 height, Uint32 filter, char* data){
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, false, GL_RGBA, GL_FLOAT, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);//Filter must be implemented
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}

//2D texture destructor.
Texture::~Texture(){
	if(texture){
		glDeleteTextures(1, &texture);
	}
}

//Bind 2D texture.
void Texture::bind(Uint32 slot){
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texture);
}

//--------------------------------------------------------------------------------------------------------

//Array texture init.
void ArrayTexture::init(Uint32 width, Uint32 height, Uint32 depth, Uint32 filter, char* data){
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA32F, width, height, depth, false, GL_RGBA, GL_FLOAT, data);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, filter);
}

//Array texture destructor.
ArrayTexture::~ArrayTexture(){
	if(texture){
		glDeleteTextures(1, &texture);
	}
}

//Bind array texture.
void ArrayTexture::bind(Uint32 slot){
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
}

//--------------------------------------------------------------------------------------------------------

//ShaderBuffer init.
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

//-------------------------------------------------------------------------

//CommonUniforms init.
void CommonUniforms::init(){
	ubo.init(GL_UNIFORM_BUFFER, UNIFORM_COMMON_BASE, sizeof(CommonBlock), nullptr);
}

//Bind ubo.
void CommonUniforms::bind(){
	ubo.bind();
}

//Write to ubo.
void CommonUniforms::write(){
	ubo.write(0, sizeof(CommonBlock), (char*)&block);
}

//-------------------------------------------------------------------------

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

//Calculate lights projView matrices.
void CSM::calcProjViews(float nearest, Vec3 target, Vec3 sunDirection){
	float scale = nearest;
	for(unsigned int i=0;i<numMaps;i++){
		projView[i] = Mat4::lookAt(
		Vec3::normalize(sunDirection) * 100 + (target),
		(target), Vec3(0,0,1)) * 
		Mat4::orthographic(-scale, scale, -scale, scale, 0.1, 200.0);
		scale *= 2;
	}
}

//-------------------------------------------------------------------------

//Lighting init.
void Lighting::init(Uint32 shadowWidth, Uint32 shadowHeight){
	ubo.init(GL_UNIFORM_BUFFER, UNIFORM_LIGHTING_BASE, sizeof(LightingBlock), nullptr);
	sunCSM.init(shadowWidth, shadowHeight, SUN_NUM_SHADOW_CASCADES);
	this->reset();
}

//Bind uniform buffer.
void Lighting::bind(){
	ubo.bind();
}

//Reset number of lights.
void Lighting::reset(){
	block.numPointlights = block.numStaticPointlights;
	block.numSpotlights = block.numStaticSpotlights;
}

//Reset number of static lights.
void Lighting::resetStatic(){
	block.numStaticPointlights = 0;
	block.numStaticSpotlights = 0;
}

//Add a pointlight.
void Lighting::push(Pointlight light){
	if(block.numPointlights < MAX_POINTLIGHTS){
		block.pointlights[(Uint32)block.numPointlights] = light;
		block.numPointlights++;
	}else{
		std::cout<<"WARNING: Pointlight capacity full. Cannot add more."<<std::endl;
	}
}

//Add a spotlight.
void Lighting::push(Spotlight light){
	if(block.numSpotlights < MAX_SPOTLIGHTS){
		block.spotlights[(Uint32)block.numSpotlights] = light;
		block.numSpotlights++;
	}else{
		std::cout<<"WARNING: Spotlight capacity full. Cannot add more."<<std::endl;
	}
}

//Add permanent point- and spotlights.
void Lighting::pushStatic(Pointlight* points, Uint32 numPoints, Spotlight* spots, Uint32 numSpots){
	memcpy(block.pointlights, points, numPoints * sizeof(Pointlight));
	memcpy(block.spotlights, spots, numSpots * sizeof(Spotlight));
	block.numStaticPointlights = numPoints;
	block.numStaticSpotlights = numSpots;
}

//Write to uniform buffer.
void Lighting::write(){
	ubo.write(0, sizeof(LightingBlock), (char*)&block);
	//this->reset();
}

//------------------------------------------------------------------------

void DeferredTarget::init(Uint32 width, Uint32 height){
	this->width = width;
	this->height = height;

	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
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

	glGenFramebuffers(1, &finalBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, finalBuffer);

	glGenTextures(1, &finalImage);
	glBindTexture(GL_TEXTURE_2D, finalImage);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//TODO Configurable screen smoothing?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, finalImage, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	const char* vertex = R"(#version 430 core
	layout(location = 0) in vec2 POSITION;
	layout(location = 1) in vec2 UV_COORD;

	out VS_OUT{
		vec2 uv_coord;
	} F;

	void main(){
		gl_Position = vec4(POSITION, 0.0, 1.0);

		F.uv_coord = UV_COORD;
	}
	)";
	std::string fragment = R"(#version 430 core
	#define UNIFORM_COMMON_BASE [COMMON_BASE]
	#define UNIFORM_LIGHTING_BASE [LIGHTING_BASE]

	#define SUN_SHADOW_BASE [SUN_BASE]
	#define SUN_NUM_SHADOW_CASCADES [SUN_CASCADES]

	#define MAX_POINTLIGHTS [MAX_POINTS]
	#define MAX_SPOTLIGHTS [MAX_SPOTS]

	#define FOG_DISTANCE 80

	out vec4 outColor;

	in VS_OUT{
		vec2 uv_coord;
	}F;

	layout(std140, binding = UNIFORM_COMMON_BASE) uniform U{
		mat4 projView;
		vec4 posTime;
	};

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
		vec4 position;//vec3 position, float radius
		vec4 direction;//vec3 direction, float cutoff
		vec3 ambient;
		vec3 diffuse;
	};

	layout(std140, binding = UNIFORM_LIGHTING_BASE) uniform L{
		Sun sun;
		vec4 numLights;//r = numPointlights, b = numSpotlights
		Pointlight pointlights[MAX_POINTLIGHTS];
		Spotlight spotlights[MAX_SPOTLIGHTS];
	};

	layout(binding = 0) uniform sampler2D u_position;
	layout(binding = 1) uniform sampler2D u_normal;
	layout(binding = 2) uniform sampler2D u_albedo;
	layout(binding = SUN_SHADOW_BASE) uniform sampler2DArray u_sunShadow;

	vec3 calcSunlight(Sun light, vec3 position, vec3 normal, vec3 albedo);
	float calcSunShadow(Sun light, vec3 position, sampler2DArray shadowMap);

	vec3 calcPointlight(Pointlight light, vec3 position, vec3 normal, vec3 albedo);
	vec3 calcSpotlight(Spotlight light, vec3 position, vec3 normal, vec3 albedo);

	void main(){
		vec3 normal = texture(u_normal, F.uv_coord).rgb;

		if(normal != vec3(0.0, 0.0, 0.0)){
			vec3 position = texture(u_position, F.uv_coord).rgb;
			vec3 normal = texture(u_normal, F.uv_coord).rgb;
			vec3 albedo = texture(u_albedo, F.uv_coord).rgb;
			float distance = texture(u_normal, F.uv_coord).a;

			vec3 result = vec3(0.0);
			if(distance < FOG_DISTANCE){
				result += calcSunlight(sun, position, normal, albedo);

				for(unsigned int i=0;i<numLights.r;i++){
					result += calcPointlight(pointlights[i], position, normal, albedo);
				}
				for(unsigned int i=0;i<numLights.b;i++){
					result += calcSpotlight(spotlights[i], position, normal, albedo);
				}
				float fogRatio = distance / FOG_DISTANCE;
				result = result * (1 - fogRatio) + sun.ambient * fogRatio;
			}else{
				result = sun.ambient;
			}

			float gamma = 1.0;
			result = result / (result + vec3(1.0));
			result = pow(result, vec3(1.0 / gamma));

			outColor = vec4(result, 1.0);
		}else{
			outColor = vec4(sun.ambient, 1.0);
			//discard;
		}
	}

	vec3 calcSunlight(Sun light, vec3 position, vec3 normal, vec3 albedo){
		if(light.diffuse == vec3(0.0, 0.0, 0.0)){
			return light.ambient * albedo;
		}

		vec3 direction = normalize(light.direction);

		float diffRatio = max(dot(normal, direction), 0.0);

		vec3 ambient = light.ambient * albedo;
		if(diffRatio <= 0){
			return ambient;
		}

		vec3 diffuse = light.diffuse * diffRatio * albedo;

		float shadow = calcSunShadow(sun, position, u_sunShadow);

		return ambient + (1.0 - shadow) * diffuse;
	}

	float calcSunShadow(Sun light, vec3 position, sampler2DArray shadowMap){
		int mapIndex = SUN_NUM_SHADOW_CASCADES - 1;
		vec3 pos = posTime.rgb;
		float distance = length(position - pos);

		if(distance < 8){mapIndex = 0;}
		else if(distance < 16){mapIndex = 1;}
		else if(distance < 32){mapIndex = 2;}

		vec4 lightSpaceFrag = light.view[mapIndex] * vec4(position.rgb, 1.0);
		vec3 projPos = lightSpaceFrag.xyz / lightSpaceFrag.w;
		if(projPos.z > 1.0){return 0.0;}
		projPos = projPos * 0.5 + 0.5;
		float currentDepth = projPos.z;
		
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

	vec3 calcPointlight(Pointlight light, vec3 position, vec3 normal, vec3 albedo){
		vec3 ab = light.position.rgb - position;
		vec3 direction = normalize(ab);
		float distSquare = dot(ab, ab);

		float diffRatio = max(dot(normal, direction), 0.0);
		float attenuation = clamp(1.0 - distSquare / (light.position.a*light.position.a), 0.0, 1.0);

		vec3 ambient = light.ambient * albedo * attenuation;
		vec3 diffuse = light.diffuse * diffRatio * albedo * attenuation;

		return ambient + diffuse;
	}

	vec3 calcSpotlight(Spotlight light, vec3 position, vec3 normal, vec3 albedo){
		vec3 ab = light.position.rgb - position;
		vec3 direction = normalize(ab);

		float distSquare = dot(ab, ab);

		float diffRatio = max(dot(normal, direction), 0.0);
		float attenuation = clamp(1.0 - distSquare / (light.position.a*light.position.a), 0.0, 1.0);

		vec3 ambient = light.ambient * albedo * attenuation;
		
		float theta = dot(direction, normalize(-light.direction.rgb));
		if(theta > cos(light.direction.a)){
			vec3 diffuse = light.diffuse * diffRatio * albedo * attenuation;
			return ambient + diffuse;
		}else{
			return ambient;
		}
	}
	)";
	findAndReplace(fragment, "[COMMON_BASE]", std::to_string(UNIFORM_COMMON_BASE));
	findAndReplace(fragment, "[LIGHTING_BASE]", std::to_string(UNIFORM_LIGHTING_BASE));
	findAndReplace(fragment, "[SUN_BASE]", std::to_string(SUN_SHADOW_BASE));
	findAndReplace(fragment, "[SUN_CASCADES]", std::to_string(SUN_NUM_SHADOW_CASCADES));
	findAndReplace(fragment, "[MAX_POINTS]", std::to_string(MAX_POINTLIGHTS));
	findAndReplace(fragment, "[MAX_SPOTS]", std::to_string(MAX_SPOTLIGHTS));
	deferredProgram.init(vertex, fragment.c_str());

	const char* displayVertex = R"(#version 430 core
	layout(location = 0) in vec2 POSITION;
	layout(location = 1) in vec2 UV_COORD;

	out VS_OUT{
		vec2 uv_coord;
	} F;

	void main(){
		gl_Position = vec4(POSITION, 0.0, 1.0);

		F.uv_coord = UV_COORD;
	}
	)";
	const char* displayFragment = R"(#version 430 core
	out vec4 outColor;

	in VS_OUT{
		vec2 uv_coord;
	}F;

	layout(binding = 0) uniform sampler2D u_texture;

	void main(){
		outColor = texture(u_texture, F.uv_coord);
	}
	)";
	displayProgram.init(displayVertex, displayFragment);

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
DeferredTarget::~DeferredTarget(){
	if(gBuffer){
		glDeleteTextures(1, &gPosition);
		glDeleteTextures(1, &gNormal);
		glDeleteTextures(1, &gAlbedo);
		glDeleteTextures(1, &depthStencil);
		glDeleteTextures(1, &finalImage);
		glDeleteFramebuffers(1, &gBuffer);
		glDeleteFramebuffers(1, &finalBuffer);
	}
}

//Bind framebuffer.
void DeferredTarget::bind(){
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glViewport(0, 0, width, height);
}

//Bind framebuffer.
void DeferredTarget::bindFinal(){
	glBindFramebuffer(GL_FRAMEBUFFER, finalBuffer);
	glViewport(0, 0, width, height);
}

//Deferred rendering pass.
void DeferredTarget::draw(){
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, finalBuffer);

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

//Display the drawn image on screen.
void DeferredTarget::display(Uint32 displayWidth, Uint32 displayHeight){
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, displayWidth, displayHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	displayProgram.use();
	buffer.bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, finalImage);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

