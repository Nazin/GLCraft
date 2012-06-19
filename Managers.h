#define FREEGLUT_STATIC

#ifndef MANAGERS_H_
#define MANAGERS_H_

#include <iostream>

#include <GLTools.h>
#include <GLShaderManager.h>
#include <GLFrustum.h>
#include <GLBatch.h>
#include <GLFrame.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#include "Camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "obj.h"

#ifdef __APPLE__
	#include <glut/glut.h>
#else
	#define FREEGLUT_STATIC
	#include <GL/glut.h>
#endif

class InputManager;

#define SIZE_X 80
#define SIZE_Z 80
#define SIZE_Y 20

#define NO_JUMPING 1
#define JUMPING 2
#define FALLING_DOWN 3

#define TEXTURES_NUM 8

typedef struct {

	GLFrame frame;
	int textureId;
	GLMatrixStack modelMatrix;
} cube;

extern const char *textureFaces[TEXTURES_NUM][7];

class BlockManager {

	cube cubes[SIZE_X][SIZE_Z][SIZE_Y];
public:
	
	BlockManager();
	virtual ~BlockManager();

	void loadBlocks(InputManager *inputManager);
	void saveBlocks(InputManager *inputManager);
	bool hasBlock(int x, int y, int z);
	cube* getBlock(int x, int y, int z);
	void setBlock(int x, int y, int z, int textureId);
};

class InputManager {

	const float TO_RADS;

	GLint windowWidth;
	GLint windowHeight;

	GLint midWindowX;
	GLint midWindowY;

	GLint currentMouseXPosition;
	GLint currentMouseYPosition;

	GLfloat camXRot;
	GLfloat camYRot;
	GLfloat camZRot;

	GLfloat camXPos;
	GLfloat camYPos;
	GLfloat camZPos;

	GLfloat camXSpeed;
	GLfloat camYSpeed;
	GLfloat camZSpeed;
	GLfloat movementSpeedFactor;
	GLfloat maxJump;

	int flyingIterator;
	int textureId;

	BlockManager* blockManager;

	int jumping;

	bool* keyStates;

	float toRads(const float &theAngleInDegrees) {
		return theAngleInDegrees * TO_RADS;
	}

	bool findCorrectBlock(int &x, int &y, int &z, bool before);
public:

	InputManager();
	virtual ~InputManager();

	void handleKeyPress(unsigned char key, int x, int y);
	void handleKeyUp(unsigned char key, int x, int y);
	void handleMouseMove(int mouseX, int mouseY);
	void moveCamera();
	void mouseLeftClick();
	void mouseRightClick();
	void mouseWheelUpScroll();
	void mouseWheelDownScroll();
	void resized(int width, int height);
	void calculateCameraMovement();

	GLfloat getCamXPos()  {
		return camXPos;
	}

	GLfloat getCamXRot()  {
		return camXRot;
	}

	GLfloat getCamYPos()  {
		return camYPos;
	}

	GLfloat getCamYRot()  {
		return (camYRot);
	}

	GLfloat getCamZPos()  {
		return camZPos;
	}

	GLfloat getCamZRot()  {
		return camZRot;
	}

	void setCamXPos(GLfloat x) {
		camXPos = x;
	}

	void setCamYPos(GLfloat y) {
		camYPos = y;
	}

	void setCamZPos(GLfloat z) {
		camZPos = z;
	}

	void setBlockManager(BlockManager *manager) {
		blockManager = manager;
	}
};

#define DRAW_PHONG 1
#define DRAW_SHADES 2
#define DRAW_DREAM 3

class ShaderManager {

	GLShaderManager shaderManager;

	GLuint skyboxShader;
	GLuint phongShader;
	GLuint depthShader;
	GLuint shadowShader;
	GLuint dreamShader;
	GLuint objShader;

	GLint locSkyboxMVP;

	GLint locPhongLight;
	GLint locPhongMVP;
	GLint locPhongMV;
	GLint locPhongNM;

	GLint locDepthMVP;
	GLint locDepthTexture;

	GLint locShadowMVP;
	GLint locShadowV;
	GLint locShadowMV;
	GLint locShadowDepthBias;
	GLint locShadowMap;
	GLint locShadowLightInv;

	GLint locDreamMVP;

	GLint locObjMVP;
	GLint locObjMV;

	GLBatch skyboxCubeBatch;
	GLBatch cubeBatch;

	GLuint FramebufferName;
	GLuint depthTexture;

	GLuint verticesLocation;
	GLuint normalsLocation;

	GLenum DrawBuffers[2];

	objShape objs[100];
	GLuint objsVertexArray[100];
	GLFrame objsPosition[100];
	bool objsRotate[100];

	int drawMode, objsCounter;

	BlockManager* blockManager;

	void initShadowShader();
	void initPhongShader();
	void initDreamShader();
	void initSkyboxShader();
	void initObjShader();

	void drawWithShades(Camera *cameraFrame, GLGeometryTransform *transformPipeline, GLuint *uiTextures);
	void drawWithoutShades(GLMatrixStack *modelViewMatrix, Camera *cameraFrame, GLGeometryTransform *transformPipeline, GLuint *uiTextures);

	void setupObjIn(char *objFileName, objShape *obj, GLuint *objVertexArray);
public:

	ShaderManager();
	virtual ~ShaderManager();

	void initShaders();
	void draw(GLMatrixStack *modelViewMatrix, Camera *cameraFrame, GLGeometryTransform *transformPipeline, GLuint *uiTextures);
	void drawSkybox(GLuint textureId, GLGeometryTransform *transformPipeline);
	void drawObj(GLMatrixStack *modelViewMatrix, GLGeometryTransform *transformPipeline);

	void setupObj(char *objFileName, GLfloat x, GLfloat y, GLfloat z, bool rotate);

	void updateDrawMode() {

		drawMode++;

		if (drawMode > 3)
			drawMode = 1;
	}

	void setBlockManager(BlockManager *manager) {
		blockManager = manager;
	}

	GLShaderManager *getGLShaderMamager() {
		return &shaderManager;
	}
};

#endif
