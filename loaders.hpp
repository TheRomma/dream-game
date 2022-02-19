#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

//Loader for static model data.
struct StaticModelLoader{
	StaticModelLoader(const char* filename);
	~StaticModelLoader();

	//Attribute data.
	Uint32 attribLength;	//Length of attribute data in bytes.
	float* attributes;		//Attribute data like positions, normals, etc.

	//Material data.
	Uint32 texWidth;		//Width of material textures.
	Uint32 texHeight;		//Height of material textures.
	Uint32 texDepth;		//Depth of material textures (texture arrays).

	Uint32 texLength;		//Length of textures in bytes.
	float* diffuse;			//Diffuse texture.

	Uint32 metalRoughLength;//Length of the metallic & roughness map.
	float* metalRough;		//Metallic & roughness map.
};

//Loader for animated model data.
struct AnimatedModelLoader{
	AnimatedModelLoader(const char* filename);
	~AnimatedModelLoader();

	//Attribute data.
	Uint32 attribLength;	//Length of attribute data in bytes.
	float* attributes;		//Attribute data like positions, weights, etc.

	//Material data.
	Uint32 texWidth;		//Width of material textures.
	Uint32 texHeight;		//Height of material textures.
	Uint32 texDepth;		//Depth of material textures (texture arrays).

	Uint32 texLength;		//Length of textures in bytes.
	float* diffuse;			//Diffuse texture.

	Uint32 metalRoughLength;//Length of the metallic & roughness map.
	float* metalRough;		//Metallic & roughness map.

	//Armature data.
	Uint32 numBones;		//Number of bones supported by model.
};

//Loader for skeletal animations.
struct AnimationLoader{
	AnimationLoader(const char* filename);
	~AnimationLoader();
	
	//Animation data.
	Uint32 numFrames;		//Number of frames.
	Uint32 numBones;		//Number of bones.
	Uint32 animRate;		//Playback rate of animation.

	Uint32 animLength;		//Length of animation data in bytes.
	float* animation;		//Skeletal animation data.
};

//Loader for physics mesh data.
struct PhysicsMeshLoader{
	PhysicsMeshLoader(const char* filename);
	~PhysicsMeshLoader();

	//Physics mesh data.
	Uint32 numConvexes;		//Number of convexes in the mesh.
	
	Uint32 vertsLength;		//Length of meshes vertices in bytes.
	float* vertices;		//Vertices of the mesh.

	Uint32 indsLength;		//Length of meshes indices in bytes.
	Uint16* indices;		//Number of vertices in a convex.
};

//Loader for environment map data.
struct EnvironmentMapLoader{
	EnvironmentMapLoader(const char* filename);
	~EnvironmentMapLoader();

	//Material data.
	Uint32 texWidth;		//Width of material texture.
	Uint32 texHeight;		//Height of material texture.

	Uint32 sideLength;		//Length of one side in bytes.
	float* diffuse;			//Diffuse map data.
};
