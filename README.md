# 2022-comp3015-CW2
2022 comp3015, 3D Object and Lighting Demo in OpenGL

## Program Basics

### Program Build Details

When building the program I used:
  - Visual Studio 2019
  - Windows 10

You can find an executable of the program that functions in this part of the file tree in the folder "executable"

#### How Does it Work?

When the program is run, the user will view two model cars from the perspective of a gently rotating camera. This effect is achieved by:
- Loading texture files from a folder and assigning them to variables
- Loading models from OBJ files and scripts.
- Setting colour values for three seperate point lights.
- Setting up the fog that is used in the fragment shader.
- Texturing and placing the models and lights into the scene.
- Rotating the camera around the scene in order to show off multiple angles and light effects on objects.
- On top of the usual MVP rendering process that would be expected.

#### How Does it fit together?

There are many important files and classes in the project, however the key ones I created are as follows:
- scenebasic_uniform.h sets up all of the necessary objects and texture variables, as well as the key methods to be used in the program
- scenebasic_uniform.cpp executes most of the C++ code, from giving all of those set variables values, to communicating all necessary information to the shader files.
    * In init scene:
        * Both Diffuse/Specular and Ambient lights colours/intensities
        * Fog's gradient distance and colour.
    * In render:
        * Light positions
        * Material specularity and shininess
        * The texture to apply to the object
    *In setMatrices:
        * The mv matrix
        * The Normal Matrix
        * The MVP
- scenebasic_uniform.cpp also updates the camera position and places the models into the scene before sending the information over to the shaders, as well as creating the viewport and projection matrix.
- basic_uniform.vert takes the vertex information given to it by scenebasic_uniform.cpp and shares the shaped meshes with the basic_uniform.frag
- basic_uniform.frag does most of my shader work. Here, the meshes are given Blinn-Phong style shading, and draped in fog the further they are from the camera.
    * These shader functions use the appropriate equations and are suitibly coded for the 3 different point lights included in my scene.

#### Extra details?

There a select few things to note about the program that are important:
1. The program uses some given template files from this module, of which I have placed under "helper" filters, to noteably mark they are not my work, however I have edited one or two slightly to get the effect I desired.
2. Both car and wheel models and all textures rendered in the project are models I created myself for the COMP3000 module. I decided to reuse them for this project simply as I had them on hand.
3. The car tyres do not render the last tri in the mesh, and I am uncertain why this is, but it does not effect the overall exepcution of the model placement, texture mapping, and shading of them.
4. The code for implementing cell shading aka "toon shading" is also present in my Fragment Shader code, however it is unused, as I found the low-poly nature of the models did not lend themselves well to showcasing the toon-like shader.

### Youtube Video

https://youtu.be/ZkZAF6Nq7X4