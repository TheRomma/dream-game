#include "shader.hpp"

#include <iostream>

//Shader constructor with vertex and fragment sources.
void Shader::init(
		const char* vertexSources,
		const char* fragmentSources
){
    int success;
    char infoLog[512];

	//Create vertex shader.
    Uint32 vertex = glCreateShader(GL_VERTEX_SHADER); 
    glShaderSource(vertex, 1, &vertexSources, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout<<"ERROR: Vertex shader compilation failed!\n"<<infoLog<<std::endl;
    }

	//Create fragment shader.
    Uint32 fragment = glCreateShader(GL_FRAGMENT_SHADER); 
    glShaderSource(fragment, 1, &fragmentSources, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout<<"ERROR: Fragment shader compilation failed!\n"<<infoLog<<std::endl;
    }

	//Link and compile shader program.
    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout<<"ERROR: Shader program linking failed!\n"<<infoLog<<std::endl;
    }

	//Discard no longer needed shaders.
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

//Shader constructor with vertex, geometry and fragment sources.
void Shader::init(
		const char* vertexSources,
		const char* geometrySources,
		const char* fragmentSources
){
    int success;
    char infoLog[512];

	//Create vertex shader.
    Uint32 vertex = glCreateShader(GL_VERTEX_SHADER); 
    glShaderSource(vertex, 1, &vertexSources, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout<<"ERROR: Vertex shader compilation failed!\n"<<infoLog<<std::endl;
    }

	//Create geometry shader.
    Uint32 geometry = glCreateShader(GL_GEOMETRY_SHADER); 
    glShaderSource(geometry, 1, &geometrySources, NULL);
    glCompileShader(geometry);
    glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(geometry, 512, NULL, infoLog);
        std::cout<<"ERROR: Geometry shader compilation failed!\n"<<infoLog<<std::endl;
    }

	//Create fragment shader.
    Uint32 fragment = glCreateShader(GL_FRAGMENT_SHADER); 
    glShaderSource(fragment, 1, &fragmentSources, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout<<"ERROR: Fragment shader compilation failed!\n"<<infoLog<<std::endl;
    }

	//Link and compile shader program.
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

	//Discard no longer needed shaders.
    glDeleteShader(vertex);
    glDeleteShader(geometry);
    glDeleteShader(fragment);
}

//Shader destructor.
Shader::~Shader(){
	glDeleteProgram(program);
}

//Use shader program.
void Shader::use(){
	glUseProgram(program);
}

//-------------------------------------------------------------------------------------------

//Shader header in glsl.
std::string glsl_header(){
	std::string str = R"(#version 430 core
	)";	
	return str;
}

//-------------------------------------------------------------------------------------------

//A shader that does nothing.
std::string glsl_emptyShader(){
	std::string str = R"(
	void main(){
		//Empty
	}
	)";	
	return str;
}

//-------------------------------------------------------------------------------------------

//Common uniforms in a ubo in glsl.
std::string glsl_commonUniforms(){
	std::string str = R"(
		layout(std140, binding = UBO_BINDING) uniform U{
			mat4 projView;
			vec4 posTime;
		};
	)";
	str.replace(
		str.find("UBO_BINDING"),
		std::string("UBO_BINDING").length(),
		std::to_string(UBO_COMMON_BASE)
	);
	return str;
}

//------------------------------------------------------------------------------------------

