#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"

#include <glm/gtc/matrix_transform.hpp>
using glm::vec3;
using glm::mat4;

#include <sstream>

SceneBasic_Uniform::SceneBasic_Uniform() : ground(50.0f, 50.0f, 1, 1), clouds(100.0f, 100.0f, 4, 4), hills(50, 10, 20, 10), angle(0.0f), tPrev(0.0f), rotSpeed(glm::pi<float>() / 8.0f), shadowMapWidth(512), shadowMapHeight(512) {
	TexBody1 = Texture::loadTexture("media/textures/CarBodyTexture.png");
	TexBody2 = Texture::loadTexture("media/textures/CarBodyTexture2.png");
	gTex = Texture::loadTexture("media/textures/Grass.png");
	wheelTex = Texture::loadTexture("media/textures/CarWheelTexture.png");

	bumperTex = Texture::loadTexture("media/textures/BumperTexture.png");
	rampTex = Texture::loadTexture("media/textures/RampTexture.png");


	carBody = ObjMesh::load("media/models/CarBody1.obj", false, true);
	carBody2 = ObjMesh::load("media/models/CarBody1.obj", false, true);
	wheel11 = ObjMesh::load("media/models/CarTyre.obj", false, true);
	wheel12 = ObjMesh::load("media/models/CarTyre.obj", false, true);
	wheel13 = ObjMesh::load("media/models/CarTyre.obj", false, true);
	wheel14 = ObjMesh::load("media/models/CarTyre.obj", false, true);
	wheel21 = ObjMesh::load("media/models/CarTyre.obj", false, true);
	wheel22 = ObjMesh::load("media/models/CarTyre.obj", false, true);
	wheel23 = ObjMesh::load("media/models/CarTyre.obj", false, true);
	wheel24 = ObjMesh::load("media/models/CarTyre.obj", false, true);

	Bumper1 = ObjMesh::load("media/models/RCRBumper.obj", false, true);
	Bumper2 = ObjMesh::load("media/models/RCRBumper.obj", false, true);

	Ramp1 = ObjMesh::load("media/models/RCR Ramp.obj", false, true);
	Ramp2 = ObjMesh::load("media/models/RCR Ramp.obj", false, true);
}

