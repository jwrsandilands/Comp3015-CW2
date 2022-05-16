#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include "helper/plane.h"
#include "helper/torus.h"
#include "helper/objmesh.h"
#include "helper/texture.h"

#include "helper/frustum.h"

#include "helper/noisetex.h"

class SceneBasic_Uniform : public Scene
{
private:
    //GLuint vaoHandle;
    GLSLProgram prog, solidProg;
    
    //gluints
    GLuint shadowFBO, pass1Index, pass2Index, noiseTex;

    //meshes that need to be included
    Plane ground, clouds;
    Torus hills;
    glm::vec3 lightPos; //position of light for clouds
    float angle, tPrev, rotSpeed; //also camera rotation variables

    //numbers for shadowmap
    int shadowMapWidth, shadowMapHeight;

    glm::mat4 lightPV, shadowBias;

    Frustum lightFrustum;

    //obj meshes
    std::unique_ptr<ObjMesh> carBody;
    std::unique_ptr<ObjMesh> carBody2;
    std::unique_ptr<ObjMesh> wheel11;
    std::unique_ptr<ObjMesh> wheel12;
    std::unique_ptr<ObjMesh> wheel13;
    std::unique_ptr<ObjMesh> wheel14;
    std::unique_ptr<ObjMesh> wheel21;
    std::unique_ptr<ObjMesh> wheel22;
    std::unique_ptr<ObjMesh> wheel23;
    std::unique_ptr<ObjMesh> wheel24;

    std::unique_ptr<ObjMesh> Bumper1;
    std::unique_ptr<ObjMesh> Bumper2;

    std::unique_ptr<ObjMesh> Ramp1;
    std::unique_ptr<ObjMesh> Ramp2;

    //textures to be used
    GLuint TexBody1;
    GLuint TexBody2;
    GLuint gTex;
    GLuint wheelTex;
    GLuint bumperTex;
    GLuint rampTex;

    //methods
    void setMatrices();

    void compile();

    void setupFBO();
    void drawScene();
    void spitOutDepthBuffer();

public:
    SceneBasic_Uniform();

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H