#ifndef CAMERA_H_
#define CAMERA_H_

#include <GLShaderManager.h>
#include <GLFrustum.h>
#include <GLBatch.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include <StopWatch.h>

class Camera {

	GLMatrixStack cameraStack;
	GLMatrixStack cameraRotOnlyStack;
	M3DMatrix44f mCamera;
	M3DMatrix44f mCameraRotOnly;

public:

	Camera();
	virtual ~Camera();

	const M3DMatrix44f& getCameraMatrix();
	const M3DMatrix44f& getCameraRotOnlyMatrix();

	void update(GLfloat camXRot, GLfloat camYRot, GLfloat camZPos, GLfloat camYPos, GLfloat camXPos);
};

#endif