//Common light structs in glsl.
std::string glsl_commonLightStructs(){
	std::string str = R"(
		struct Sun{
			vec3 direction;
			vec3 ambient;
			vec3 diffuse;
			mat4 view[SUN_NUM_SHADOW_CASCADES];
		};

		struct Pointlight{
			vec4 position;//vec3 position, float radius
			vec3 ambient;
			vec3 diffuse;
		};

		struct Spotlight{
			vec4 position;//vec3 position, float radius
			vec4 direction;//vec3 direction, float cutoff
			vec3 ambient;
			vec3 diffuse;
		};

		layout(std140, binding = LIGHT_UBO_BINDING) uniform L{
			Sun sun;
			vec4 numLights;//r = numPointlights, b = numSpotlights
			Pointlight pointlights[MAX_POINTLIGHTS];
			Spotlight spotlights[MAX_SPOTLIGHTS];
		};
	)";
	str.replace(
		str.find("SUN_NUM_SHADOW_CASCADES"),
		std::string("SUN_NUM_SHADOW_CASCADES").length(),
		std::to_string(SUN_NUM_SHADOW_CASCADES)
	);
	str.replace(
		str.find("LIGHT_UBO_BINDING"),
		std::string("LIGHT_UBO_BINDING").length(),
		std::to_string(UBO_LIGHT_BASE)
	);
	str.replace(
		str.find("MAX_POINTLIGHTS"),
		std::string("MAX_POINTLIGHTS").length(),
		std::to_string(MAX_POINTLIGHTS)
	);
	str.replace(
		str.find("MAX_SPOTLIGHTS"),
		std::string("MAX_SPOTLIGHTS").length(),
		std::to_string(MAX_SPOTLIGHTS)
	);
	return str;
}

//------------------------------------------------------------------------------------------------------

//Vertex shader program for static models.
std::string glsl_deferredStaticModelVertex(){
	std::string str = R"(
		layout(location = 0) in vec3 POSITION;
		layout(location = 1) in vec3 UV_COORD;
		layout(location = 2) in vec3 NORMAL;

		out VS_OUT{
			vec4 position;
			vec3 uv_coord;
			vec3 normal;
		} F;

		layout(location = 0) uniform mat4 u_model;

		void main(){
			vec4 tempPos = u_model * vec4(POSITION, 1.0);
			vec4 result = projView * tempPos;
			gl_Position = result;

			F.position = vec4(tempPos.rgb, result.z);
			F.uv_coord = UV_COORD;
			F.normal = normalize(mat3(transpose(inverse(u_model))) * NORMAL.xyz);
		}
	)";	
	return str;
}

//Fragment shader program for static models.
std::string glsl_deferredAllModelFragment(){
	std::string str = R"(
		layout(location = 0) out vec4 gPosition;
		layout(location = 1) out vec4 gNormal;
		layout(location = 2) out vec4 gAlbedo;

		in VS_OUT{
			vec4 position;
			vec3 uv_coord;
			vec3 normal;
		}F;

		layout(binding = 0) uniform sampler2DArray u_diffuse;
		layout(binding = 1) uniform sampler2DArray u_metalRough;

		void main(){
			gPosition = F.position;
			vec2 metalRough = texture(u_metalRough, F.uv_coord).rg;
			gNormal = vec4(normalize(F.normal), metalRough.g);
			gAlbedo = vec4(texture(u_diffuse, F.uv_coord).rgb, metalRough.r);
		}
	)";	
	return str;
}

std::string glsl_staticModelShadowVertex(){
	std::string str = R"(
	layout(location = 0) in vec3 POSITION;

	layout(location = 0) uniform mat4 u_model;

	void main(){
		gl_Position = u_model * vec4(POSITION, 1.0);
	}
	)";	
	return str;
}

std::string glsl_allModelShadowGeometry(){
	std::string str = R"(
	#define NUM_CASCADES NUM_SHADOWS

	layout(triangles, invocations = NUM_CASCADES) in;
	layout(triangle_strip, max_vertices = 3) out;
	
	void main(){
		for(unsigned int i=0;i<NUM_CASCADES;i++){
			gl_Position = 
				sun.view[gl_InvocationID] * gl_in[i].gl_Position;
			gl_Layer = gl_InvocationID;
			EmitVertex();
		}
		EndPrimitive();
	}
	)";	
	str.replace(
		str.find("NUM_SHADOWS"),
		std::string("NUM_SHADOWS").length(),
		std::to_string(SUN_NUM_SHADOW_CASCADES)
	);
	return str;
}

//------------------------------------------------------------------------------------------------------

