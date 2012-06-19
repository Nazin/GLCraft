#include <iostream>

#include <GLTools.h>
#include <GLShaderManager.h>
#include <GLFrustum.h>
#include <GLBatch.h>
#include <GLFrame.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#include <math.h>
#include <stdio.h>

#include "Camera.h"
#include "Managers.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

Camera cameraFrame;
InputManager inputManager;
BlockManager blockManager;
ShaderManager shaderManager;

GLFrustum viewFrustum;
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLGeometryTransform transformPipeline;

GLuint uiTextures[TEXTURES_NUM];
GLenum cubePositions[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode) {

	GLbyte *pBits;
	int nWidth, nHeight, nComponents;
	GLenum eFormat;

	pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
	if (pBits == NULL) 
		return false;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, nWidth, nHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBits);
	
	free(pBits);

	if (minFilter == GL_LINEAR_MIPMAP_LINEAR || minFilter == GL_LINEAR_MIPMAP_NEAREST || minFilter == GL_NEAREST_MIPMAP_LINEAR || minFilter == GL_NEAREST_MIPMAP_NEAREST) 
		glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}

void getCubeTextures(int id, const char **faces) {

	GLbyte *pBytes;
	GLint iWidth, iHeight, iComponents;
	GLenum eFormat;
	int i;

	glBindTexture(GL_TEXTURE_CUBE_MAP, uiTextures[id]);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (i = 1; i < 7; i++) {
		pBytes = gltReadTGABits(faces[i], &iWidth, &iHeight, &iComponents, &eFormat);
		glTexImage2D(cubePositions[i-1], 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
		free(pBytes);
	}

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void SetupRC(void) {

	glClearColor(0.53f, 0.81f, 0.98f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	
	shaderManager.initShaders();

	glGenTextures(TEXTURES_NUM, uiTextures);

	for (int i=0; i<TEXTURES_NUM; i++)
		getCubeTextures(i, textureFaces[i]);

	blockManager.loadBlocks(&inputManager);
	inputManager.setBlockManager(&blockManager);
	shaderManager.setBlockManager(&blockManager);

	shaderManager.setupObj("obj/sword.obj", 5.0f, 1.0f, 1.0f, true);
	shaderManager.setupObj("obj/ivy.obj", 1.0f, 1.0f, 1.0f, true);
	shaderManager.setupObj("obj/ball.obj", 1.0f, 5.0f, 1.0f, true);
	shaderManager.setupObj("obj/elf.obj", 4.0f, 1.5f, 4.0f, false);
}

void ShutdownRC(void) {
	glDeleteTextures(TEXTURES_NUM, uiTextures);
}

void RenderScene(void) {

	inputManager.moveCamera(); 
	inputManager.calculateCameraMovement(); 
	cameraFrame.update(inputManager.getCamXRot(), inputManager.getCamYRot(), inputManager.getCamZPos(), inputManager.getCamYPos(), inputManager.getCamXPos());

	modelViewMatrix.PushMatrix();
	modelViewMatrix.MultMatrix(cameraFrame.getCameraMatrix());

	shaderManager.draw(&modelViewMatrix, &cameraFrame, &transformPipeline, uiTextures);

	shaderManager.drawObj(&modelViewMatrix, &transformPipeline);

	modelViewMatrix.PopMatrix();

	modelViewMatrix.PushMatrix();
	modelViewMatrix.MultMatrix(cameraFrame.getCameraRotOnlyMatrix());

	shaderManager.drawSkybox(uiTextures[0], &transformPipeline);
	
	modelViewMatrix.PopMatrix();

	glutSwapBuffers();
	glutPostRedisplay();
}

void ChangeSize(int w, int h) {

	if (h == 0)
		h = 1;

	inputManager.resized(w, h);

	glViewport(0, 0, w, h);

	viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 100.0f);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

void KeyUp(unsigned char key, int x, int y) {
	inputManager.handleKeyUp(key, x, y);
}

void KeyPress(unsigned char key, int x, int y) {

	inputManager.handleKeyPress(key, x, y);

	if (key == 27) {
		blockManager.saveBlocks(&inputManager);
		exit(0);
	} else if (key == 9) 
		shaderManager.updateDrawMode();
}

void PassiveMotion(int x, int y) {
	inputManager.handleMouseMove(x, y);
}

void MouseClick(int button, int state, int x, int) {
	
	if (state == GLUT_DOWN) {

		if (button == GLUT_LEFT_BUTTON)
			inputManager.mouseLeftClick();
		else if (button == GLUT_RIGHT_BUTTON)
			inputManager.mouseRightClick();
		else if (button == 3) 
			inputManager.mouseWheelUpScroll();
		else if (button == 4) 
			inputManager.mouseWheelDownScroll();
	}
}

int main(int argc, char* argv[]) {

	gltSetWorkingDirectory(argv[0]);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(800, 600);

	glutCreateWindow("World - Project");

	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
	glutKeyboardFunc(KeyPress);
	glutKeyboardUpFunc(KeyUp);
	glutPassiveMotionFunc(PassiveMotion);
	glutMotionFunc(PassiveMotion);
	glutMouseFunc(MouseClick);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}
	
	glutSetCursor(GLUT_CURSOR_NONE);
	SetupRC();	
	glutMainLoop();
	ShutdownRC();

	return 0;
}
