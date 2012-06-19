#define FREEGLUT_STATIC

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>

#include <obj.h>

using namespace std;


inline bool objShape::checkEOF(ifstream &objFile)
{
	char c;

	objFile.get(c);
	bool end = objFile.eof();
	objFile.putback(c);

	return end;
}

inline string objShape::getAttribVertex(string &vertex)
{
	string attrib, tmpVertex;

	size_t endPos = vertex.find('/');
	if (!endPos) {
		// Znak "/" jest na pierwszej pozycji (zerowy indeks) - przesuwamy początek
		// łańcucha "vertex" o jeden znak
		tmpVertex = vertex.substr(1, vertex.length() - 1);
		vertex = tmpVertex;
		// Wyszukanie kolejnego znaku "/"
		endPos = vertex.find('/');
	}
	if ((endPos == string::npos) && (vertex.length()))
		// Nie ma znaku "/" - zwrócenie wejściowej wartości "vertex"
		attrib = vertex;
	else {
		// Wartość zwracanego stringu jest do znaku wartością "vertex" do znaku "/"
		attrib = vertex.substr(0, endPos);
		// Przesunięcie począte łańcucha "vertex" za znak "/"
		tmpVertex = vertex.substr(endPos + 1, vertex.length() - endPos);
		vertex = tmpVertex;
	}
	return attrib;
}

objShape::objShape(void) {

	indTableRebuilded = false;
	normIndGood = false;
	texIndGood = false;

	texCordPerVert = 0;
	nFaces = 0;
	nVertices = 0;
	nNormals = 0;
	nTextures = 0;
}

// wyznaczenie najmniejszych współrzędnych puntku i zapisanie ich do wektorowej struktry:
// minVals[0] - min wartość wsp X, itd...
vector3<GLfloat> objShape::findMinCoords(void)
{
	vector3<GLfloat> minVals = vertices[0];

	for (int iVert = 1; iVert < nVertices; iVert++)
		for (int iCoord = 0; iCoord < 3; iCoord++)
			if (vertices[iVert][iCoord] < minVals[iCoord])
				minVals[iCoord] = vertices[iVert][iCoord];

	return minVals;
}

// wyznaczenie największych współrzędnych puntku i zapisanie ich do wektorowej struktry:
vector3<GLfloat> objShape::findMaxCoords(void)
{
	vector3<GLfloat> maxVals = vertices[0];

	for (int iVert = 1; iVert < nVertices; iVert++)
		for (int iCoord = 0; iCoord < 3; iCoord++)
			if (vertices[iVert][iCoord] > maxVals[iCoord])
				maxVals[iCoord] = vertices[iVert][iCoord];

	return maxVals;
}

// pierwsze czytanie pliku z danymi w celu policzenia wierzchołków, normalnych, tekstur, itd...
int objShape::countAttrFromObjFile (ifstream &objFile)
{
	char line[300];
	string type, vertexFormat, buff;
	bool checkTexCoord = true;
	int ln = 0;

	while ( !checkEOF(objFile) ) {
		type.clear();
		objFile >> type;
		if ( !type.compare("v") )
			nVertices++;
		else if ( !type.compare("vn") )
			nNormals++;
		else if ( !type.compare("vt") ) {
			if (checkTexCoord) {
				objFile.get(line, 299); strcat(line, "\n");
				stringstream ssBuff;
				ssBuff.clear();
				ssBuff << line;
				GLfloat tmpCoord;
				do {
					ssBuff >> tmpCoord;
					texCordPerVert++;
				} while( !ssBuff.fail() );
				texCordPerVert--;
				checkTexCoord = false;
			}
			nTextures++;
		}
		else if ( !type.compare("f") )
			nFaces++;
		objFile.getline(line, 299);
		ln ++;
	}

	return 0;
}

// sprawdzenie czy tablica z indeksami (normalnych lub tekstur) jest taka sama jak główna tablica
bool objShape::checkIndices(vector3<GLuint> *indices)
{
	int iInd = 0;
	while ((indices[iInd] == faces[iInd]) && (iInd < nFaces - 1))
		iInd++;

	if (iInd == nFaces - 1)
		return true;
	else
		return false;
}

