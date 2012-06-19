#include "Camera.h"
#include <cstdio>

Camera::Camera() {}
Camera::~Camera() {}

const M3DMatrix44f& Camera::getCameraMatrix() {

	cameraStack.GetMatrix(mCamera);
	return mCamera;
}

const M3DMatrix44f& Camera::getCameraRotOnlyMatrix() {

	cameraRotOnlyStack.GetMatrix(mCameraRotOnly);
	return mCameraRotOnly;
}

void Camera::update(GLfloat camXRot, GLfloat camYRot, GLfloat camZPos, GLfloat camYPos, GLfloat camXPos) {

	cameraStack.LoadIdentity();
	cameraStack.Rotate(camXRot, 1.0f, 0.0f, 0.0f);
	cameraStack.Rotate(camYRot, 0.0f, 1.0f, 0.0f);

	cameraRotOnlyStack.LoadIdentity();
	cameraRotOnlyStack.Rotate(camXRot, 1.0f, 0.0f, 0.0f);
	cameraRotOnlyStack.Rotate(camYRot, 0.0f, 1.0f, 0.0f);

	cameraStack.Translate(-camXPos, -camYPos, -camZPos);
}
