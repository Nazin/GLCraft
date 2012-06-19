#include "Managers.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

vector<string> &split(const string &s, char delim, vector<string> &elems) {

	stringstream ss(s);
	string item;
	while (getline(ss, item, delim))
		elems.push_back(item);

	return elems;
}

vector<string> split(const string &s, char delim) {
	vector<string> elems;
	return split(s, delim, elems);
}

const char *textureFaces[TEXTURES_NUM][7] = {
	{"skybox", "media/xpos.tga", "media/xneg.tga", "media/ypos.tga", "media/yneg.tga", "media/zpos.tga", "media/zneg.tga"},
	{"grass", "media/grassSide.tga", "media/grassSide.tga", "media/grass.tga", "media/dirt.tga", "media/grassSide.tga", "media/grassSide.tga"},
	{"dirt", "media/dirt.tga", "media/dirt.tga", "media/dirt.tga", "media/dirt.tga", "media/dirt.tga", "media/dirt.tga"},
	{"stone", "media/stone.tga", "media/stone.tga", "media/stone.tga", "media/stone.tga", "media/stone.tga", "media/stone.tga"},
	{"plank", "media/plank.tga", "media/plank.tga", "media/plank.tga", "media/plank.tga", "media/plank.tga", "media/plank.tga"},
	{"wood", "media/woodSide.tga", "media/woodSide.tga", "media/wood.tga", "media/wood.tga", "media/woodSide.tga", "media/woodSide.tga"},
	{"leaves", "media/leaves.tga", "media/leaves.tga", "media/leaves.tga", "media/leaves.tga", "media/leaves.tga", "media/leaves.tga"},
	{"sand", "media/sand.tga", "media/sand.tga", "media/sand.tga", "media/sand.tga", "media/sand.tga", "media/sand.tga"}
};

BlockManager::BlockManager() {}
BlockManager::~BlockManager() {}

void BlockManager::loadBlocks(InputManager *inputManager) {

	string line;
	vector<string> elems;

	fstream save;
	save.open("save.dat");

	float camX, camY, camZ;

	if (save.is_open()) {

		getline(save, line);
		elems = split(line, ' ');

		int xMax, yMax, zMax;
		
		xMax = atoi(elems[0].c_str());
		yMax = atoi(elems[1].c_str());
		zMax = atoi(elems[2].c_str());

		getline(save, line);
		elems = split(line, ' ');

		camX = atof(elems[0].c_str());
		camY = atof(elems[1].c_str());
		camZ = atof(elems[2].c_str());

		for (int y = 0; y < yMax; y++)
			for (int x = 0; x < xMax; x++) {

				getline(save, line);
				elems = split(line, ' ');

				for (int z = 0; z < zMax; z++) {
					cubes[x][z][y].frame.SetOrigin(x, y, z);
					cubes[x][z][y].textureId = atoi(elems[z].c_str());
					cubes[x][z][y].modelMatrix.LoadIdentity();
					cubes[x][z][y].modelMatrix.MultMatrix(cubes[x][z][y].frame);
				}
			}
	} else {
		cout << "Unable to load\n";
		exit(0);
	}

	inputManager->setCamXPos(camX);
	inputManager->setCamYPos(camY);
	inputManager->setCamZPos(camZ);
}

void BlockManager::saveBlocks(InputManager *inputManager) {

	fstream save;
	save.open("save.dat");

	if (save.is_open()) {

		save << SIZE_X << " " << SIZE_Y << " " << SIZE_Z << endl;
		save << inputManager->getCamXPos() << " " << inputManager->getCamYPos() << " " << inputManager->getCamZPos() << endl;

		for (int y = 0; y < SIZE_Y; y++)
			for (int x = 0; x < SIZE_X; x++) {

				for (int z = 0; z < SIZE_Z; z++) 
					save << cubes[x][z][y].textureId << " ";
			
				save << endl;
			}

		save.close();
	} else
		cout << "Unable to save\n";
}

bool BlockManager::hasBlock(int x, int y, int z) {
	return !(x >= SIZE_X || x < 0 || y >= SIZE_Y || y < 0 || z >= SIZE_Z || z < 0 || cubes[x][z][y].textureId == 0);
}

cube* BlockManager::getBlock(int x, int y, int z) {
	return &cubes[x][z][y];
}

void BlockManager::setBlock(int x, int y, int z, int textureId) {
	cubes[x][z][y].textureId = textureId;
}