// odczyt danych z pliku
int objShape::readFromFile(const char *objFileName)
{
	ifstream objFile;
	stringstream ssBuff;
	string type, sVert, sBuff, sVertex;
	int i, iVert = 0, iNorm = 0, iTex = 0, iInd = 0;

	// otwarcie pliku .obj do odczytu
	objFile.open(objFileName);
	if ( objFile.fail() ) {
		cout << "Error opening an .obj file: " << objFileName << endl;
		return 1;
	}
	// policzenie atrybutów obiektu z pliku z .obj
	if ( countAttrFromObjFile (objFile) )
		return 2;
	objFile.close();

	// Przydzielenie pamięci na tablice wartości i indeksów
	vertices = new vector3<GLfloat> [nVertices];
	indVerts = new vector3<GLuint> [nFaces];
	if (nNormals)
		normals = new vector3<GLfloat> [nNormals];
	// indeksy normalnych (czy to wczytanych czy wygenerowanych później będą przytadtne)
	indNorms = new vector3<GLuint> [nFaces];
	// Jak nie ma współrzędnych tekstur, to i nie ma ich indeksów - wsp. tekstur nie można wygenerować
	if (nTextures) {
		textures3D = new vector3<GLfloat> [nTextures];
		indTexts = new vector3<GLuint> [nFaces];
	}

	// właściwe odczytanie danych z pliku
	objFile.open(objFileName);
	while ( !checkEOF(objFile) ) {
		type.clear();
		objFile >> type;
		if ( !type.compare("v") ) {
			for (i = 0; i < 3; i++)
				objFile >> vertices[iVert][i];
			iVert++;
		}
		else if ( !type.compare("vn") ) {
			for (i = 0; i < 3; i++)
				objFile >> normals[iNorm][i];
			iNorm++;
		}
		else if ( !type.compare("vt") ) {
			for (i = 0; i < texCordPerVert; i++)
				objFile >> textures3D[iTex][i];
			iTex++;
		}
		else if ( !type.compare("f") ) {
			for (i = 0; i < 3; i++) {
				objFile >> sVertex;
				ssBuff.clear();
				ssBuff << getAttribVertex(sVertex);
				ssBuff >> indVerts[iInd][i]; indVerts[iInd][i]--;
				if (nTextures) {
					ssBuff.clear();
					ssBuff << getAttribVertex(sVertex);
					ssBuff >> indTexts[iInd][i]; indTexts[iInd][i]--;
				}
				if (nNormals) {
					ssBuff.clear();
					ssBuff << getAttribVertex(sVertex);
					ssBuff >> indNorms[iInd][i]; indNorms[iInd][i]--;
				}
			}
			iInd++;
		}
	}
	objFile.close();

	// definicja liczby atrybutów oraz tablicy indeksów tworzących ścianki
	nAttribs = nVertices;
	faces = indVerts;

	// sprawdzenie poprawności indeksów wektorów normmalnych
	if (nNormals) {
		if (nNormals == nVertices)
			normIndGood = checkIndices(indNorms);
		else
			normIndGood = false;
	}
	// ... oraz teksztur
	if (nTextures) {
		if (nTextures == nVertices)
			texIndGood = checkIndices(indTexts);
		else
			texIndGood = false;
	}
	return 0;
}

// wyznaczenie wektorów normalnych dla ścianek obiektu i unormowanie ich
void objShape::compFacesNormals(void)
{
	vector3<GLuint> vertInd;
	vector3<GLfloat> vec0, vec1;

	cout << "Generating normals for faces..." << endl;

	facNormals = new vector3<GLfloat> [nFaces];

	for (int iFace = 0; iFace < nFaces; iFace++) {
		vertInd.set(indVerts[iFace][0], indVerts[iFace][1], indVerts[iFace][2]);

		vec0 = vertices[vertInd[2]] - vertices[vertInd[0]];
		vec1 = vertices[vertInd[0]] - vertices[vertInd[1]];

		facNormals[iFace] = normalize(cross3(vec0, vec1));
	}
}