//Vertex shader program for animated models.
std::string glsl_deferredAnimatedModelVertex(Uint32 numBones){
	std::string str = R"(
		layout(location = 0) in vec3 POSITION;
		layout(location = 1) in vec3 UV_COORD;
		layout(location = 2) in vec3 NORMAL;
		layout(location = 3) in vec4 BONES;
		layout(location = 4) in vec4 WEIGHTS;

		out VS_OUT{
			vec4 position;
			vec3 uv_coord;
			vec3 normal;
		} F;

		layout(location = 0) uniform mat4 u_model;
		layout(location = 1) uniform mat4 u_joints[NUM_BONES];

		void main(){
			vec4 transform = u_model * (
				u_joints[int(BONES.r)] * vec4(POSITION, 1.0) * WEIGHTS.r +
				u_joints[int(BONES.g)] * vec4(POSITION, 1.0) * WEIGHTS.g +
				u_joints[int(BONES.b)] * vec4(POSITION, 1.0) * WEIGHTS.b +
				u_joints[int(BONES.a)] * vec4(POSITION, 1.0) * WEIGHTS.a
			);
			vec4 result = projView * transform;
			gl_Position = result;

			F.position = vec4(transform.rgb, result.z);
			F.uv_coord = UV_COORD;
			F.normal = normalize(mat3(transpose(inverse(u_model))) * (
				mat3(transpose(inverse(u_joints[int(BONES.r)]))) * NORMAL.xyz * WEIGHTS.r +
				mat3(transpose(inverse(u_joints[int(BONES.g)]))) * NORMAL.xyz * WEIGHTS.g +
				mat3(transpose(inverse(u_joints[int(BONES.b)]))) * NORMAL.xyz * WEIGHTS.b +
				mat3(transpose(inverse(u_joints[int(BONES.a)]))) * NORMAL.xyz * WEIGHTS.a
			));
		}
	)";	
	str.replace(
		str.find("NUM_BONES"),
		std::string("NUM_BONES").length(),
		std::to_string(numBones)
	);
	return str;
}

std::string glsl_animatedModelShadowVertex(Uint32 numBones){
	std::string str = R"(
	layout(location = 0) in vec3 POSITION;
	layout(location = 1) in vec3 UV_COORD;
	layout(location = 2) in vec3 NORMAL;
	layout(location = 3) in vec4 BONES;
	layout(location = 4) in vec4 WEIGHTS;

	layout(location = 0) uniform mat4 u_model;
	layout(location = 1) uniform mat4 u_joints[NUM_BONES];

	void main(){
		gl_Position = u_model * (
			u_joints[int(BONES.r)] * vec4(POSITION, 1.0) * WEIGHTS.r +
			u_joints[int(BONES.g)] * vec4(POSITION, 1.0) * WEIGHTS.g +
			u_joints[int(BONES.b)] * vec4(POSITION, 1.0) * WEIGHTS.b +
			u_joints[int(BONES.a)] * vec4(POSITION, 1.0) * WEIGHTS.a
		);
	}
	)";	
	str.replace(
		str.find("NUM_BONES"),
		std::string("NUM_BONES").length(),
		std::to_string(numBones)
	);
	return str;
}

//------------------------------------------------------------------------------------------------------

//Simple vertex shader for drawing a screen sized quad.
std::string glsl_displayQuadVertex(){
	std::string str = R"(
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
	return str;
}

//Simple fragment shader for drawing a screen sized quad.
std::string glsl_displayQuadFragment(){
	std::string str = R"(
		out vec4 outColor;

		in VS_OUT{
			vec2 uv_coord;
		}F;

		layout(binding = 0) uniform sampler2D u_image;

		void main(){
			outColor = texture(u_image, F.uv_coord);
			//outColor = texture(u_image, vec2((F.uv_coord.x + cos(posTime.a + F.uv_coord.y * 10) * 0.02) * 0.96 + 0.02, F.uv_coord.y));
		}
	)";	
	return str;
}

//------------------------------------------------------------------------------------------------

