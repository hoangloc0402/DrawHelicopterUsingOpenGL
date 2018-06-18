﻿#include "Mesh.h"
#include <math.h>

#define PI			3.1415926
#define DEG2RAD (3.14159f/180.0f)


void Mesh::Draw() {
	for (int f = 0; f < numFaces; f++) {
		glBegin(GL_POLYGON);
		for (int v = 0; v < face[f].nVerts; v++) {
			int iv = face[f].vert[v].vertIndex;
			glNormal3f(face[f].facenorm.x, face[f].facenorm.y, face[f].facenorm.z);
			glVertex3f(pt[iv].x, pt[iv].y, pt[iv].z);
		}
		glEnd();
	}
}

void Mesh::CalculateFacesNorm() {
	float mx, my, mz;
	int numVertOnFace;
	Point3 curPoint, nextPoint;
	for (int faceCount = 0; faceCount < this->numFaces; faceCount++) {
		mx = my = mz = 0.0;
		numVertOnFace = face[faceCount].nVerts;
		for (int i = 0; i < numVertOnFace; i++) {
			curPoint = pt[face[faceCount].vert[i].vertIndex];
			nextPoint = pt[face[faceCount].vert[(i + 1) % numVertOnFace].vertIndex];
			mx += (curPoint.y - nextPoint.y) *(curPoint.z + nextPoint.z);
			my += (curPoint.z - nextPoint.z) *(curPoint.x + nextPoint.x);
			mz += (curPoint.x - nextPoint.x) *(curPoint.y + nextPoint.y);
		}
		float size = sqrt(mx * mx + my * my + mz * mz);
		mx /= size;
		my /= size;
		mz /= size;
		face[faceCount].facenorm.set(mx, my, mz);
	}

}

void Mesh::CreateHeliFace(int numVertexEachEdge) {

	numVerts = numVertexEachEdge * numVertexEachEdge;
	pt = new Point3[numVerts];

	int count = 0;
	float delta = 2.0f / (numVertexEachEdge -1);
	/*for (float x = -1.0f; x <= 1.0f; x += delta) {
		for (float y = -1.0f; y <= 1.0f; y +=delta) {
			float z = (x + 0.5f) * (x + 0.5f) + y * y;
			pt[count].set(x, y, z);
			count++;
		}
	}*/
	for (int i = 0; i < numVertexEachEdge; i++) {
		float x = -1.0f + i * delta;
		for (float j = 0; j < numVertexEachEdge; j++) {
			float y = -1.0f + j * delta;
			float z = (x + 0.5f) * (x + 0.5f) + y * y;
			pt[count].set(x, y, z);
			count++;
		}
	}

	numFaces = pow(numVertexEachEdge - 1, 2);
	face = new Face[numFaces];


	count = 0;
	for (int i = 0; i < (numVertexEachEdge - 1); i++) {
		for (int j = 0; j < (numVertexEachEdge - 1); j++) {
			face[count].nVerts = 4;
			face[count].vert = new VertexID[4];

			face[count].vert[0].vertIndex = i * numVertexEachEdge + j;
			face[count].vert[1].vertIndex = i * numVertexEachEdge + j + 1;
			face[count].vert[2].vertIndex = (i + 1) * numVertexEachEdge + j + 1;
			face[count].vert[3].vertIndex = (i + 1) * numVertexEachEdge + j;

			count++;
		}
	}
}

void Mesh::CreateEclipseCylinder(int length, int solidity, float rX, float rY) {
	numVerts = solidity * 2;
	pt = new Point3[numVerts];
	double radStep = 360*DEG2RAD / solidity;
	for (int i = 0; i < solidity; i++) {
		pt[i].x = rX*cos(radStep *i);
		pt[i].y = 0;
		pt[i].z = rY*sin(radStep*i);

		pt[i + solidity].x = pt[i].x;
		pt[i + solidity].y = length;
		pt[i + solidity].z = pt[i].z;
	}

	numFaces =  solidity;
	face = new Face[numFaces];
	for (int i = 0; i < solidity; i++) {
		face[i].nVerts = 4;
		face[i].vert = new VertexID[4];

		face[i].vert[0].vertIndex = i;
		face[i].vert[1].vertIndex = (i + 1) % solidity;
		face[i].vert[2].vertIndex = (i + 1) % solidity + solidity ;
		face[i].vert[3].vertIndex = i + solidity;
	}

	CalculateFacesNorm();
}

