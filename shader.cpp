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

		#define PI 3.14159265358979323846264

		float noise(vec2 coord){
			return fract(sin(dot(
				coord.xy, vec2(12.9898, 78.233))) * 43758.5453123 * posTime.a
			);
		}

		vec3 toneMapping(vec3 color, float gamma, float exposure){
			vec3 toned = vec3(1.0) - exp(-color * exposure);
			toned = pow(toned, vec3(1.0 / gamma));

			return toned;
		}

		float noise(vec3 coord){
			return fract(sin(dot(
				coord.xyz, vec3(12.9898, 78.233, 144.7272))) * 43758.5453123 * posTime.a
			);
		}
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
			vec4 position;	//vec3 position, float radius
			//vec3 ambient;
			vec3 diffuse;
		};

		struct Spotlight{
			vec4 position;	//vec3 position, float radius
			vec4 direction;	//vec3 direction, float cutoff
			//vec3 ambient;
			vec3 diffuse;
			mat4 view;
		};

		layout(std140, binding = LIGHT_UBO_BINDING) uniform L{
			Sun sun;
			vec4 numLights;	//r = numPointlights, b = numSpotlights
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
			vec4 transform = u_model * vec4(POSITION, 1.0);
			vec4 result = projView * transform;
			gl_Position = result;

			F.position = vec4(transform.rgb/transform.a, result.z);
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
			vec4 diffColor = texture(u_diffuse, F.uv_coord).rgba;
			if(diffColor.a < 1.0){
				/*
				if(noise(gl_FragCoord.xyz) < diffColor.a){
					gPosition = F.position;
					vec2 metalRough = texture(u_metalRough, F.uv_coord).rg;
					gNormal = vec4(normalize(F.normal), metalRough.g);
					gAlbedo = vec4(diffColor.rgb, metalRough.r);
				}else{
					discard;
				}
				*/
				discard;
			}else{
				gPosition = F.position;
				vec2 metalRough = texture(u_metalRough, F.uv_coord).rg;
				gNormal = vec4(normalize(F.normal), metalRough.g);
				gAlbedo = vec4(diffColor.rgb, metalRough.r);
			}
		}
	)";	
	return str;
}

std::string glsl_staticModelShadowVertex(){
	std::string str = R"(
	layout(location = 0) in vec3 POSITION;
	layout(location = 1) in vec3 UV_COORD;

	out VS_OUT{
		vec3 uv_coord;
	} F;

	layout(location = 0) uniform mat4 u_model;
	layout(location = 1) uniform mat4 u_lightSpace;

	void main(){
		gl_Position = u_lightSpace * u_model * vec4(POSITION, 1.0);
		F.uv_coord = UV_COORD;
	}
	)";	
	return str;
}

std::string glsl_allModelShadowFragment(){
	std::string str = R"(
	layout(binding = 0) uniform sampler2DArray u_diffuse;

	in VS_OUT{
		vec3 uv_coord;
	}F;

	void main(){
		float alpha = texture(u_diffuse, F.uv_coord).a;
		/*
		if(alpha < 1.0){
			if(noise(gl_FragCoord.xyz) < alpha){
				discard;
			}else{
				//Empty
			}
		}else{
			//Empty
		}
		*/
		if(alpha < 1.0){
			discard;
		}
	}
	)";	
	return str;
}