void SceneBasic_Uniform::initScene() {
	compile();	//compile, link, and use shaders

	glClearColor(0.6f, 0.8f, 1.0f, 1.0f);	//set up background colour

	glEnable(GL_DEPTH_TEST);	//enable depth test

	//set up scene
	projection = mat4(1.0f);

	angle = glm::radians(90.0f);

	//set framebuffer object
	setupFBO();

	//get depth, used for shadow texture
	GLuint programHandle = prog.getHandle();
	pass1Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "recordDepth");
	pass2Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "shadeWithShadow");

	shadowBias = mat4(
		glm::vec4(0.5f, 0.0f, 0.0f, 0.0f), 
		glm::vec4(0.0f, 0.5f, 0.0f, 0.0f), 
		glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
		glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

	////Specular and Diffuse light colours
	//prog.setUniform("lights[0].L", vec3(0.0f, 0.6f, 1.5f)); //light blue
	//prog.setUniform("lights[1].L", vec3(1.5f, 0.6f, 0.0f)); //orange
	//prog.setUniform("lights[2].L", vec3(0.6f, 0.0f, 1.5f)); //purple
	////Ambient Light Colours
	//prog.setUniform("lights[0].La", vec3(0.0f, 0.3f, 0.6f)); //light blue
	//prog.setUniform("lights[1].La", vec3(0.6f, 0.3f, 0.0f)); //orange
	//prog.setUniform("lights[2].La", vec3(0.3f, 0.0f, 0.6f)); //purple

	//set up single light for shadows test
	prog.setUniform("lights[0].L", vec3(0.8f, 0.8f, 0.8f));
	prog.setUniform("lights[0].La", vec3(0.8f, 0.8f, 0.8f));

	//set up the lights' positions
	float x, z; //position co-ords
	for (int i = 0; i < 1; i++) {
		std::stringstream name;
		name << "lights[" << i << "].Position";
		x = 2.0f * cosf((glm::two_pi<float>() / 3) * i);
		z = 2.0f * sinf((glm::two_pi<float>() / 3) * i);
		//send calculated information to the shaders
		//prog.setUniform(name.str().c_str(), view * glm::vec4(x, 1.2f, z + 1.0f, 1.0f));

		//set up MVP for shadows
		//view = lookAt(vec3(x, 1.2f, z + 1.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f)); //set view position to light position TEMP
		prog.setUniform(name.str().c_str(), view * glm::vec4(x, 2.2f, z + 1.0f, 1.0f)); //set light position

		vec3 lightPos = vec3(x, 2.2f, z + 1.0f); //world coords
		lightFrustum.orient(lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
		lightFrustum.setPerspective(70.0f, 1.0f, 2.0f, 10.0f);
		lightPV = shadowBias * lightFrustum.getProjectionMatrix() * lightFrustum.getViewMatrix();

		//view = lookAt(vec3(x, 1.2f, z + 1.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f)); //set view position to light position TEMP
		//projection = glm::perspective(glm::radians(70.0f), (float)width / height, 0.3f, 100.0f); //set perspective from the light
		//lightPV = shadowBias * projection * view;

		prog.setUniform("ShadowMap", 0);
	}


	//set up fog
	prog.setUniform("fogInfo.MaxDist", 40.0f);
	prog.setUniform("fogInfo.MinDist", 10.0f);
	prog.setUniform("fogInfo.Color", vec3(0.7f, 0.9f, 1.0f));

	//set up noise for cloud/snow
	prog.setUniform("NoiseTex", 2);
	glActiveTexture(GL_TEXTURE2);
	noiseTex = NoiseTex::generate2DTex(10.0f);
}

void SceneBasic_Uniform::compile() {
	try {
		prog.compileShader("shaders/basic_uniform.vert");
		prog.compileShader("shaders/basic_uniform.frag");
		prog.link();
		prog.use();

		//when rendering light frustrum
		solidProg.compileShader("shaders/solid.vs", GLSLShader::VERTEX);
		solidProg.compileShader("shaders/solid.fs", GLSLShader::FRAGMENT);
		solidProg.link();

	}
	catch (GLSLProgramException& e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update(float t) {
	float deltaT = t - tPrev;

	if (tPrev == 0.0f) {
		deltaT = 0.0f;
	}

	tPrev = t;

	angle += rotSpeed * deltaT;

	if (angle > glm::two_pi<float>()) {
		angle -= glm::two_pi<float>();
	}
}

void SceneBasic_Uniform::render() {

	prog.use();
	//Pass1 (shadow map generation)
	view = lightFrustum.getViewMatrix();
	projection = lightFrustum.getProjectionMatrix();
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, shadowMapWidth, shadowMapHeight);
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass1Index);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.5f, 10.0f);
	drawScene();
	glDisable(GL_CULL_FACE);
	glFlush();
	//spitOutDepthBuffer(); //Get an image of the depth buffer.

	//pass 2 (render)
	view = glm::lookAt(vec3(2.5f * cos(angle), 0.7f, -2.5f * sin(angle)), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)); //camera position co-ords
	//view = glm::lookAt(vec3(7, 7, -7), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)); //camera position co-ords

	prog.setUniform("lights[0].Position", view * glm::vec4(lightFrustum.getOrigin(), 1.0f));
	projection = glm::perspective(glm::radians(70.0f), (float)width / height, 0.3f, 100.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass2Index);
	drawScene();

	////draw light frustum wireframe to test its shape
	//solidProg.use();
	//solidProg.setUniform("Color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	//mat4 mv = view * lightFrustum.getInverseViewMatrix();
	//solidProg.setUniform("MVP", projection * mv);
	//lightFrustum.render();
}

void SceneBasic_Uniform::resize(int w, int h) {
	//setup the viewport and the projection matrix
	glViewport(0, 0, w, h);
	width = w;
	height = h;

	//projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices() {
	mat4 mv = view * model;								//set up Model View matrix
	prog.setUniform("ModelViewMatrix", mv);				//send MV matrix
	prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));  //send Normal Matrix
	prog.setUniform("MVP", projection * mv);			//send MVP
	prog.setUniform("ShadowMatrix", lightPV * model);   //send shadow matrix
}