void Mesh::CreateHoleCylinder(float height, int solidity, float radiusInside, float radiusOutside1, float radiusOutside2) {
	numVerts = solidity * 4;
	pt = new Point3[numVerts];
	double radStep = 360 * DEG2RAD / solidity;
	for (int i = 0; i < solidity; i++) {
		pt[i].x = radiusOutside1 * cos(i*radStep);
		pt[i].y = 0;
		pt[i].z = radiusOutside1 * sin(i*radStep);

		pt[i + solidity].x = radiusOutside2 * cos(i*radStep);
		pt[i + solidity].y = height;
		pt[i + solidity].z = radiusOutside2 * sin(i*radStep);

		pt[i + 3 * solidity].x = pt[i + 2 * solidity].x = radiusInside * cos(i*radStep);
		pt[i + 2 * solidity].y = 0;
		pt[i + 3 * solidity].y = height;
		pt[i + 3 * solidity].z = pt[i + 2 * solidity].z = radiusInside * sin(i*radStep);
	}

	numFaces = 4 * solidity;
	face = new Face[numFaces];

	for (int i = 0; i < solidity; i++) {
		face[i].nVerts = 4;
		face[i].vert = new VertexID[4];
		face[i].vert[0].vertIndex = i;
		face[i].vert[1].vertIndex = (i + 1) % solidity;
		face[i].vert[2].vertIndex = (i + 1) % solidity + solidity;
		face[i].vert[3].vertIndex = i + solidity;

		int a, b, c;
		a = i + solidity;
		face[a].nVerts = 4;
		face[a].vert = new VertexID[4];
		face[a].vert[0].vertIndex = i + 2*solidity;
		face[a].vert[3].vertIndex = (i + 1) % solidity + 2*solidity;
		face[a].vert[2].vertIndex = (i + 1) % solidity + solidity + 2*solidity;
		face[a].vert[1].vertIndex = i + solidity + 2*solidity;

		b = i + 2 * solidity;
		face[b].nVerts = 4;
		face[b].vert = new VertexID[4];
		face[b].vert[0].vertIndex = i ;
		face[b].vert[1].vertIndex = (i + 1) % solidity;
		face[b].vert[2].vertIndex = (i + 1) % solidity + 2 * solidity;
		face[b].vert[3].vertIndex = i + 2 * solidity;

		c = i + 3 * solidity;
		face[c].nVerts = 4;
		face[c].vert = new VertexID[4];
		face[c].vert[0].vertIndex = i +  solidity;
		face[c].vert[1].vertIndex = (i + 1) % solidity +  solidity;
		face[c].vert[2].vertIndex = (i + 1) % solidity  + 3 * solidity;
		face[c].vert[3].vertIndex = i + 3 * solidity;
	}

	CalculateFacesNorm();
}

