#include "loaders.hpp"

#include <fstream>
#include <iostream>

//Load static model (aka non animated model) data from file.
StaticModelLoader::StaticModelLoader(const char* filename){
	//Read file.
	std::ifstream file(filename, std::ios::in|std::ios::binary);
	file.seekg(0);
	
	//Attributes.
	file.read((char*)&attribLength, 4);
	attributes = (float*)malloc(attribLength);
	file.read((char*)attributes, attribLength);

	//Material.
	file.read((char*)&texWidth, 4);
	file.read((char*)&texHeight, 4);
	file.read((char*)&texDepth, 4);

	//Temp buffers.
	Uint32 diffColorsLength;	//Length of indexed diffuse color buffer.
	float* diffColors;			//Indexed diffuse color data.
	Uint32 diffIndicesLength;	//Length of the buffer containing the indices.
	Uint16* diffIndices;		//Buffer containing the indices.

	file.read((char*)&diffColorsLength, 4);
	diffColors = (float*)malloc(diffColorsLength);
	file.read((char*)diffColors, diffColorsLength);

	file.read((char*)&diffIndicesLength, 4);
	diffIndices = (Uint16*)malloc(diffIndicesLength);
	file.read((char*)diffIndices, diffIndicesLength);

	file.read((char*)&metalRoughLength, 4);
	metalRough = (float*)malloc(metalRoughLength);
	file.read((char*)metalRough, metalRoughLength);

	//Construct textures from indexed sources.
	Uint32 numTexels = texWidth * texHeight * texDepth;
	texLength = numTexels * 4 * sizeof(float);
	diffuse = (float*)malloc(texLength);
	for(unsigned int i=0;i<numTexels;i++){
		diffuse[i*4] = diffColors[diffIndices[i]*4];
		diffuse[i*4+1] = diffColors[diffIndices[i]*4+1];
		diffuse[i*4+2] = diffColors[diffIndices[i]*4+2];
		diffuse[i*4+3] = diffColors[diffIndices[i]*4+3];
	}

	//Free temp buffers.
	free(diffColors);
	free(diffIndices);

	//Close file.
	file.close();
}

//Destructor for static model loader.
StaticModelLoader::~StaticModelLoader(){
	free(attributes);
	free(diffuse);
	free(metalRough);
}

//Load animated model data from file.
AnimatedModelLoader::AnimatedModelLoader(const char* filename){
	//Read file.
	std::ifstream file(filename, std::ios::in|std::ios::binary);
	file.seekg(0);
	
	//Attributes.
	file.read((char*)&attribLength, 4);
	attributes = (float*)malloc(attribLength);
	file.read((char*)attributes, attribLength);

	//Material.
	file.read((char*)&texWidth, 4);
	file.read((char*)&texHeight, 4);
	file.read((char*)&texDepth, 4);

	//Temp buffers.
	Uint32 diffColorsLength;	//Length of indexed diffuse color buffer.
	float* diffColors;			//Indexed diffuse color data.
	Uint32 diffIndicesLength;	//Length of the buffer containing the indices.
	Uint16* diffIndices;		//Buffer containing the indices.

	file.read((char*)&diffColorsLength, 4);
	diffColors = (float*)malloc(diffColorsLength);
	file.read((char*)diffColors, diffColorsLength);

	file.read((char*)&diffIndicesLength, 4);
	diffIndices = (Uint16*)malloc(diffIndicesLength);
	file.read((char*)diffIndices, diffIndicesLength);

	file.read((char*)&metalRoughLength, 4);
	metalRough = (float*)malloc(metalRoughLength);
	file.read((char*)metalRough, metalRoughLength);

	//Armature.
	file.read((char*)&numBones, 4);

	//Construct textures from indexed sources.
	Uint32 numTexels = texWidth * texHeight * texDepth;
	texLength = numTexels * 4 * sizeof(float);
	diffuse = (float*)malloc(texLength);
	for(unsigned int i=0;i<numTexels;i++){
		diffuse[i*4] = diffColors[diffIndices[i]*4];
		diffuse[i*4+1] = diffColors[diffIndices[i]*4+1];
		diffuse[i*4+2] = diffColors[diffIndices[i]*4+2];
		diffuse[i*4+3] = diffColors[diffIndices[i]*4+3];
	}

	//Free temp buffers.
	free(diffColors);
	free(diffIndices);

	//Close file.
	file.close();
}

//Destructor for Animated model loader.
AnimatedModelLoader::~AnimatedModelLoader(){
	free(attributes);
	free(diffuse);
	free(metalRough);
}

//Load skeletal animation files from file.
AnimationLoader::AnimationLoader(const char* filename){
	std::ifstream file(filename, std::ios::in|std::ios::binary);
	file.seekg(0);

	file.read((char*)&numFrames, 4);
	file.read((char*)&numBones, 4);
	file.read((char*)&animRate, 4);

	file.read((char*)&animLength, 4);
	animation = (float*)malloc(animLength);
	file.read((char*)animation, animLength);

	file.close();
}

//Destructor for animation loader.
AnimationLoader::~AnimationLoader(){
	free(animation);
}

//Load complex collision meshes from a file.
PhysicsMeshLoader::PhysicsMeshLoader(const char* filename){
	std::ifstream file(filename, std::ios::in|std::ios::binary);
	file.seekg(0);

	file.read((char*)&numConvexes, 4);

	file.read((char*)&vertsLength, 4);
	vertices = (float*)malloc(vertsLength);
	file.read((char*)vertices, vertsLength);

	file.read((char*)&indsLength, 4);
	indices = (Uint16*)malloc(indsLength);
	file.read((char*)indices, indsLength);

	file.close();
}

//Destructor for physics mesh loader.
PhysicsMeshLoader::~PhysicsMeshLoader(){
	free(vertices);
	free(indices);
}

//Load environtment map data from file.
EnvironmentMapLoader::EnvironmentMapLoader(const char* filename){
	//Read file.
	std::ifstream file(filename, std::ios::in|std::ios::binary);
	file.seekg(0);
	
	//Material.
	file.read((char*)&texWidth, 4);
	file.read((char*)&texHeight, 4);

	//Temp buffers.
	Uint32 diffColorsLength;	//Length of indexed diffuse color buffer.
	float* diffColors;			//Indexed diffuse color data.
	Uint32 diffIndicesLength;	//Length of the buffer containing the indices.
	Uint16* diffIndices;		//Buffer containing the indices.

	file.read((char*)&diffColorsLength, 4);
	diffColors = (float*)malloc(diffColorsLength);
	file.read((char*)diffColors, diffColorsLength);

	file.read((char*)&diffIndicesLength, 4);
	diffIndices = (Uint16*)malloc(diffIndicesLength);
	file.read((char*)diffIndices, diffIndicesLength);

	//Construct textures from indexed sources.
	Uint32 numTexels = texWidth * texHeight;
	sideLength = numTexels * 3 * sizeof(float);
	diffuse = (float*)malloc(sideLength);
	for(unsigned int i=0;i<numTexels;i++){
		diffuse[i*3] = diffColors[diffIndices[i]*3];
		diffuse[i*3+1] = diffColors[diffIndices[i]*3+1];
		diffuse[i*3+2] = diffColors[diffIndices[i]*3+2];
	}

	texHeight /= 6;
	sideLength /= 24;

	//Free temp buffers.
	free(diffColors);
	free(diffIndices);

	//Close file.
	file.close();
}

//Destructor for static model loader.
EnvironmentMapLoader::~EnvironmentMapLoader(){
	free(diffuse);
}