void objShape::genFacesNormals(void)
{
	if (nNormals)
		delete [] normals;

	nNormals = 3 * nFaces;
	normals = new vector3<GLfloat> [nNormals];

	compFacesNormals();

	rebuildAttribTable('v');

	int iNorm = 0;
	for (int iFace = 0; iFace < nFaces; iFace++) {
		indNorms[iFace] = faces[iFace];
		for (int iVpF = 0; iVpF < 3; iVpF++)
			normals[iNorm++] = facNormals[iFace];
	}
	// indeksy normalnych są takie same jak indeksy w tablicy faces
	normIndGood = true;
}

// wyznaczenie uśrednionych wektorów normalnych (w danym wierzchołku jest jeden wektor normalny)
void objShape::genSmoothNormals(void)
{
	if (nNormals)
		delete [] normals;

	nNormals = nVertices;
	normals = new vector3<GLfloat> [nNormals];

	// wpierw policzenie normalnych dla ścianek
	compFacesNormals();

	for (int iFace = 0; iFace < nFaces; iFace++) {
		indNorms[iFace] = indVerts[iFace];
		for (int iVpF = 0; iVpF < 3; iVpF++)
			normals[indNorms[iFace][iVpF]] += facNormals[iFace];
	}
	for (int iNorm = 0; iNorm < nNormals; iNorm++)
		normals[iNorm] = normalize(normals[iNorm]);

	delete [] facNormals;

	// indeksy normalnych są takie same jak indeksy w tablicy faces
	normIndGood = true;
}

// pzebudowanie zadanej tablicy z atrybutów obiektu
int objShape::rebuildAttribTable(char attrib)
{
	if ((attrib != 'v') && (!indTableRebuilded))
		rebuildAttribTable('v');

	nAttribs = 3 * nFaces;

	// przydzielenie pamięci na zmienne pomocnicze
	vector3<GLuint> vertInd;
	vector3<GLuint> *indAttribs;
	vector3<GLfloat> *oldAttribsVals;
	vector3<GLfloat> *attribsVals = new vector3<GLfloat> [nAttribs];

	// ustawienie odpowiednich tablic do przebudowania
	switch (attrib) {
		case 'v':
			nVertices = nAttribs;
			indAttribs = indVerts;
			indVerts = faces;
			oldAttribsVals = vertices;
			vertices = attribsVals; break;
		case 'n':
			nNormals = nAttribs;
			indAttribs = indNorms;
			indNorms = faces;
			oldAttribsVals = normals;
			normals = attribsVals;
			normIndGood = true; break;
		case 't':
			nTextures = nAttribs;
			indAttribs = indTexts;
			indTexts = faces;
			oldAttribsVals = textures3D;
			textures3D = attribsVals;
			texIndGood = true; break;
		default:
			return 22;
	}

	// właściwe przebudowanie rablic
	int iAttrib = 0;
	for (int iFace = 0; iFace < nFaces; iFace++) {
		for (int iAtrPerF = 0; iAtrPerF < 3; iAtrPerF++)
			attribsVals[iAttrib++] = oldAttribsVals[indAttribs[iFace][iAtrPerF]];
	}
//	delete [] indAttribs;
	delete [] oldAttribsVals;

	if (!indTableRebuilded)
		rebuildIndicesTable();

	return 0;
}

// przebudowanie głównej tablicy z indeksami wszystkich atrybutów
void objShape::rebuildIndicesTable(void)
{
	nAttribs = 3 * nFaces;
	int iVert = 0;
	for (int iFace = 0; iFace < nFaces; iFace++)
		for (int indPerF = 0; indPerF < 3; indPerF++)
			faces[iFace][indPerF] = iVert++;

	// ustawienie zmiennej informującej o przebudowaniu tablicy indeksów
	indTableRebuilded = true;

	// Sprawdzenie indeksów odpowiednich atrybutów, które mogły się zmienić po przeindeksowaniu wierzchołków
	if (nTextures == nVertices)
		texIndGood = checkIndices(indTexts);
	else
		texIndGood = false;

	if (nNormals == nVertices)
		normIndGood = checkIndices(indNorms);
	else
		normIndGood = false;
}