void Mesh:: CreatePanel(float length, float width, float thickness1, float thickness2) {
	numVerts = 8;
	pt = new Point3[numVerts];
	pt[0].set(0, thickness1 / 2, 0);
	pt[1].set(length, thickness1 / 2, 0);
	pt[2].set(length, thickness2 / 2, width);
	pt[3].set(0, thickness2 / 2, width);
	pt[4].set(0, -thickness1 / 2, 0);
	pt[5].set(length, -thickness1 / 2, 0);
	pt[6].set(length, -thickness2 / 2, width);
	pt[7].set(0, -thickness2 / 2, width);

	numFaces = 6;
	face = new Face[numFaces];

	face[1].nVerts = 4;
	face[1].vert = new VertexID[4];
	face[1].vert[0].vertIndex = 0;
	face[1].vert[1].vertIndex = 1;
	face[1].vert[2].vertIndex = 2;
	face[1].vert[3].vertIndex = 3;

	face[2].nVerts = 4;
	face[2].vert = new VertexID[4];
	face[2].vert[0].vertIndex = 4;
	face[2].vert[1].vertIndex = 7;
	face[2].vert[2].vertIndex = 6;
	face[2].vert[3].vertIndex = 5;
	
	face[3].nVerts = 4;
	face[3].vert = new VertexID[4];
	face[3].vert[0].vertIndex = 0;
	face[3].vert[1].vertIndex = 4;
	face[3].vert[2].vertIndex = 5;
	face[3].vert[3].vertIndex = 1;

	face[4].nVerts = 4;
	face[4].vert = new VertexID[4];
	face[4].vert[0].vertIndex = 1;
	face[4].vert[1].vertIndex = 5;
	face[4].vert[2].vertIndex = 6;
	face[4].vert[3].vertIndex = 2;

	face[5].nVerts = 4;
	face[5].vert = new VertexID[4];
	face[5].vert[0].vertIndex = 2;
	face[5].vert[1].vertIndex = 6;
	face[5].vert[2].vertIndex = 7;
	face[5].vert[3].vertIndex = 3;

	face[0].nVerts = 4;
	face[0].vert = new VertexID[4];
	face[0].vert[0].vertIndex = 3;
	face[0].vert[1].vertIndex = 7;
	face[0].vert[2].vertIndex = 4;
	face[0].vert[3].vertIndex = 0;

	CalculateFacesNorm();
}

void Mesh::CreateFlag(float X2, float X3, float X4, float Z3, float Z4, float thickness1, float thickness2) {
	numVerts = 8;
	pt = new Point3[numVerts];
	pt[0].set(0, thickness1 / 2, 0);
	pt[1].set(X2, thickness1 / 2, 0);
	pt[2].set(X4, thickness2 / 2, Z4);
	pt[3].set(X3, thickness2 / 2, Z3);
	pt[4].set(0, -thickness1 / 2, 0);
	pt[5].set(X2, -thickness1 / 2, 0);
	pt[6].set(X4, -thickness2 / 2,Z4);
	pt[7].set(X3, -thickness2 / 2, Z3);

	numFaces = 6;
	face = new Face[numFaces];

	face[1].nVerts = 4;
	face[1].vert = new VertexID[4];
	face[1].vert[3].vertIndex = 0;
	face[1].vert[2].vertIndex = 1;
	face[1].vert[1].vertIndex = 2;
	face[1].vert[0].vertIndex = 3;

	face[2].nVerts = 4;
	face[2].vert = new VertexID[4];
	face[2].vert[3].vertIndex = 4;
	face[2].vert[2].vertIndex = 7;
	face[2].vert[1].vertIndex = 6;
	face[2].vert[0].vertIndex = 5;

	face[3].nVerts = 4;
	face[3].vert = new VertexID[4];
	face[3].vert[0].vertIndex = 0;
	face[3].vert[1].vertIndex = 4;
	face[3].vert[2].vertIndex = 5;
	face[3].vert[3].vertIndex = 1;

	face[4].nVerts = 4;
	face[4].vert = new VertexID[4];
	face[4].vert[0].vertIndex = 1;
	face[4].vert[1].vertIndex = 5;
	face[4].vert[2].vertIndex = 6;
	face[4].vert[3].vertIndex = 2;

	face[5].nVerts = 4;
	face[5].vert = new VertexID[4];
	face[5].vert[3].vertIndex = 2;
	face[5].vert[2].vertIndex = 6;
	face[5].vert[1].vertIndex = 7;
	face[5].vert[0].vertIndex = 3;

	face[0].nVerts = 4;
	face[0].vert = new VertexID[4];
	face[0].vert[3].vertIndex = 3;
	face[0].vert[2].vertIndex = 7;
	face[0].vert[1].vertIndex = 4;
	face[0].vert[0].vertIndex = 0;

	CalculateFacesNorm();
}