/*
std::string glsl_allModelShadowGeometry(){
	std::string str = R"(
	#define NUM_SUN_CASCADES NUM_SUN_SHADOWS

	layout(triangles, invocations = NUM_SUN_CASCADES) in;
	layout(triangle_strip, max_vertices = 3) out;
	
	void main(){
		for(int i=0;i<NUM_SUN_CASCADES;i++){
			gl_Position = 
				sun.view[gl_InvocationID] * gl_in[i].gl_Position;
			gl_Layer = gl_InvocationID;
			EmitVertex();
		}

		EndPrimitive();
	}
	)";	
	str.replace(
		str.find("NUM_SUN_SHADOWS"),
		std::string("NUM_SUN_SHADOWS").length(),
		std::to_string(SUN_NUM_SHADOW_CASCADES)
	);
	return str;
}
*/

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
		layout(location = 2) uniform mat4 u_joints[NUM_BONES];

		void main(){
			
			vec4 transform = u_model * (
				u_joints[int(BONES.r)] * vec4(POSITION, 1.0) * WEIGHTS.r +
				u_joints[int(BONES.g)] * vec4(POSITION, 1.0) * WEIGHTS.g +
				u_joints[int(BONES.b)] * vec4(POSITION, 1.0) * WEIGHTS.b +
				u_joints[int(BONES.a)] * vec4(POSITION, 1.0) * WEIGHTS.a
			);
			
			vec4 result = projView * transform;
			gl_Position = result;

			F.position = vec4(transform.rgb/transform.a, result.z);
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

	out VS_OUT{
		vec3 uv_coord;
	} F;

	layout(location = 0) uniform mat4 u_model;
	layout(location = 1) uniform mat4 u_lightSpace;
	layout(location = 2) uniform mat4 u_joints[NUM_BONES];

	void main(){
		
		vec4 pos = u_model * (
			u_joints[int(BONES.r)] * vec4(POSITION, 1.0) * WEIGHTS.r +
			u_joints[int(BONES.g)] * vec4(POSITION, 1.0) * WEIGHTS.g +
			u_joints[int(BONES.b)] * vec4(POSITION, 1.0) * WEIGHTS.b +
			u_joints[int(BONES.a)] * vec4(POSITION, 1.0) * WEIGHTS.a
		);
		gl_Position = u_lightSpace * pos;
		F.uv_coord = UV_COORD;
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
			vec3 color = texture(u_image, F.uv_coord).rgb;
			outColor = vec4(toneMapping(color, 1.0, 1.1), 1.0);
			//outColor = texture(u_image, vec2((F.uv_coord.x + cos(posTime.a + F.uv_coord.y * 10) * 0.02) * 0.96 + 0.02, F.uv_coord.y));
			//outColor = texture(u_image, vec2((F.uv_coord.x + noise(gl_FragCoord.xyz) * 0.1) * 0.8 + 0.1, F.uv_coord.y));
		}
	)";	
	return str;
}

//------------------------------------------------------------------------------------------------

//Contains various light calculation functions in glsl. Requires commonLightStructs.
std::string glsl_lightCalculations(){
	std::string str = R"(
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

			float NdotV = max(dot(N, V), 0.0);
			float NdotL = max(dot(N, L), 0.0);

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
					shadow += (currentDepth) > (pcfDepth) ? 1.0 : 0.0;
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

			vec3 ambient = light.ambient * albedo * 0.2;

			if(light.diffuse == vec3(0.0, 0.0, 0.0)){
				//radiance = ambient;
				//return calcBRDF(albedo, radiance, normal, V, direction, F0, metalRough);
				return ambient;
			}

			float diffRatio = max(dot(normal, direction), 0.0);

			if(diffRatio <= 0){
				//radiance = ambient;
				//return calcBRDF(albedo, radiance, normal, V, direction, F0, metalRough);
				return ambient;
			}

			vec3 diffuse = light.diffuse * diffRatio * albedo;

			float shadow = calcSunShadow(sun, position.rgb, numShadowCascades, shadowMap);

			radiance = (1.0 - shadow) * diffuse;

			return calcBRDF(albedo, radiance, normal, V, direction, F0, metalRough) + ambient;
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

			//vec3 ambient = light.ambient * albedo * attenuation;
			vec3 diffuse = light.diffuse * diffRatio * albedo * attenuation;

			vec3 radiance = diffuse;

			return calcBRDF(albedo, radiance, normal, V, direction, F0, metalRough);
		}

		float calcSpotShadow(Spotlight light, vec3 position, int index, sampler2DArray shadowMap){
			vec3 pos = posTime.rgb;

			vec4 lightSpaceFrag = light.view * vec4(position.rgb, 1.0);
			vec3 projPos = lightSpaceFrag.xyz / lightSpaceFrag.w;
			if(projPos.z > 1.0){return 0.0;}
			projPos = projPos * 0.5 + 0.5;
			float currentDepth = projPos.z;
			
			float shadow = 0.0;
			float pcfDepth = 0.0;
			vec2 texelSize = 1.0 / textureSize(shadowMap, 0).xy;

			return calcPCF(index, projPos, currentDepth, shadowMap);
		}

		vec3 calcSpotlight(Spotlight light, int index, vec3 position, vec3 normal, vec3 albedo, vec3 V, vec3 F0, vec2 metalRough, sampler2DArray shadowMap){
			vec3 ab = light.position.rgb - position.rgb;
			float distSquare = dot(ab, ab);

			if(distSquare > light.position.a * light.position.a){
				return vec3(0.0);
			}

			vec3 direction = normalize(ab);

			float diffRatio = max(dot(normal, direction), 0.0);
			float attenuation = clamp(1.0 - distSquare / (light.position.a*light.position.a), 0.0, 1.0);

			//vec3 ambient = light.ambient * albedo * attenuation;

			//vec3 radiance = vec3(0.0);
			
			float theta = dot(direction, normalize(-light.direction.rgb));
			if(theta > light.direction.a){
				vec3 diffuse = light.diffuse * diffRatio * albedo * attenuation;
				float shadow = calcSpotShadow(light, position, index, shadowMap);
				
				float epsilon = 0.05;
				float intensity = clamp((theta - (light.direction.a + epsilon)) / epsilon, 0.0, 1.0);

				vec3 radiance = (1.0 - shadow) * (diffuse * intensity);
				return calcBRDF(albedo, radiance, normal, V, direction, F0, metalRough);
			}else{
				return vec3(0.0);
			}
		}
	)";
	return str;
}

