#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include "helper/plane.h"
#include "helper/torus.h"
#include "helper/objmesh.h"
#include "helper/texture.h"

class SceneBasic_Uniform : public Scene
{
private:
    //GLuint vaoHandle;
    GLSLProgram prog;
    
    //meshes that need to be included
    Plane ground;
    Torus hills;
    float angle, tPrev, rotSpeed; //also camera rotation variables

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

    //textures to be used
    GLuint TexBody1;
    GLuint TexBody2;
    GLuint gTex;
    GLuint wheelTex;

    void setMatrices();

    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H