void Mesh::CreateStar() {
	numVerts = 20;
	pt = new Point3[numVerts];
	pt[0].set(107, 20, 10);
	pt[1].set(130, 20, 78);
	pt[2].set(205, 20, 78);
	pt[3].set(144, 20, 122);
	pt[4].set(166, 20, 193);
	pt[5].set(108, 20, 152);
	pt[6].set(44, 20, 193);
	pt[7].set(68, 20, 123);
	pt[8].set(10, 20, 79);
	pt[9].set(84, 20, 77);

	pt[10].set(107, -20, 10);
	pt[11].set(130, -20, 78);
	pt[12].set(205, -20, 78);
	pt[13].set(144, -20, 122);
	pt[14].set(166, -20, 193);
	pt[15].set(108, -20, 152);
	pt[16].set(44, -20, 193);
	pt[17].set(68, -20, 123);
	pt[18].set(10, -20, 79);
	pt[19].set(84, -20, 77);


	numFaces = 22;
	face = new Face[numFaces];
	{
		face[0].nVerts = 5;
		face[0].vert = new VertexID[5];
		face[0].vert[4].vertIndex = 1;
		face[0].vert[3].vertIndex = 3;
		face[0].vert[2].vertIndex = 5;
		face[0].vert[1].vertIndex = 7;
		face[0].vert[0].vertIndex = 9;

		face[1].nVerts = 3;
		face[1].vert = new VertexID[3];
		face[1].vert[2].vertIndex = 0;
		face[1].vert[1].vertIndex = 1;
		face[1].vert[0].vertIndex = 9;


		face[2].nVerts = 3;
		face[2].vert = new VertexID[3];
		face[2].vert[2].vertIndex = 1;
		face[2].vert[1].vertIndex = 2;
		face[2].vert[0].vertIndex = 3;

		face[3].nVerts = 3;
		face[3].vert = new VertexID[3];
		face[3].vert[2].vertIndex = 3;
		face[3].vert[1].vertIndex = 4;
		face[3].vert[0].vertIndex = 5;

		face[4].nVerts = 3;
		face[4].vert = new VertexID[3];
		face[4].vert[2].vertIndex = 5;
		face[4].vert[1].vertIndex = 6;
		face[4].vert[0].vertIndex = 7;

		face[5].nVerts = 3;
		face[5].vert = new VertexID[3];
		face[5].vert[2].vertIndex = 7;
		face[5].vert[1].vertIndex = 8;
		face[5].vert[0].vertIndex = 9;

		face[6].nVerts = 5;
		face[6].vert = new VertexID[5];
		face[6].vert[4].vertIndex = 11;
		face[6].vert[3].vertIndex = 13;
		face[6].vert[2].vertIndex = 15;
		face[6].vert[1].vertIndex = 17;
		face[6].vert[0].vertIndex = 19;

		face[7].nVerts = 3;
		face[7].vert = new VertexID[3];
		face[7].vert[2].vertIndex = 10;
		face[7].vert[1].vertIndex = 11;
		face[7].vert[0].vertIndex = 19;


		face[8].nVerts = 3;
		face[8].vert = new VertexID[3];
		face[8].vert[2].vertIndex = 11;
		face[8].vert[1].vertIndex = 12;
		face[8].vert[0].vertIndex = 13;

		face[9].nVerts = 3;
		face[9].vert = new VertexID[3];
		face[9].vert[2].vertIndex = 13;
		face[9].vert[1].vertIndex = 14;
		face[9].vert[0].vertIndex = 15;

		face[10].nVerts = 3;
		face[10].vert = new VertexID[3];
		face[10].vert[2].vertIndex = 15;
		face[10].vert[1].vertIndex = 16;
		face[10].vert[0].vertIndex = 17;

		face[11].nVerts = 3;
		face[11].vert = new VertexID[3];
		face[11].vert[2].vertIndex = 17;
		face[11].vert[1].vertIndex = 18;
		face[11].vert[0].vertIndex = 19;
	}

	face[12].nVerts = 4;
	face[12].vert = new VertexID[4];
	face[12].vert[3].vertIndex = 0;
	face[12].vert[2].vertIndex = 1;
	face[12].vert[1].vertIndex = 11;
	face[12].vert[0].vertIndex = 10;

	face[13].nVerts = 4;
	face[13].vert = new VertexID[4];
	face[13].vert[3].vertIndex = 1;
	face[13].vert[2].vertIndex = 2;
	face[13].vert[1].vertIndex = 12;
	face[13].vert[0].vertIndex = 11;

	face[14].nVerts = 4;
	face[14].vert = new VertexID[4];
	face[14].vert[3].vertIndex = 2;
	face[14].vert[2].vertIndex = 3;
	face[14].vert[1].vertIndex = 13;
	face[14].vert[0].vertIndex = 12;

	face[15].nVerts = 4;
	face[15].vert = new VertexID[4];
	face[15].vert[3].vertIndex = 3;
	face[15].vert[2].vertIndex = 4;
	face[15].vert[1].vertIndex = 14;
	face[15].vert[0].vertIndex = 13;

	face[16].nVerts = 4;
	face[16].vert = new VertexID[4];
	face[16].vert[3].vertIndex = 4;
	face[16].vert[2].vertIndex = 5;
	face[16].vert[1].vertIndex = 15;
	face[16].vert[0].vertIndex = 14;

	face[17].nVerts = 4;
	face[17].vert = new VertexID[4];
	face[17].vert[3].vertIndex = 5;
	face[17].vert[2].vertIndex = 6;
	face[17].vert[1].vertIndex = 16;
	face[17].vert[0].vertIndex = 15;


	face[18].nVerts = 4;
	face[18].vert = new VertexID[4];
	face[18].vert[3].vertIndex = 6;
	face[18].vert[2].vertIndex = 7;
	face[18].vert[1].vertIndex = 17;
	face[18].vert[0].vertIndex = 16;

	face[19].nVerts = 4;
	face[19].vert = new VertexID[4];
	face[19].vert[3].vertIndex = 7;
	face[19].vert[2].vertIndex = 8;
	face[19].vert[1].vertIndex = 18;
	face[19].vert[0].vertIndex = 17;

	face[20].nVerts = 4;
	face[20].vert = new VertexID[4];
	face[20].vert[3].vertIndex = 8;
	face[20].vert[2].vertIndex = 9;
	face[20].vert[1].vertIndex = 19;
	face[20].vert[0].vertIndex = 18;

	face[21].nVerts = 4;
	face[21].vert = new VertexID[4];
	face[21].vert[0].vertIndex = 9;
	face[21].vert[1].vertIndex = 0;
	face[21].vert[2].vertIndex = 10;
	face[21].vert[3].vertIndex = 19;

	CalculateFacesNorm();
}