//-----------------------------------------------------------------------------------------------------

//Shaders for calculating light data.
std::string glsl_deferredLightPassFragment(){
	std::string str = R"(
	#define FOG_DISTANCE 200
	#define NUM_SUN_CASCADES 4

	layout(location = 0) out vec4 outColor;

	in VS_OUT{
		vec2 uv_coord;
	}F;

	layout(binding = 0) uniform sampler2D u_position;
	layout(binding = 1) uniform sampler2D u_normal;
	layout(binding = 2) uniform sampler2D u_albedo;
	layout(binding = SHADOW_BASE) uniform sampler2DArray u_shadowMap;
	layout(binding = 4) uniform samplerCube u_environmentMap;

	void main(){

		vec3 shadowTest = texture(u_shadowMap, vec3(F.uv_coord, 4.0)).rgb;
		vec4 normal = texture(u_normal, F.uv_coord).rgba;
		if(normal.rgb != vec3(0.0, 0.0, 0.0)){

			vec4 position = texture(u_position, F.uv_coord).rgba;
			vec4 albedo = texture(u_albedo, F.uv_coord).rgba;
			float distance = position.a;
			vec2 metalRough = vec2(albedo.a, normal.a);

			vec3 result = vec3(0.0);
			if(distance < FOG_DISTANCE){

				vec3 V = normalize(posTime.rgb - position.rgb);

				vec3 F0 = vec3(0.04);
				F0 = mix(F0, albedo.rgb, metalRough.r);

				int offset = 0;

				if(metalRough.r > 0.0){
					float rough = metalRough.g * textureQueryLevels(u_environmentMap) * 0.8;
					vec3 R = reflect(-V, normal.rgb);
					result += metalRough.r * albedo.rgb * textureLod(u_environmentMap, vec3(R.x, -R.z, -R.y), rough).rgb;
				}

				//Sunlight
				result += calcSunlight(sun, position.rgb, normal.rgb, albedo.rgb, V, F0, metalRough, NUM_SUN_CASCADES, u_shadowMap);

				//Pointlights
				for(int i=0;i<numLights.r;i++){
					result += calcPointlight(pointlights[i], position.rgb, normal.rgb, albedo.rgb, V, F0, metalRough);
				}

				offset += NUM_SUN_CASCADES;
				//Spotlights
				for(int i=0;i<numLights.b;i++){
					result += calcSpotlight(spotlights[i], offset + i, position.rgb, normal.rgb, albedo.rgb, V, F0, metalRough, u_shadowMap);
				}
				
				//Fog
				float fogRatio = distance / FOG_DISTANCE;
				result = result * (1 - fogRatio) + sun.ambient * 0.2 * fogRatio;

			}else{

				result = sun.ambient * 0.2;

			}
			outColor = vec4(result.rgb, 1.0);
			//outColor = vec4(position.rgb, 1.0);

		}else{

			//outColor = vec4(sun.ambient, 1.0);
			//outColor = vec4(shadowTest, 1.0);
			discard;

		}
	}
	)";	
	str.replace(
		str.find("SHADOW_BASE"),
		std::string("SHADOW_BASE").length(),
		std::to_string(SHADOW_BASE)
	);
	return str;
}

