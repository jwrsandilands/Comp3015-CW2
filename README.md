# 2022-comp3015-CW2
2022 comp3015, 3D Object and Lighting Demo in OpenGL

### Before you Read

This markdown document is built on top of my original submission markdown document, which detailed several of the same elements. As this project is built on top of the previous submission, much of the content mentioned is the same. However, I have updated it to include the new elements that have been added.

## Program Basics

### Program Build Details

When building the program I used:
  - Visual Studio 2019
  - Windows 10

You can find an executable of the program that functions in this part of the file tree in the folder "executable"

#### How Does it Work?

When the program is run, the user will view two model cars, some bumpers, and grassy ramps on snowy plains from the perspective of a gently rotating camera. This effect is achieved by:
- Loading texture files from a folder and assigning them to variables
- Loading models from OBJ files and scripts.
- Setting the colour value for a single point light.
- Setting up the fog that is used in the fragment shader.
- Setting up the noise maps and materials that will create the effect of snow and clouds in the sky
- Using a frustum and two passes to generate and apply a shadowmap to the scene.
- Texturing and placing the models and light into the scene.
- Rotating the camera around the scene in order to show off multiple angles and light effects on the objects.
- On top of the usual MVP rendering process that would be expected.

#### How Does it fit together?

There are many important files and classes in the project, however the key ones I created are as follows:
- scenebasic_uniform.h sets up all of the necessary objects and texture variables, as well as the key methods to be used in the program
- scenebasic_uniform.cpp executes most of the C++ code, from giving all of those set variables values, to communicating all necessary information to the shader files.
    * In init scene:
        * The Framebuffer, Shadow bias and position of the frustum for the Shadow map is created 
        * Both Diffuse/Specular and Ambient lights colours/intensities
        * Light position
        * Fog's gradient distance and colour.
        * Noise generated for the cloud/snow texture
    * In render:
        * Multiple passes of "drawScene()" are set up for the shadow map and usual perspective rendering
        * The perspective's camera position
        * Set light's position to that of the Frustum to make sure shadows look right
        * Create shadow map projection
    *In drawScene:
        * The objects to use to apply to the object
        * The texture index to apply to the object
        * If the given object should be snowy or considered the sky
    *In setMatrices:
        * The mv matrix
        * The Normal Matrix
        * The MVP
        * The ShadowMatrix
- scenebasic_uniform.cpp also updates the camera position and places the models into the scene before sending the information over to the shaders, as well as creating the viewport and projection matrix.
- basic_uniform.vert takes the vertex information given to it by scenebasic_uniform.cpp and shares the shaped meshes with the basic_uniform.frag
- basic_uniform.frag does most of my shader work. Here, the meshes are given Blinn-Phong style shading, noise is interpreted in multiple ways to affect objects, the shadow map is projected and all ground objects are draped in fog the further they are from the camera.
    * These shader functions use the appropriate equations and are suitibly coded for the potential of using mutiple point lights different point lights in the scene.

#### Extra details?

There a select few things to note about the program that are important:
1. The program uses some given template files from this module, of which I have placed under "helper" filters, to noteably mark they are not my work, however I have edited one or two slightly to get the effect I desired.
2. The car, wheel, bumper, and ramp models and all textures rendered in the project are models/textures I created myself for the COMP3000 module. I decided to reuse them for this project simply as I had them on hand.
3. The car tyres, bumpers, and ramps do not render the last tri in the mesh, and I am still uncertain why exactly this is although I am certain it has something to do with the mesh renderer I am using. It does not effect the overall execution of the model placement, texture mapping, and shading of them, fortunately.
4. The code for implementing cell shading aka "toon shading" is also present in my Fragment Shader code, however it is unused, as I found the low-poly nature of the models did not lend themselves well to showcasing the toon-like shader.
5. The potential for multiple pointlights is still present in the way the fragment shader is built, however I do not utilise this anymore.
6. The clouds and snow both use the same noise map.

Originally, the intention of the project was to create something in a more cartoon style, with cell-shading and multiple lights, however the further I built the project, the more I came to like the more realistic approach with shadow projections and Perlin noise. This is also why I use the Perlin noise for both the clouds and on the ground as snow, as I felt the effect created looked good and fit the new direction.

### Youtube Video

https://youtu.be/ZkZAF6Nq7X4