void SceneBasic_Uniform::setupFBO() {
	
	GLfloat border[] = { 1.0f, 0.0f, 0.0f, 0.0f }; //border colour to be used
	//the depth buffer texture
	GLuint depthTex;
	glGenTextures(1, &depthTex);
	glBindTexture(GL_TEXTURE_2D, depthTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, shadowMapWidth, shadowMapHeight); //create texture of width and height
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //shadow map processing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

	//assign the depth buffer texture to texture channel 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTex);

	//create and set up the FBO
	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

	GLenum drawBuffers[] = { GL_NONE };
	glDrawBuffers(1, drawBuffers);

	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result == GL_FRAMEBUFFER_COMPLETE) {
		printf("framebuffer is complete.\n");
	}
	else {
		printf("framebuffer is incomplete.\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void SceneBasic_Uniform::drawScene() {
	//set car1 properties and position
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TexBody1);

	//prog.setUniform("material.Kd", 0.9f, 0.9f, 0.9f);		//Set material colour (light reflectivity)
	prog.setUniform("material.Ks", 0.95f, 0.95f, 0.95f);	//Set Material Specularity (does it get highlights?)
	//prog.setUniform("material.Ka", 0.1f, 0.1f, 0.1f);		//Set Material Ambience (How much ambint light does it reflect?)
	prog.setUniform("material.Shininess", 100.0f);			//Set material "Shininess" 

	prog.setUniform("HasSnow", 0);
	prog.setUniform("IsCloud", 0);

	model = mat4(1.0f);
	model = glm::scale(model, vec3(2.0f, 2.0f, 2.0f));
	model = glm::translate(model, vec3(0.3f, 0.0f, 0.3f));
	setMatrices();

	carBody->render();

	//car 1 tyres
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, wheelTex);

		//wheel 1
		prog.setUniform("material.Ks", 0.95f, 0.95f, 0.95f);	//Set Material Specularity (does it get highlights?)
		prog.setUniform("material.Shininess", 100.0f);			//Set material "Shininess" 

		model = mat4(1.0f);
		model = glm::scale(model, vec3(2.0f, 2.0f, 2.0f));
		model = glm::translate(model, vec3(0.52f, 0.0f, 0.2f));
		setMatrices();

		wheel11->render();

		//wheel 2
		prog.setUniform("material.Ks", 0.95f, 0.95f, 0.95f);	//Set Material Specularity (does it get highlights?)
		prog.setUniform("material.Shininess", 100.0f);			//Set material "Shininess" 

		model = mat4(1.0f);
		model = glm::scale(model, vec3(2.0f, 2.0f, 2.0f));
		model = glm::translate(model, vec3(0.08f, 0.0f, 0.2f));
		setMatrices();

		wheel12->render();

		//wheel 3
		prog.setUniform("material.Ks", 0.95f, 0.95f, 0.95f);	//Set Material Specularity (does it get highlights?)
		prog.setUniform("material.Shininess", 100.0f);			//Set material "Shininess" 

		model = mat4(1.0f);
		model = glm::scale(model, vec3(2.0f, 2.0f, 2.0f));
		model = glm::translate(model, vec3(0.08f, 0.0f, 0.5f));
		setMatrices();

		wheel13->render();

		//wheel 3
		prog.setUniform("material.Ks", 0.95f, 0.95f, 0.95f);	//Set Material Specularity (does it get highlights?)
		prog.setUniform("material.Shininess", 100.0f);			//Set material "Shininess" 

		model = mat4(1.0f);
		model = glm::scale(model, vec3(2.0f, 2.0f, 2.0f));
		model = glm::translate(model, vec3(0.52f, 0.0f, 0.5f));
		setMatrices();

		wheel14->render();
	}


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TexBody2);

	////set car2 properties and position
	//prog.setUniform("material.Kd", 0.9f, 0.9f, 0.9f);		//Set material colour (light reflectivity)
	prog.setUniform("material.Ks", 0.95f, 0.95f, 0.95f);	//Set Material Specularity (does it get highlights?)
	//prog.setUniform("material.Ka", 0.1f, 0.1f, 0.1f);		//Set Material Ambience (How much ambint light does it reflect?)
	prog.setUniform("material.Shininess", 100.0f);			//Set material "Shininess" 

	model = mat4(1.0f);
	model = glm::scale(model, vec3(2.0f, 2.0f, 2.0f));
	model = glm::rotate(model, glm::radians(-160.0f), vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, vec3(0.3f, 0.0f, 0.2f));
	setMatrices();
	carBody2->render();

	//car 2 tyres
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, wheelTex);

		//wheel 1
		prog.setUniform("material.Ks", 0.95f, 0.95f, 0.95f);	//Set Material Specularity (does it get highlights?)
		prog.setUniform("material.Shininess", 100.0f);			//Set material "Shininess" 

		model = mat4(1.0f);
		model = glm::scale(model, vec3(2.0f, 2.0f, 2.0f));
		model = glm::rotate(model, glm::radians(-160.0f), vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, vec3(0.52f, 0.0f, 0.1f));
		setMatrices();

		wheel21->render();

		//wheel 2
		prog.setUniform("material.Ks", 0.95f, 0.95f, 0.95f);	//Set Material Specularity (does it get highlights?)
		prog.setUniform("material.Shininess", 100.0f);			//Set material "Shininess" 

		model = mat4(1.0f);
		model = glm::scale(model, vec3(2.0f, 2.0f, 2.0f));
		model = glm::rotate(model, glm::radians(-160.0f), vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, vec3(0.08f, 0.0f, 0.1f));
		setMatrices();

		wheel22->render();

		//wheel 3
		prog.setUniform("material.Ks", 0.95f, 0.95f, 0.95f);	//Set Material Specularity (does it get highlights?)
		prog.setUniform("material.Shininess", 100.0f);			//Set material "Shininess" 

		model = mat4(1.0f);
		model = glm::scale(model, vec3(2.0f, 2.0f, 2.0f));
		model = glm::rotate(model, glm::radians(-160.0f), vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, vec3(0.08f, 0.0f, 0.4f));
		setMatrices();

		wheel23->render();

		//wheel 3
		prog.setUniform("material.Ks", 0.95f, 0.95f, 0.95f);	//Set Material Specularity (does it get highlights?)
		prog.setUniform("material.Shininess", 100.0f);			//Set material "Shininess" 

		model = mat4(1.0f);
		model = glm::scale(model, vec3(2.0f, 2.0f, 2.0f));
		model = glm::rotate(model, glm::radians(-160.0f), vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, vec3(0.52f, 0.0f, 0.4f));
		setMatrices();

		wheel24->render();
	}

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bumperTex);
	
	//set obstacles appearance
	//Bumper1
	prog.setUniform("material.Ks", 0.95f, 0.95f, 0.95f);	//Set Material Specularity (does it get highlights?)
	prog.setUniform("material.Shininess", 100.0f);			//Set material "Shininess" 

	model = mat4(1.0f);
	model = glm::scale(model, vec3(0.5f, 0.5f, 0.5f));
	//model = glm::rotate(model, glm::radians(-160.0f), vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, vec3(1.0f, -0.5f, -3.0f));
	setMatrices();

	Bumper1->render();


	//Bumper2
	prog.setUniform("material.Ks", 0.95f, 0.95f, 0.95f);	//Set Material Specularity (does it get highlights?)
	prog.setUniform("material.Shininess", 100.0f);			//Set material "Shininess" 

	model = mat4(1.0f);
	model = glm::scale(model, vec3(0.5f, 0.5f, 0.5f));
	model = glm::rotate(model, glm::radians(-180.0f), vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, vec3(2.0f, -0.5f, -3.0f));
	setMatrices();

	Bumper2->render();

	glBindTexture(GL_TEXTURE_2D, gTex);
	//Ramp1
	prog.setUniform("material.Ks", 0.95f, 0.95f, 0.95f);	//Set Material Specularity (does it get highlights?)
	prog.setUniform("material.Shininess", 100.0f);			//Set material "Shininess" 

	model = mat4(1.0f);
	model = glm::scale(model, vec3(3.0f, 3.0f, 3.0f));
	model = glm::rotate(model, glm::radians(-180.0f), vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, vec3(-1.0f, -0.1f, -1.0f));
	setMatrices();

	Ramp1->render();

	//Ramp2
	prog.setUniform("material.Ks", 0.95f, 0.95f, 0.95f);	//Set Material Specularity (does it get highlights?)
	prog.setUniform("material.Shininess", 100.0f);			//Set material "Shininess" 

	model = mat4(1.0f);
	model = glm::scale(model, vec3(3.0f, 3.0f, 3.0f));
	//model = glm::rotate(model, glm::radians(-180.0f), vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, vec3(-1.0f, -0.1f, -1.0f));
	setMatrices();

	Ramp1->render();

	glBindTexture(GL_TEXTURE_2D, gTex);
	////set ground properties and position
	//prog.setUniform("material.Kd", 0.9f, 0.9f, 0.9f);		//Set material colour (light reflectivity)
	prog.setUniform("material.Ks", 0.2f, 0.2f, 0.2f);	//Set Material Specularity (does it get highlights?)
	//prog.setUniform("material.Ka", 0.1f, 0.1f, 0.1f);		//Set Material Ambience (How much ambint light does it reflect?)
	prog.setUniform("material.Shininess", 20.0f);			//Set material "Shininess" 
	prog.setUniform("HasSnow", 1);


	model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, -0.2f, 0.0f));
	setMatrices();
	ground.render();

	////set hills properties and position
	//prog.setUniform("material.Kd", 0.9f, 0.9f, 0.9f);		//Set material colour (light reflectivity)
	prog.setUniform("material.Ks", 0.2f, 0.2f, 0.2f);	//Set Material Specularity (does it get highlights?)
	//prog.setUniform("material.Ka", 0.1f, 0.1f, 0.1f);		//Set Material Ambience (How much ambint light does it reflect?)
	prog.setUniform("material.Shininess", 20.0f);			//Set material "Shininess" 
	prog.setUniform("HasSnow", 1);

	model = mat4(1.0f);
	model = glm::rotate(model, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	setMatrices();
	hills.render();

	////set clouds properties and position
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, noiseTex);

	prog.setUniform("HasSnow", 0);
	prog.setUniform("IsCloud", 1);

	model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, 8.0f, 0.0f));
	setMatrices();

	clouds.render();
}

void SceneBasic_Uniform::spitOutDepthBuffer() {

}