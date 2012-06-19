#include "Managers.h"

ShaderManager::ShaderManager() {}
ShaderManager::~ShaderManager() {
	
	glDeleteProgram(shadowShader);
	glDeleteProgram(skyboxShader);
	glDeleteProgram(depthShader);
	glDeleteProgram(phongShader);
	glDeleteProgram(dreamShader);

	glDeleteTextures(1, &depthTexture);
	glDeleteFramebuffers(1, &FramebufferName);
}

void ShaderManager::initShadowShader() {

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	depthShader = shaderManager.LoadShaderPairWithAttributes("shaders/depth_vs.glsl", "shaders/depth_fs.glsl", 1, GLT_ATTRIBUTE_VERTEX, "vertexPosition_modelspace");

	locDepthMVP = glGetUniformLocation(depthShader, "depthMVP");
	locDepthTexture = glGetUniformLocation(depthShader, "myTextureSampler");

	FramebufferName = 0;
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

	DrawBuffers[1] = GL_DEPTH_ATTACHMENT;
	glDrawBuffers(1, DrawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("\n\nif(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){\n\n");
		exit(-1);
	}
	
	shadowShader = shaderManager.LoadShaderPairWithAttributes("shaders/shadow_vs.glsl", "shaders/shadow_fs.glsl", 3, GLT_ATTRIBUTE_VERTEX, "vertexPosition_modelspace", GLT_ATTRIBUTE_TEXTURE0, "vertexUV", GLT_ATTRIBUTE_NORMAL, "vertexNormal_modelspace");

	locShadowMVP = glGetUniformLocation(shadowShader, "MVP");
	locShadowV = glGetUniformLocation(shadowShader, "V");
	locShadowMV = glGetUniformLocation(shadowShader, "M");
	locShadowDepthBias = glGetUniformLocation(shadowShader, "DepthBiasMVP");
	locShadowMap = glGetUniformLocation(shadowShader, "shadowMap");
	locShadowLightInv = glGetUniformLocation(shadowShader, "LightInvDirection_worldspace");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShaderManager::initPhongShader() {

	phongShader = shaderManager.LoadShaderPairWithAttributes("shaders/phong_vs.glsl", "shaders/phong_fs.glsl", 2, GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal");

	locPhongLight = glGetUniformLocation(phongShader, "vLightPosition");
	locPhongMVP = glGetUniformLocation(phongShader, "mvpMatrix");
	locPhongMV  = glGetUniformLocation(phongShader, "mvMatrix");
	locPhongNM  = glGetUniformLocation(phongShader, "normalMatrix");
}

void ShaderManager::initDreamShader() {

	dreamShader = shaderManager.LoadShaderPairWithAttributes("shaders/dream_vs.glsl", "shaders/dream_fs.glsl", 2, GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal");

	locDreamMVP = glGetUniformLocation(dreamShader, "mvpMatrix");
}

void ShaderManager::initSkyboxShader() {

	skyboxShader = shaderManager.LoadShaderPairWithAttributes("shaders/skybox_vs.glsl", "shaders/skybox_fs.glsl", 2, GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal");
	locSkyboxMVP = glGetUniformLocation(skyboxShader, "mvpMatrix");

	gltMakeCube(skyboxCubeBatch, 50.0f);
}

void ShaderManager::initObjShader() {

	objShader = shaderManager.LoadShaderPairWithAttributes("shaders/obj_vs.glsl", "shaders/obj_fs.glsl", 2, GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal");

	verticesLocation = glGetAttribLocation(objShader, "inVertex");
	normalsLocation = glGetAttribLocation(objShader, "inNormal");

	locObjMVP = glGetUniformLocation(objShader, "modelViewProjectionMatrix");
	locObjMV = glGetUniformLocation(objShader, "modelViewMatrix");
}

void ShaderManager::initShaders() {

	shaderManager.InitializeStockShaders();

	initSkyboxShader();
	initPhongShader();
	initDreamShader();
	initShadowShader();
	initObjShader();

	gltMakeCube(cubeBatch, 0.5f);
	drawMode = DRAW_PHONG;
	objsCounter = 0;
}

void ShaderManager::setupObj(char *objFileName, GLfloat x, GLfloat y, GLfloat z, bool rotate) {

	setupObjIn(objFileName, &objs[objsCounter], &objsVertexArray[objsCounter]);
	objsPosition[objsCounter].SetOrigin(x, y, z);
	objsRotate[objsCounter] = rotate;

	objsCounter++;
}

void ShaderManager::setupObjIn(char *objFileName, objShape *obj, GLuint *objVertexArray) {

	GLuint verticesBuffer, normalsBuffer, indicesBuffer;
	int objError;

	if ((objError = obj->readFromFile(objFileName))) {
		std::cout << "Nie udalo sie zaladowac obj\n";
		return;
	}
		
	obj->writeProps();
	obj->scale();

	if (!obj->nNormals)
		obj->genFacesNormals();
	else
		if (!obj->normIndGood)
			obj->rebuildAttribTable('n');

	obj->writeProps();

	glGenVertexArrays(1, objVertexArray);
	glBindVertexArray(objVertexArray[0]);

	glGenBuffers(1, &verticesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
	glBufferData(GL_ARRAY_BUFFER, 3*obj->nAttribs*sizeof(GLfloat), obj->vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(verticesLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glGenBuffers(1, &normalsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	glBufferData(GL_ARRAY_BUFFER, 3*obj->nAttribs*sizeof(GLfloat), obj->normals, GL_STATIC_DRAW);
	glVertexAttribPointer(normalsLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glGenBuffers(1, &indicesBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*obj->nFaces*sizeof(GLuint), obj->faces, GL_STATIC_DRAW);

	glEnableVertexAttribArray(verticesLocation);
	glEnableVertexAttribArray(normalsLocation);

	glBindVertexArray(0);
}

void ShaderManager::drawObj(GLMatrixStack *modelViewMatrix, GLGeometryTransform *transformPipeline) {

	for (int i = 0; i<objsCounter; i++) {

		modelViewMatrix->PushMatrix();
		modelViewMatrix->MultMatrix(objsPosition[i]);

		if (objsRotate[i]) {
			modelViewMatrix->Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
			modelViewMatrix->Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
		}

		modelViewMatrix->Scale(2.0f, 2.0f, 2.0f);
		modelViewMatrix->Translate(0.0f, 0.0f, 0.25f);

		glUseProgram(objShader);

		glUniformMatrix4fv(locObjMVP, 1, GL_FALSE, transformPipeline->GetModelViewProjectionMatrix());
		glUniformMatrix4fv(locObjMV, 1, GL_FALSE, transformPipeline->GetModelViewMatrix());

		glBindVertexArray(objsVertexArray[i]);
	
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, 3*objs[i].nFaces, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glUseProgram(0);

		modelViewMatrix->PopMatrix();
	}
}

void ShaderManager::drawWithShades(Camera *cameraFrame, GLGeometryTransform *transformPipeline, GLuint *uiTextures) {

	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);
	glViewport(0, 0, 1024, 1024);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(depthShader);

	glm::vec3 lightInvDir = glm::vec3(0.5f, 2, 2);

	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-200, 200, -200, 200, -200, 400);
	glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	
	cube* tempCube;

	for (int x = 0; x < SIZE_X; x++) 
		for (int z = 0; z < SIZE_Z; z++) 
			for (int y = 0; y < SIZE_Y; y++) {
				
				if (!blockManager->hasBlock(x, y, z))
					continue;

				tempCube = blockManager->getBlock(x, y, z);

				glm::mat4 depthModelMatrix = glm::make_mat4(tempCube->modelMatrix.GetMatrix());
				glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

				glUniformMatrix4fv(locDepthMVP, 1, GL_FALSE, &depthMVP[0][0]);
				cubeBatch.Draw();
			}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shadowShader);

	glm::mat4 biasMatrix(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0);

	glUniform3f(locShadowLightInv, lightInvDir.x, lightInvDir.y, lightInvDir.z);

	glUniform1i(locDepthTexture, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glUniform1i(locShadowMap, 1);
	
	int bindedTexture = 0;

	for (int x = 0; x < SIZE_X; x++) 
		for (int z = 0; z < SIZE_Z; z++) 
			for (int y = 0; y < SIZE_Y; y++) {
				
				if (!blockManager->hasBlock(x, y, z))
					continue;

				tempCube = blockManager->getBlock(x, y, z);

				glm::mat4 depthModelMatrix = glm::make_mat4(tempCube->modelMatrix.GetMatrix());
				glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
				glm::mat4 depthBiasMVP = biasMatrix * depthMVP;

				glm::mat4 m = glm::make_mat4(tempCube->modelMatrix.GetMatrix());
				glm::mat4 v = glm::make_mat4(cameraFrame->getCameraMatrix());
				glm::mat4 p = glm::make_mat4(transformPipeline->GetProjectionMatrix());

				glm::mat4 mvp = p*v*m;

				glUniformMatrix4fv(locShadowMVP, 1, GL_FALSE, &mvp[0][0]);
				glUniformMatrix4fv(locShadowMV, 1, GL_FALSE, tempCube->modelMatrix.GetMatrix());
				glUniformMatrix4fv(locShadowV, 1, GL_FALSE, cameraFrame->getCameraMatrix());
				glUniformMatrix4fv(locShadowDepthBias, 1, GL_FALSE, &depthBiasMVP[0][0]);

				if (bindedTexture != tempCube->textureId) {
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_CUBE_MAP, uiTextures[tempCube->textureId]);
					bindedTexture = tempCube->textureId;
				}

				cubeBatch.Draw();
			}
}

void ShaderManager::drawWithoutShades(GLMatrixStack *modelViewMatrix, Camera *cameraFrame, GLGeometryTransform *transformPipeline, GLuint *uiTextures) {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cube* tempCube;
	int bindedTexture = 0;

	GLfloat vEyeLightBase[] = { -100.0f, 100.0f, 100.0f };
	GLfloat vEyeLight[] = { -100.0f, 100.0f, 100.0f };

	for (int x = 0; x < SIZE_X; x++) 
		for (int z = 0; z < SIZE_Z; z++) 
			for (int y = 0; y < SIZE_Y; y++) {
				
				if (!blockManager->hasBlock(x, y, z))
					continue;

				tempCube = blockManager->getBlock(x, y, z);
				
				modelViewMatrix->PushMatrix();
				modelViewMatrix->MultMatrix(tempCube->frame);

				if (bindedTexture != tempCube->textureId) {
					glBindTexture(GL_TEXTURE_CUBE_MAP, uiTextures[tempCube->textureId]);
					bindedTexture = tempCube->textureId;
				}

				m3dTransformVector4(vEyeLight, vEyeLightBase, cameraFrame->getCameraRotOnlyMatrix());

				if (drawMode == DRAW_PHONG) {

					glUseProgram(phongShader);
					glUniform3fv(locPhongLight, 1, vEyeLight);
					glUniformMatrix4fv(locPhongMVP, 1, GL_FALSE, transformPipeline->GetModelViewProjectionMatrix());
					glUniformMatrix4fv(locPhongMV, 1, GL_FALSE, transformPipeline->GetModelViewMatrix());
					glUniformMatrix3fv(locPhongNM, 1, GL_FALSE, transformPipeline->GetNormalMatrix());
				} else {

					glUseProgram(dreamShader);
					glUniformMatrix4fv(locDreamMVP, 1, GL_FALSE, transformPipeline->GetModelViewProjectionMatrix());
				}
				
				cubeBatch.Draw();
				modelViewMatrix->PopMatrix();
			}
}

void ShaderManager::draw(GLMatrixStack *modelViewMatrix, Camera *cameraFrame, GLGeometryTransform *transformPipeline, GLuint *uiTextures) {

	if (drawMode == DRAW_SHADES)
		drawWithShades(cameraFrame, transformPipeline, uiTextures);
	else
		drawWithoutShades(modelViewMatrix, cameraFrame, transformPipeline, uiTextures);
}

void ShaderManager::drawSkybox(GLuint textureId, GLGeometryTransform *transformPipeline) {

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
	glUseProgram(skyboxShader);
	glUniformMatrix4fv(locSkyboxMVP, 1, GL_FALSE, transformPipeline->GetModelViewProjectionMatrix());

	skyboxCubeBatch.Draw();
}