void Mesh::DrawCameraLens() {
	float lenHeight = 0.5f;
	float lenRadius = 1.0f;
	float alpha = 0.9f;
	float beta = 0.87f;
	float garma = 0.8f;
	float theta = 0.85f;
	float s1 = 0.4f;
	float s2 = 0.9f;
	float s3 = 1.0f;
	float s4 = 1.7f;
	float alphaLenRadius = lenRadius * alpha;
	float betaLenRadius = lenRadius * beta;
	float garmaLenRadius = lenRadius * garma;
	float thetaLenRadius = lenRadius * theta;
	int secCount = 360;

	numVerts = secCount * 10;
	pt = new Point3[numVerts];

	for (int i = 0; i < secCount; i++) {
		pt[i].x = lenRadius * cos(i * 360.0 / secCount * DEG2RAD);
		pt[i].z = lenRadius * sin(i * 360.0 / secCount * DEG2RAD);
		pt[i].y = 0;

		pt[i + secCount].x = alphaLenRadius * cos(i * 360.0 / secCount * DEG2RAD);
		pt[i + secCount].z = alphaLenRadius * sin(i * 360.0 / secCount * DEG2RAD);
		pt[i + secCount].y = lenHeight;
	}

	for (int i = secCount * 2; i < secCount * 3; i++) {
		pt[i].x = betaLenRadius * cos(i * 360.0 / secCount * DEG2RAD);
		pt[i].z = betaLenRadius * sin(i * 360.0 / secCount * DEG2RAD);
		pt[i].y = lenHeight;
	}

	for (int i = secCount * 3; i < secCount * 4; i++) {
		pt[i].x = garmaLenRadius * cos(i * 360.0 / secCount * DEG2RAD);
		pt[i].z = garmaLenRadius * sin(i * 360.0 / secCount * DEG2RAD);
		pt[i].y = - lenHeight * s1;
	}

	for (int i = secCount * 4; i < secCount * 5; i++) {
		pt[i].x = thetaLenRadius * cos(i * 360.0 / secCount * DEG2RAD);
		pt[i].z = thetaLenRadius * sin(i * 360.0 / secCount * DEG2RAD);
		pt[i].y = -lenHeight * s1;
	}

	for (int i = secCount * 5; i < secCount * 6; i++) {
		pt[i].x = thetaLenRadius * cos(i * 360.0 / secCount * DEG2RAD);
		pt[i].z = thetaLenRadius * sin(i * 360.0 / secCount * DEG2RAD);
		pt[i].y = -lenHeight * s2;
	}

	for (int i = secCount * 6; i < secCount * 7; i++) {
		pt[i].x = garmaLenRadius * cos(i * 360.0 / secCount * DEG2RAD);
		pt[i].z = garmaLenRadius * sin(i * 360.0 / secCount * DEG2RAD);
		pt[i].y = -lenHeight * s2;
	}

	for (int i = secCount * 7; i < secCount * 8; i++) {
		pt[i].x = garmaLenRadius * cos(i * 360.0 / secCount * DEG2RAD);
		pt[i].z = garmaLenRadius * sin(i * 360.0 / secCount * DEG2RAD);
		pt[i].y = -lenHeight * s3;
	}

	for (int i = secCount * 8; i < secCount * 9; i++) {
		pt[i].x = thetaLenRadius * cos(i * 360.0 / secCount * DEG2RAD);
		pt[i].z = thetaLenRadius * sin(i * 360.0 / secCount * DEG2RAD);
		pt[i].y = -lenHeight * s3;
	}

	for (int i = secCount * 9; i < secCount * 10; i++) {
		pt[i].x = thetaLenRadius * cos(i * 360.0 / secCount * DEG2RAD);
		pt[i].z = thetaLenRadius * sin(i * 360.0 / secCount * DEG2RAD);
		pt[i].y = -lenHeight * s4;
	}

	numFaces = secCount * 9;
	face = new Face[numFaces];

	// Draw Outter Lens
	for (int i = 0; i < secCount; i++) {
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];

		face[i].vert[0].vertIndex = i;
		face[i].vert[1].vertIndex = i + secCount;
		face[i].vert[2].vertIndex = (i + 1) % secCount + secCount;
		face[i].vert[3].vertIndex = (i + 1) % secCount;

	}

	// Draw In Lens
	for (int i = 0; i < secCount; i++) {
		face[i + secCount].nVerts = 4;
		face[i + secCount].vert = new VertexID[face[i].nVerts];

		face[i + secCount].vert[0].vertIndex = i + secCount;
		face[i + secCount].vert[1].vertIndex = i + secCount * 2;
		face[i + secCount].vert[2].vertIndex = (i + 1) % secCount + secCount * 2;
		face[i + secCount].vert[3].vertIndex = (i + 1) % secCount + secCount;

	}
	// Draw InnerLens
	for (int i = 0; i < secCount; i++) {
		face[i + secCount * 2].nVerts = 4;
		face[i + secCount * 2].vert = new VertexID[face[i].nVerts];

		face[i + secCount * 2].vert[0].vertIndex = i + secCount * 2;
		face[i + secCount * 2].vert[1].vertIndex = i + secCount * 3;
		face[i + secCount * 2].vert[2].vertIndex = (i + 1) % secCount + secCount * 3;
		face[i + secCount * 2].vert[3].vertIndex = (i + 1) % secCount + secCount * 2;

	}

	// Draw bottom Lens
	for (int i = 0; i < secCount; i++) {
		face[i + secCount * 3].nVerts = 4;
		face[i + secCount * 3].vert = new VertexID[face[i].nVerts];

		face[i + secCount * 3].vert[0].vertIndex = i;
		face[i + secCount * 3].vert[1].vertIndex = i + secCount * 4;
		face[i + secCount * 3].vert[2].vertIndex = (i + 1) % secCount + secCount * 4;
		face[i + secCount * 3].vert[3].vertIndex = (i + 1) % secCount;

	}

	// Draw bottom Lens
	for (int i = 0; i < secCount; i++) {
		face[i + secCount * 4].nVerts = 4;
		face[i + secCount * 4].vert = new VertexID[face[i].nVerts];

		face[i + secCount * 4].vert[0].vertIndex = i + secCount * 4;
		face[i + secCount * 4].vert[1].vertIndex = i + secCount * 5;
		face[i + secCount * 4].vert[2].vertIndex = (i + 1) % secCount + secCount * 5;
		face[i + secCount * 4].vert[3].vertIndex = (i + 1) % secCount + secCount * 4;

	}

	for (int i = 0; i < secCount; i++) {
		face[i + secCount * 5].nVerts = 4;
		face[i + secCount * 5].vert = new VertexID[face[i].nVerts];

		face[i + secCount * 5].vert[0].vertIndex = i + secCount * 5;
		face[i + secCount * 5].vert[1].vertIndex = i + secCount * 6;
		face[i + secCount * 5].vert[2].vertIndex = (i + 1) % secCount + secCount * 6;
		face[i + secCount * 5].vert[3].vertIndex = (i + 1) % secCount + secCount * 5;

	}

	for (int i = 0; i < secCount; i++) {
		face[i + secCount * 6].nVerts = 4;
		face[i + secCount * 6].vert = new VertexID[face[i].nVerts];

		face[i + secCount * 6].vert[0].vertIndex = i + secCount * 6;
		face[i + secCount * 6].vert[1].vertIndex = i + secCount * 7;
		face[i + secCount * 6].vert[2].vertIndex = (i + 1) % secCount + secCount * 7;
		face[i + secCount * 6].vert[3].vertIndex = (i + 1) % secCount + secCount * 6;

	}

	for (int i = 0; i < secCount; i++) {
		face[i + secCount * 7].nVerts = 4;
		face[i + secCount * 7].vert = new VertexID[face[i].nVerts];

		face[i + secCount * 7].vert[0].vertIndex = i + secCount * 7;
		face[i + secCount * 7].vert[1].vertIndex = i + secCount * 8;
		face[i + secCount * 7].vert[2].vertIndex = (i + 1) % secCount + secCount * 8;
		face[i + secCount * 7].vert[3].vertIndex = (i + 1) % secCount + secCount * 7;

	}

	for (int i = 0; i < secCount; i++) {
		face[i + secCount * 8].nVerts = 4;
		face[i + secCount * 8].vert = new VertexID[face[i].nVerts];

		face[i + secCount * 8].vert[0].vertIndex = i + secCount * 8;
		face[i + secCount * 8].vert[1].vertIndex = i + secCount * 9;
		face[i + secCount * 8].vert[2].vertIndex = (i + 1) % secCount + secCount * 9;
		face[i + secCount * 8].vert[3].vertIndex = (i + 1) % secCount + secCount * 8;

	}
}
