#include <iostream>
#include "Managers.h"
#include <math3d.h>
#include <StopWatch.h>

double round(double d) {
	return floor(d + 0.5);
}

InputManager::InputManager() : TO_RADS(3.141592654f / 180.0f) {

	currentMouseXPosition = 0;
	currentMouseYPosition = 0;

	camXRot = 0.0f;
	camYRot = 0.0f;
	camZRot = 0.0f;

	camXPos = 0.0f;
	camYPos = 0.0f;
	camZPos = 0.0f;

	camXSpeed = 0.0f;
	camYSpeed = 0.0f;
	camZSpeed = 0.0f;
	movementSpeedFactor = 1.0f;

	jumping = NO_JUMPING;
	textureId = 1;

	keyStates = new bool[256];

	for (int i=0; i<256; i++)
		keyStates[i] = false;
}

InputManager::~InputManager() {}

void InputManager::resized(int width, int height) {
	
	windowWidth = width;
	windowHeight = height;

	midWindowX = windowWidth / 2;
	midWindowY = windowHeight / 2;
}

void InputManager::moveCamera() {

	camYPos += camYSpeed;

	if (blockManager->hasBlock(round(camXPos+camXSpeed), camYPos-1, round(camZPos+camZSpeed)) || blockManager->hasBlock(round(camXPos+camXSpeed), camYPos, round(camZPos+camZSpeed))) {
		return;
	}

	camXPos += camXSpeed;
	camZPos += camZSpeed;

	int t = (int)camYPos-2;

	if (!blockManager->hasBlock(round(camXPos), t, round(camZPos)) && jumping == NO_JUMPING) {
		jumping = FALLING_DOWN;
		flyingIterator = 0;
	}
}

void InputManager::handleMouseMove(int mouseX, int mouseY) {

	GLfloat vertMouseSensitivity = 10.0f;
	GLfloat horizMouseSensitivity = 10.0f;

	int horizMovement = mouseX - midWindowX;
	int vertMovement = mouseY - midWindowY;

	camXRot += vertMovement / vertMouseSensitivity;
	camYRot += horizMovement / horizMouseSensitivity;

	if (camXRot < -90.0f)
		camXRot = -90.0f;

	if (camXRot > 90.0f)
		camXRot = 90.0f;

	if (camYRot < -180.0f) 
		camYRot += 360.0f;

	if (camYRot > 180.0f)
		camYRot -= 360.0f;

	if (currentMouseXPosition != mouseX || currentMouseYPosition != mouseY) {
		currentMouseXPosition = midWindowX;
		currentMouseYPosition = midWindowY;
		glutWarpPointer(midWindowX, midWindowY);
	}
}

void InputManager::handleKeyPress(unsigned char key, int x, int y) {
	keyStates[key] = true;
}

void InputManager::handleKeyUp(unsigned char key, int x, int y) {
	keyStates[key] = false;
}

