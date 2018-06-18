#if !defined (_MESH_CLASS)
#define _MESH_CLASS

#include "supportClass.h"


class VertexID
{
public:
	int		vertIndex;
	int		colorIndex;
};

class Face
{
public:
	int		nVerts;
	Vector3 facenorm;
	VertexID*	vert;

	Face()
	{
		nVerts = 0;
		vert = NULL;
	}
	~Face()
	{
		if (vert != NULL)
		{
			delete[] vert;
			vert = NULL;
		}
		nVerts = 0;
	}
};

class Mesh
{
public:
	int		numVerts;
	Point3*		pt;
	float		slideX, slideY, slideZ;
	float		rotateX, rotateY, rotateZ;
	float		scaleX, scaleY, scaleZ;

	int		numFaces;
	Face*		face;
public:
	Mesh()
	{
		numVerts = 0;
		pt = NULL;
		numFaces = 0;
		face = NULL;
	}
	~Mesh()
	{
		if (pt != NULL)
		{
			delete[] pt;
		}
		if (face != NULL)
		{
			delete[] face;
		}
		numVerts = 0;
		numFaces = 0;
	}
	
		void Draw();
		void CalculateFacesNorm();
		void DrawCameraLens();
		void CreateHeliFace(int numVertexEachEdge);
		void CreateEclipseCylinder(int length, int solidity, float rX, float rY);
		void CreateHoleCylinder(float height, int solidity, float radiusInside, float radiusOutside1, float radiusOutside2);
		void CreatePanel(float length, float width, float thickness1, float thickness2);
		void CreateFlag(float X2, float X3, float X4, float Z3, float Z4, float thickness1, float thickness2);
		void CreateStar();
	};

#endif