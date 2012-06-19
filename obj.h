#include <vecmatquat.h>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>

#include <fstream>

using namespace std;

class objShape {
private:
	bool indTableRebuilded;
	vector3<GLfloat> *facVertices;
	vector3<GLfloat> *facNormals;

	vector3<GLuint> *indVerts;
	vector3<GLuint> *indNorms;
	vector3<GLuint> *indTexts;

	GLfloat *textures2D;
	vector3<GLfloat> *textures3D;

	inline bool checkEOF(ifstream &objFile);
	inline string getAttribVertex(string &vertex);

	vector3<GLfloat> findMaxCoords(void);
	vector3<GLfloat> findMinCoords(void);

	int countAttrFromObjFile (ifstream &objFile);
	bool checkIndices(vector3<GLuint> *indices);
	void compFacesNormals(void);
	void genVerticesTable(void);
	void rebuildIndicesTable(void);
	void scaleVertices(GLfloat scaleFactor);
	void moveVertices(vector3<GLfloat> moveVec);

public:
	bool normIndGood;
	bool texIndGood;

	int vertsPerFace;
	int nAttribs;
	int nFaces;

	int texCordPerVert;

	int nVertices;
	int nNormals;
	int nTextures;
	vector3<GLuint> *faces;
	vector3<GLfloat> *vertices;
	vector3<GLfloat> *normals;
	GLfloat *textures;

	objShape(void);
	int readFromFile(const char *objFileName);
	void scale(void);
	int rebuildAttribTable(char attrib);
	void genFacesNormals(void);
	void genSmoothNormals(void);
	void writeProps(void);
	void writeTables(void);
	void makeTexture2D(void);
	int setTexDimension(int dimension);
};
