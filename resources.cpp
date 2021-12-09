#include "resources.hpp"

//--------------------------------------------------------------------------------------------

//Static model file constructor.
StaticModelFile::StaticModelFile(const char* filename){
	std::ifstream file(filename, std::ios::in|std::ios::binary);
	file.seekg(0);

	file.read((char*)&attribLength, 4);
	attributes = (char*)malloc(attribLength);
	file.read(attributes, attribLength);

	file.read((char*)&texWidth, 4);
	file.read((char*)&texHeight, 4);
	file.read((char*)&texChannels, 4);
	file.read((char*)&texFilter, 4);

	Uint32 colorsLength;
	float* colors;
	Uint32 texelsLength;
	Uint16* texels;

	file.read((char*)&colorsLength, 4);
	colors = (float*)malloc(colorsLength);
	file.read((char*)colors, colorsLength);

	file.read((char*)&texelsLength, 4);
	texels = (Uint16*)malloc(texelsLength);
	file.read((char*)texels, texelsLength);

	Uint32 numTexels = texWidth * texHeight;
	texLength = numTexels * 4 * sizeof(float);
	texture = (float*)malloc(texLength);
	for(unsigned int i=0;i<numTexels;i++){
		texture[i*4] = colors[texels[i]*4];
		texture[i*4+1] = colors[texels[i]*4+1];
		texture[i*4+2] = colors[texels[i]*4+2];
		texture[i*4+3] = colors[texels[i]*4+3];
	}

	free(colors);
	free(texels);
	file.close();
}

//Static model file destructor.
StaticModelFile::~StaticModelFile(){
	free(attributes);
	free(texture);
}

//----------------------------------------------------------------------------------------------

//Animated model file constructor.
AnimatedModelFile::AnimatedModelFile(const char* filename){
	std::ifstream file(filename, std::ios::in|std::ios::binary);
	file.seekg(0);

	file.read((char*)&attribLength, 4);
	attributes = (char*)malloc(attribLength);
	file.read(attributes, attribLength);

	file.read((char*)&texWidth, 4);
	file.read((char*)&texHeight, 4);
	file.read((char*)&texChannels, 4);
	file.read((char*)&texFilter, 4);

	Uint32 colorsLength;
	float* colors;
	Uint32 texelsLength;
	Uint16* texels;

	file.read((char*)&colorsLength, 4);
	colors = (float*)malloc(colorsLength);
	file.read((char*)colors, colorsLength);

	file.read((char*)&texelsLength, 4);
	texels = (Uint16*)malloc(texelsLength);
	file.read((char*)texels, texelsLength);

	Uint32 numTexels = texWidth * texHeight;
	texLength = numTexels * 4 * sizeof(float);
	texture = (float*)malloc(texLength);
	for(unsigned int i=0;i<numTexels;i++){
		texture[i*4] = colors[texels[i]*4];
		texture[i*4+1] = colors[texels[i]*4+1];
		texture[i*4+2] = colors[texels[i]*4+2];
		texture[i*4+3] = colors[texels[i]*4+3];
	}

	file.read((char*)&numBones, 4);

	free(colors);
	free(texels);
	file.close();
}

//Animated model file destructor.
AnimatedModelFile::~AnimatedModelFile(){
	free(attributes);
	free(texture);
}

//------------------------------------------------------------------------------------------------

//Multi model file constructor.
MultiModelFile::MultiModelFile(const char* filename){
	std::ifstream file(filename, std::ios::in|std::ios::binary);
	file.seekg(0);

	file.read((char*)&attribLength, 4);
	attributes = (char*)malloc(attribLength);
	file.read(attributes, attribLength);

	file.read((char*)&texWidth, 4);
	file.read((char*)&texHeight, 4);
	file.read((char*)&texDepth, 4);
	file.read((char*)&texChannels, 4);
	file.read((char*)&texFilter, 4);

	Uint32 colorsLength;
	float* colors;
	Uint32 texelsLength;
	Uint16* texels;

	file.read((char*)&colorsLength, 4);
	colors = (float*)malloc(colorsLength);
	file.read((char*)colors, colorsLength);

	file.read((char*)&texelsLength, 4);
	texels = (Uint16*)malloc(texelsLength);
	file.read((char*)texels, texelsLength);

	Uint32 numTexels = texWidth * texHeight * texDepth;
	texLength = numTexels * 4 * sizeof(float);
	texture = (float*)malloc(texLength);
	for(unsigned int i=0;i<numTexels;i++){
		texture[i*4] = colors[texels[i]*4];
		texture[i*4+1] = colors[texels[i]*4+1];
		texture[i*4+2] = colors[texels[i]*4+2];
		texture[i*4+3] = colors[texels[i]*4+3];
	}

	free(colors);
	free(texels);
	file.close();
}

//Multi model file destructor.
MultiModelFile::~MultiModelFile(){
	free(attributes);
	free(texture);
}

//Print file values
void MultiModelFile::print(){
	std::cout<<"attribLength: "<<attribLength<<std::endl;
	std::cout<<"texWidth: "<<texWidth<<std::endl;
	std::cout<<"texHeight: "<<texHeight<<std::endl;
	std::cout<<"texDepth: "<<texDepth<<std::endl;
	std::cout<<"texFilter: "<<texFilter<<std::endl;
	std::cout<<"texLength: "<<texLength<<std::endl;
}

//-------------------------------------------------------------------------------------------------


//Physics mesh file constructor.
PhysicsMeshFile::PhysicsMeshFile(const char* filename){
	std::ifstream file(filename, std::ios::in|std::ios::binary);
	file.seekg(0);

	file.read((char*)&numConvexes, 4);

	file.read((char*)&vertsLength, 4);
	vertices = (char*)malloc(vertsLength);
	file.read(vertices, vertsLength);

	file.read((char*)&indsLength, 4);
	indices = (char*)malloc(indsLength);
	file.read(indices, indsLength);

	file.close();
}

//Physics mesh file destructor.
PhysicsMeshFile::~PhysicsMeshFile(){
	free(vertices);
	free(indices);
}

//------------------------------------------------------------------------------------------------

TextureFile::TextureFile(const char* filename){
	std::ifstream file(filename, std::ios::in|std::ios::binary);
	file.seekg(0);

	file.read((char*)&texWidth, 4);
	file.read((char*)&texHeight, 4);

	Uint32 colorsLength;
	float* colors;
	Uint32 texelsLength;
	Uint16* texels;

	file.read((char*)&colorsLength, 4);
	colors = (float*)malloc(colorsLength);
	file.read((char*)colors, colorsLength);

	file.read((char*)&texelsLength, 4);
	texels = (Uint16*)malloc(texelsLength);
	file.read((char*)texels, texelsLength);

	Uint32 numTexels = texWidth * texHeight;
	texLength = numTexels * 4 * sizeof(float);
	texture = (float*)malloc(texLength);
	for(unsigned int i=0;i<numTexels;i++){
		texture[i*4] = colors[texels[i]*4];
		texture[i*4+1] = colors[texels[i]*4+1];
		texture[i*4+2] = colors[texels[i]*4+2];
		texture[i*4+3] = colors[texels[i]*4+3];
	}

	free(colors);
	free(texels);
	file.close();
}

TextureFile::~TextureFile(){
	free(texture);
}