//----------------------------------------------------------------------------------------

std::string glsl_environmentVertex(){
	std::string str = R"(
	layout(location = 0) in vec3 POSITION;

	out VS_OUT{
		vec3 uv_coord;
	} F;

	void main()
	{
		mat4 originView = projView;
		originView[3][0] = 0;
		originView[3][1] = 0;
		originView[3][2] = 0;
		originView[3][3] = 0.01;

		gl_Position = originView * vec4(POSITION, 1.0);
		//gl_Position = pos.xyzw;

		F.uv_coord = POSITION;
	}
	)";
	return str;
}

std::string glsl_environmentFragment(){
	std::string str = R"(
	out vec4 outColor;

	in VS_OUT{
		vec3 uv_coord;
	}F;

	layout(binding = 0) uniform samplerCube diffuse;

	void main()
	{
		vec4 mapColor = texture(diffuse, vec3(F.uv_coord.x, -F.uv_coord.z, -F.uv_coord.y));
		outColor = mapColor * 0.9 + vec4(sun.ambient * 0.1, 1.0);
	}
	)";
	return str;
}

//-----------------------------------------------------------------------------------------------------------

std::string glsl_bloomFragment(){
	std::string str = R"(
		out vec4 outColor;

		in VS_OUT{
			vec2 uv_coord;
		}F;

		layout(binding = 0) uniform sampler2D u_image;

		void main(){
			vec3 color = texture(u_image, F.uv_coord).rgb;
			float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722)); 
			if(brightness > 1.0){
				outColor = vec4(color * 0.1, 1.0);
			}else{
				outColor = vec4(0.0, 0.0, 0.0, 1.0);
			}
		}
	)";
	return str;
}

std::string glsl_kernelFragment(){
	std::string str = R"(
		out vec4 outColor;

		in VS_OUT{
			vec2 uv_coord;
		}F;

		layout(binding = 0) uniform sampler2D u_image;
		layout(location = 1) uniform mat3 u_kernel;

		void main(){
			vec2 texelSize = 1.0 / textureSize(u_image, 0).xy;
			vec3 color = vec3(0.0);

			for(int i=0;i<3;i++){
				for(int j=0;j<3;j++){
					color += u_kernel[j][i] * texture(u_image, (F.uv_coord + vec2(texelSize.x * (i - 1), texelSize.y * (j - 1)))).rgb;
				}
			}


			outColor = vec4(color, 1.0);
		}
	)";
	return str;
}

std::string glsl_gaussianBlurFragment(){
	std::string str = R"(
		out vec4 outColor;

		in VS_OUT{
			vec2 uv_coord;
		}F;

		layout(binding = 0) uniform sampler2D u_image;
		layout(location = 1) uniform bool u_horizontal;

		uniform float weights[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

		void main(){
			vec2 texelSize = 1.0 / textureSize(u_image, 0).xy;
			vec3 color = texture(u_image, F.uv_coord).rgb * weights[0];

			if(u_horizontal){
				for(int i=1;i<5;i++){
					color += texture(u_image, F.uv_coord + vec2(texelSize.x * i, 0.0)).rgb * weights[i];
					color += texture(u_image, F.uv_coord - vec2(texelSize.x * i, 0.0)).rgb * weights[i];
				}
			}else{
				for(int i=1;i<5;i++){
					color += texture(u_image, F.uv_coord + vec2(0.0, texelSize.y * i)).rgb * weights[i];
					color += texture(u_image, F.uv_coord - vec2(0.0, texelSize.y * i)).rgb * weights[i];
				}
			}

			outColor = vec4(color, 1.0);
		}
	)";
	return str;
}

std::string glsl_combineFragment(){
	std::string str = R"(
		out vec4 outColor;

		in VS_OUT{
			vec2 uv_coord;
		}F;

		layout(binding = 0) uniform sampler2D u_imgFirst;
		layout(binding = 1) uniform sampler2D u_imgSecond;

		void main(){
			outColor = vec4(texture(u_imgFirst, F.uv_coord).rgb + texture(u_imgSecond, F.uv_coord).rgb, 1.0);
			//outColor = vec4(texture(u_imgFirst, F.uv_coord).rgb, 1.0);
		}
	)";
	return str;
}