//Contains various light calculation functions in glsl. Requires commonLightStructs.
std::string glsl_lightCalculations(){
	std::string str = R"(
		#define PI 3.14

		float distributionGGX(vec3 N, vec3 H, float roughness){
			float a = roughness * roughness;
			float a2 = a * a;
			float NdotH = max(dot(N, H), 0.0);
			float NdotH2 = NdotH * NdotH;

			float denom = (NdotH2 * (a2 - 1.0) + 1.0);
			denom = PI * denom * denom;

			return a2 / denom;
		}

		float geometrySchlickGGX(float NdotV, float roughness){
			float r = (roughness + 1.0);
			float k = (r * r) / 8.0;

			float denom = NdotV * (1.0 - k) + k;

			return NdotV / denom;
		}

		float geometrySmith(float NdotV, float NdotL, float roughness){
			float ggx1 = geometrySchlickGGX(NdotV, roughness);
			float ggx2 = geometrySchlickGGX(NdotL, roughness);

			return ggx1 * ggx2;
		}

		vec3 fresnelSchlick(float cosTheta, vec3 F0){
			return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
		}

		vec3 calcBRDF(vec3 albedo, vec3 radiance, vec3 N, vec3 V, vec3 L, vec3 F0, vec2 metalRough){
			vec3 H = normalize(V + L);

			float NdotL = max(dot(N, V), 0.0);
			float NdotV = max(dot(N, L), 0.0);

			float NDF = distributionGGX(N, H, metalRough.g);
			float G = geometrySmith(NdotV, NdotL, metalRough.g);
			vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

			vec3 kS = F;
			vec3 kD = vec3(1.0) - kS;
			kD *= 1.0 - metalRough.r;

			vec3 num = NDF * G * F;
			float denom = 4.0 * NdotV * NdotL + 0.0001;
			vec3 specular = num / denom;

			return (kD * albedo / PI + specular) * radiance * NdotL;
		}

		float calcPCF(int mapIndex, vec3 projectedPos, float currentDepth, sampler2DArray shadowMap){
			float shadow = 0.0;
			float pcfDepth = 0.0;
			vec2 texelSize = 1.0 / textureSize(shadowMap, 0).xy;

			for(int i=-1;i<=1;i++){
				for(int j=-1;j<=1;j++){
					pcfDepth = texture(shadowMap, vec3((projectedPos.xy + vec2(i,j) * texelSize), float(mapIndex))).r;
					shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
				}
			}

			return shadow / 9;
		}

		float calcSunShadow(Sun light, vec3 position, int numShadowCascades, sampler2DArray shadowMap){
			int mapIndex = numShadowCascades - 1;
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

			return calcPCF(mapIndex, projPos, currentDepth, shadowMap);
		}

		vec3 calcSunlight(Sun light, vec3 position, vec3 normal, vec3 albedo, vec3 V, vec3 F0, vec2 metalRough,
			int numShadowCascades, sampler2DArray shadowMap
		){
			vec3 radiance = vec3(0.0);

			vec3 direction = normalize(light.direction);

			if(light.diffuse == vec3(0.0, 0.0, 0.0)){
				radiance = light.ambient * albedo;
				return calcBRDF(albedo, radiance, normal, V, direction, F0, metalRough);
			}

			float diffRatio = max(dot(normal, direction), 0.0);

			vec3 ambient = light.ambient * albedo;
			if(diffRatio <= 0){
				radiance = ambient;
				return calcBRDF(albedo, radiance, normal, V, direction, F0, metalRough);
			}

			vec3 diffuse = light.diffuse * diffRatio * albedo;

			float shadow = calcSunShadow(sun, position.rgb, numShadowCascades, shadowMap);

			radiance = ambient + (1.0 - shadow) * diffuse;

			return calcBRDF(albedo, radiance, normal, V, direction, F0, metalRough);
		}

		vec3 calcPointlight(Pointlight light, vec3 position, vec3 normal, vec3 albedo, vec3 V, vec3 F0, vec2 metalRough){
			vec3 ab = light.position.rgb - position.rgb;
			float distSquare = dot(ab, ab);

			if(distSquare > light.position.a * light.position.a){
				return vec3(0.0);
			}

			vec3 direction = normalize(ab);

			float diffRatio = max(dot(normal, direction), 0.0);
			float attenuation = clamp(1.0 - distSquare / (light.position.a*light.position.a), 0.0, 1.0);

			vec3 ambient = light.ambient * albedo * attenuation;
			vec3 diffuse = light.diffuse * diffRatio * albedo * attenuation;

			vec3 radiance = ambient + diffuse;

			return calcBRDF(albedo, radiance, normal, V, direction, F0, metalRough);
		}

		vec3 calcSpotlight(Spotlight light, vec3 position, vec3 normal, vec3 albedo, vec3 V, vec3 F0, vec2 metalRough){
			vec3 ab = light.position.rgb - position.rgb;
			float distSquare = dot(ab, ab);

			if(distSquare > light.position.a * light.position.a){
				return vec3(0.0);
			}

			vec3 direction = normalize(ab);

			float diffRatio = max(dot(normal, direction), 0.0);
			float attenuation = clamp(1.0 - distSquare / (light.position.a*light.position.a), 0.0, 1.0);

			vec3 ambient = light.ambient * albedo * attenuation;

			vec3 radiance = vec3(0.0);
			
			float theta = dot(direction, normalize(-light.direction.rgb));
			if(theta > cos(light.direction.a)){
				vec3 diffuse = light.diffuse * diffRatio * albedo * attenuation;
				radiance = ambient + diffuse;
			}else{
				radiance = ambient;
			}
			return calcBRDF(albedo, radiance, normal, V, direction, F0, metalRough);
		}

		vec3 toneMapping(vec3 color, float gamma, float exposure){
			vec3 toned = vec3(1.0) - exp(-color * exposure);
			toned = pow(toned, vec3(1.0 / gamma));

			return toned;
		}
	)";
	return str;
}