// przeskalowanie obiektu tak by był wpisany w sześcian o środku symetrii w punkcie 0.0 i długości boku dwa
void objShape::scale(void)
{
	GLfloat scaleFactor;
	vector3<GLfloat> minCoords, maxCoords, moveVec, scaleVec;

	// wyznaczenie wektora przesunięcia
	minCoords = findMinCoords();
	maxCoords = findMaxCoords();
	moveVec = minCoords + (maxCoords - minCoords) / 2.0f;

	// przesunięcie obiektu tak aby był wpisany w sześcian o środku w punkcie 0.0
	moveVertices(moveVec);

	// wyznaczenie wektora skalowania
	scaleVec = maxCoords - minCoords;
	scaleFactor = max(scaleVec.x(), scaleVec.y());
	scaleFactor = max(scaleVec.z(), scaleFactor);
	// przeskalowanie obiektu tak aby mieścił się w sześcianie o boku dwa
	scaleVertices(scaleFactor);
}

// skalowanie wierzchołków obiektu przez zadany współczynnik
void objShape::scaleVertices(GLfloat scaleFactor)
{
	for (int iVert = 0; iVert < nVertices; iVert++)
		vertices[iVert] /= scaleFactor;
}

// przesunięcie obiektu o zadany wektor
void objShape::moveVertices(vector3<GLfloat> moveVec)
{
	for (int iVert = 0; iVert < nVertices; iVert++)
		vertices[iVert] -= moveVec;
}

// przepisanie dwuwymiarowych współrzędnych tekstury z tablicy zawierającej trójwymiarowe wektory
void objShape::makeTexture2D(void)
{
	textures2D = new GLfloat [2 * nAttribs];
	for (int i = 0; i < nAttribs; i++) {
		textures2D[2*i] = textures3D[i][0];
		textures2D[2*i+1] = textures3D[i][1];
	}
}

// określenie liczby wymiarów tekstury - przypisanie adresu odpowiedniej tablicy pod zmienna textures
int objShape::setTexDimension(int dimension)
{
	switch (dimension) {
		case 2: makeTexture2D();
			    textures = textures2D; break;
		case 3: textures = (GLfloat *)textures3D; break;
		default: return 33;
	}
	return 0;
}

// wypisanie własności obiektu
void objShape::writeProps(void)
{
	cout << "Number of vertices: " << nVertices << endl;
	cout << "Number of normals: " << nNormals << endl;
	cout << "Number of textures: " << nTextures << endl;
	cout << "Number of faces: " << nFaces << endl;
	cout << "Number of texture's coordinates: " << texCordPerVert << endl;

	if (nNormals) {
		if (normIndGood)
			cout << "Normals' indices are good" << endl;
		else
			cout << "Normals' indices are bad" << endl;
	}
	if (nTextures) {
		if (texIndGood)
			cout << "Textures' indices are good" << endl;
		else
			cout << "Textures' indices are bad" << endl;
	}
}

// wypisanie wszystkich tablic z atrybutami wierzchołków - używane w celu debugowania
void objShape::writeTables(void)
{
	cout << "========================   vertices   ========================" << endl;
	for (int i = 0; i < nVertices; i++) {
		for (int j = 0; j < 3; j++)
			cout << setw(12) << right << vertices[i][j];
		cout << endl;
	}
	if (nTextures) {
		cout << "========================   textures   ========================" << endl;
		for (int i = 0; i < nTextures; i++) {
			for (int j = 0; j < 3; j++)
				cout << setw(12) << right << textures3D[i][j];
			cout << endl;
		}
	}
	if (nNormals) {
		cout << "========================   normals   ========================" << endl;
		for (int i = 0; i < nNormals; i++) {
			for (int j = 0; j < 3; j++)
				cout << setw(12) << right << normals[i][j];
			cout << endl;
		}
	}

	cout << "========================   vert indices   ======================" << endl;
	for (int i = 0; i < nFaces; i++) {
		for (int j = 0; j < 3; j++)
			cout << setw(6) << right << faces[i][j];
		cout << endl;
	}
	if (nTextures) {
		cout << "=======================   tex indices   ======================" << endl;
		for (int i = 0; i < nFaces; i++) {
			for (int j = 0; j < 3; j++)
				cout << setw(6) << right << indTexts[i][j];
			cout << endl;
		}
	}
	if (nNormals) {
		cout << "======================   norm indices   ======================" << endl;
		for (int i = 0; i < nFaces; i++) {
			for (int j = 0; j < 3; j++)
				cout << setw(6) << right << indNorms[i][j];
			cout << endl;
		}
	}
}
