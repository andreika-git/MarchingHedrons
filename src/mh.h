/*
Marching Hedrons LUT generator
author: andrei (c) 2015

*/

#ifndef _MH_H_
#define _MH_H_

#include <vector>
#include "vec.h"

struct Face
{
	int v[3];

	static Face getFace(int v0, int v1, int v2)
	{
		Face f;
		// 012
		f.v[0] = v0; f.v[1] = v1; f.v[2] = v2;
		return f;
	}

	static Face getSortedFace(int v0, int v1, int v2)
	{
		Face f;
		// 012
		if (v0 < v1 && v1 < v2)
		{
			f.v[0] = v0; f.v[1] = v1; f.v[2] = v2;
		}
		// 021
		if (v0 < v2 && v2 < v1)
		{
			f.v[0] = v0; f.v[1] = v2; f.v[2] = v1;
		}
		// 102
		if (v1 < v0 && v0 < v2)
		{
			f.v[0] = v1; f.v[1] = v0; f.v[2] = v2;
		}
		// 120
		if (v1 < v2 && v2 < v0)
		{
			f.v[0] = v1; f.v[1] = v2; f.v[2] = v0;
		}
		// 201
		if (v2 < v0 && v0 < v1)
		{
			f.v[0] = v2; f.v[1] = v0; f.v[2] = v1;
		}
		// 210
		if (v2 < v1 && v1 < v0)
		{
			f.v[0] = v2; f.v[1] = v1; f.v[2] = v0;
		}
		return f;
	}

	friend bool operator== (const Face &n1, const Face &n2)
	{
		return Face::equal(n1, n2);
	}

	friend bool operator!= (const Face &n1, const Face &n2)
	{
		return !Face::equal(n1, n2);
	};

private:
	static bool equal(const Face & f0, const Face & f1)
	{
		return  (f0.v[0] == f1.v[0] && f0.v[1] == f1.v[1] && f0.v[2] == f1.v[2]) ||
			(f0.v[0] == f1.v[0] && f0.v[1] == f1.v[2] && f0.v[2] == f1.v[1]) ||
			(f0.v[0] == f1.v[1] && f0.v[1] == f1.v[0] && f0.v[2] == f1.v[2]) ||
			(f0.v[0] == f1.v[1] && f0.v[1] == f1.v[2] && f0.v[2] == f1.v[0]) ||
			(f0.v[0] == f1.v[2] && f0.v[1] == f1.v[1] && f0.v[2] == f1.v[0]) ||
			(f0.v[0] == f1.v[2] && f0.v[1] == f1.v[0] && f0.v[2] == f1.v[1]);
	}
};

typedef std::vector<Face>   FaceHVec;

typedef std::vector<Vec3>   Vec3HVec;

typedef std::vector<int>	IntHVec;

struct FaceRecord
{
	FaceHVec faces;
	int idx;
	int totalIdx;
};

class MH
{
public:
	static const int cell_edges[26][2];
	static const Vec3 cell_vertices[8];
	static const float cell_hedron_vertices[2][26][3];
	static const int faceVertices[6][9];
	static const int edgeFacesCentralVertices[12][2];
	static int edge_indices[16][4];

public:

	static bool initConvexHull();
	static bool computeConvexHull(const Vec3HVec & pointVec, FaceHVec & faces);
	static bool removeSpecialFaces(int *idx, FaceHVec & faces, FaceHVec & outfaces);
	static void FixNormals(const Vec3HVec & pointVec, FaceHVec & outfaces);
	static void SortOutFaces(int *idx, FaceHVec & faces, FaceHVec faceGroups[7]);
	static int AddUniqueFaceGroups(std::vector<FaceRecord> &faceStore, int *idx, FaceHVec &faceGroups, bool *addedNew);

	static int tr_tri_intersect3D(const Vec3 & C1, const Vec3 & P1, const Vec3 & P2, const Vec3 & D1, const Vec3 & Q1, const Vec3 & Q2);
};

#endif // of _MH_H_