//-----------------------------------------------------------------------------------------------------

//Shaders for calculating light data.
std::string glsl_deferredLightPassFragment(){
	std::string str = R"(
	#define FOG_DISTANCE 80
	#define NUM_SUN_CASCADES 4

	out vec4 outColor;

	in VS_OUT{
		vec2 uv_coord;
	}F;

	layout(binding = 0) uniform sampler2D u_position;
	layout(binding = 1) uniform sampler2D u_normal;
	layout(binding = 2) uniform sampler2D u_albedo;
	layout(binding = SUN_SHADOW_BASE) uniform sampler2DArray u_sunShadow;

	void main(){

		vec4 normal = texture(u_normal, F.uv_coord).rgba;
		if(normal.rgb != vec3(0.0, 0.0, 0.0)){

			vec4 position = texture(u_position, F.uv_coord).rgba;
			vec4 albedo = texture(u_albedo, F.uv_coord).rgba;
			float distance = position.a;
			vec2 metalRough = vec2(albedo.a, normal.a);

			vec3 result = vec3(0.0);
			if(position.a < FOG_DISTANCE){

				vec3 V = normalize(posTime.rgb - position.rgb);

				vec3 F0 = vec3(0.04);
				F0 = mix(F0, albedo.rgb, metalRough.r);

				//Sunlight
				result += calcSunlight(sun, position.rgb, normal.rgb, albedo.rgb, V, F0, metalRough, NUM_SUN_CASCADES, u_sunShadow);

				//Pointlights
				for(unsigned int i=0;i<numLights.r;i++){
					result += calcPointlight(pointlights[i], position.rgb, normal.rgb, albedo.rgb, V, F0, metalRough);
				}

				//Spotlights
				for(unsigned int i=0;i<numLights.b;i++){
					result += calcSpotlight(spotlights[i], position.rgb, normal.rgb, albedo.rgb, V, F0, metalRough);
				}
				
				//Fog
				float fogRatio = position.a / FOG_DISTANCE;
				result = result * (1 - fogRatio) + sun.ambient * fogRatio;

			}else{

				result = sun.ambient;

			}

			outColor = vec4(toneMapping(result, 1.0, 1.0), 1.0);

		}else{

			outColor = vec4(sun.ambient, 1.0);
			//discard;

		}
	}
	)";	
	str.replace(
		str.find("SUN_SHADOW_BASE"),
		std::string("SUN_SHADOW_BASE").length(),
		std::to_string(SUN_SHADOW_BASE)
	);
	return str;
}