void InputManager::calculateCameraMovement() {

	static CStopWatch cameraTimer;
	float fTime = cameraTimer.GetElapsedSeconds();
	cameraTimer.Reset(); 

	float linear = fTime * 6.0f;

	float camMovementXComponent = 0.0f;
	float camMovementYComponent = 0.0f;
	float camMovementZComponent = 0.0f;

	if (keyStates['w']) {

		float pitchFactor = cos(toRads(camXRot));
		camMovementXComponent += (movementSpeedFactor * float(sin(toRads(camYRot)))) * pitchFactor;

		float yawFactor = float(cos(toRads(camXRot)));

		camMovementZComponent += (movementSpeedFactor * float(cos(toRads(camYRot))) * -1.0f) * yawFactor;
	}

	if (keyStates['s']) {
		
		float pitchFactor = cos(toRads(camXRot));
		camMovementXComponent += (movementSpeedFactor * float(sin(toRads(camYRot))) * -1.0f) * pitchFactor;
		
		float yawFactor = float(cos(toRads(camXRot)));
		
		camMovementZComponent += (movementSpeedFactor * float(cos(toRads(camYRot)))) * yawFactor;
	}

	if (keyStates['a']) {

		float yRotRad = toRads(camYRot);

		camMovementXComponent += -movementSpeedFactor * float(cos(yRotRad));
		camMovementZComponent += -movementSpeedFactor * float(sin(yRotRad));
	}

	if (keyStates['d']) {

		float yRotRad = toRads(camYRot);

		camMovementXComponent += movementSpeedFactor * float(cos(yRotRad));
		camMovementZComponent += movementSpeedFactor * float(sin(yRotRad));
	}

	if (keyStates[' '] && jumping == NO_JUMPING) {
		jumping = JUMPING;
		maxJump = camYPos + 1.5f;
		flyingIterator = 0;
	}

	if (jumping == JUMPING) {

		camMovementYComponent += (2.0f/++flyingIterator);

		if (camYPos >= maxJump) {
			camMovementYComponent = 0.0f;
			jumping = FALLING_DOWN;
			flyingIterator = 0;
		}
	} else if (jumping == FALLING_DOWN) {

		camMovementYComponent -= (0.7f*flyingIterator++);

		for (int t=(int)camYPos-2;t>(int)camYPos-2+camMovementYComponent;t--) {

			if (blockManager->hasBlock(round(camXPos), t, round(camZPos))) {
				camMovementYComponent = 0.0f;
				camYPos = t+2;
				jumping = NO_JUMPING;
				break;
			}
		}

		if (camYPos < -1000.0f)
			camYPos = 1000.0f;
	}

	camXSpeed = camMovementXComponent;
	camYSpeed = camMovementYComponent;
	camZSpeed = camMovementZComponent;

	if (camXSpeed > movementSpeedFactor) 
		camXSpeed = movementSpeedFactor;

	if (camXSpeed < -movementSpeedFactor)
		camXSpeed = -movementSpeedFactor;

	if (camZSpeed > movementSpeedFactor)
		camZSpeed = movementSpeedFactor;

	if (camZSpeed < -movementSpeedFactor)
		camZSpeed = -movementSpeedFactor;

	camXSpeed*=linear;
	camYSpeed*=linear;
	camZSpeed*=linear;
}

void InputManager::mouseLeftClick() {

	int x, y, z;

	if (findCorrectBlock(x, y, z, true)) 
		blockManager->setBlock(x, y, z, textureId);
}

void InputManager::mouseRightClick() {

	int x, y, z;

	if (findCorrectBlock(x, y, z, false)) 
		blockManager->setBlock(x, y, z, 0);
}

void InputManager::mouseWheelUpScroll() {
	
	if (++textureId >= TEXTURES_NUM)
		textureId = 1;

	std::cout << "Using texture: " << textureFaces[textureId][0] << "\n";
}

void InputManager::mouseWheelDownScroll() {

	if (--textureId <= 0)
		textureId = TEXTURES_NUM-1;

	std::cout << "Using texture: " << textureFaces[textureId][0] << "\n";
}

bool InputManager::findCorrectBlock(int &x, int &y, int &z, bool before) {

	float camMovementXComponent = 0.0f;
	float camMovementYComponent = 0.0f;
	float camMovementZComponent = 0.0f;
	float pitchFactor = cos(toRads(camXRot));
	float yawFactor = float(cos(toRads(camXRot)));

	bool notFound = true, found = false;

	int beforeX = round(camXPos);
	int beforeY = round(camYPos);
	int beforeZ = round(camZPos);

	do {

		camMovementXComponent += (movementSpeedFactor * float(sin(toRads(camYRot)))) * pitchFactor;
		camMovementYComponent += movementSpeedFactor * float(sin(toRads(camXRot))) * -1.0f;
		camMovementZComponent += (movementSpeedFactor * float(cos(toRads(camYRot))) * -1.0f) * yawFactor;

		if (blockManager->hasBlock(round(camXPos+camMovementXComponent), round(camYPos+camMovementYComponent), round(camZPos+camMovementZComponent))) {
			found = true;
		} else {
			beforeX = round(camXPos+camMovementXComponent);
			beforeY = round(camYPos+camMovementYComponent);
			beforeZ = round(camZPos+camMovementZComponent);
		}

		if (camMovementXComponent > 8 || camMovementYComponent > 8 || camMovementZComponent > 8 || camMovementXComponent < -8 || camMovementYComponent < -8 || camMovementZComponent < -8)
			notFound = false;
	} while (!found && notFound);

	if (found && before) {
		x = beforeX;
		y = beforeY;
		z = beforeZ;
	} else if (found && !before) {
		x = round(camXPos+camMovementXComponent);
		y = round(camYPos+camMovementYComponent);
		z = round(camZPos+camMovementZComponent);
	}

	return found;
}
