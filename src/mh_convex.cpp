/*
Marching Hedrons LUT generator
author: andrei (c) 2015

Marching hedron convex test

*/

#include <iostream>

#include "mh.h"

#define EPS 1e-6


bool MH::initConvexHull()
{
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 4; j++)
			edge_indices[i][j] = (i >> j) & 1;
	}
	return true;
}

bool MH::computeConvexHull(const Vec3HVec & pointVec, FaceHVec & faces)
{
	int s = pointVec.size();
	FaceHVec allfaces;
	for (int i0 = 0; i0 < s; i0++)
	{
		for (int i1 = i0 + 1; i1 < s; i1++)
		{
			for (int i2 = i1 + 1; i2 < s; i2++)
			{
				allfaces.push_back(Face::getFace(i0, i1, i2));
			}
		}
	}

	for (size_t i = 0; i < allfaces.size(); i++)
	{
		Face &f = allfaces[i];
		Vec3 n = (pointVec[f.v[2]] - pointVec[f.v[0]]).cross(pointVec[f.v[1]] - pointVec[f.v[0]]);
		bool pos = false;
		bool neg = false;
		for (int j = 0; j < s; j++)
		{
			if (j == f.v[0] || j == f.v[1] || j == f.v[2])
				continue;
			Vec3 p = pointVec[j] - pointVec[f.v[0]];
			float d = p.dot(n);
			if (d < -EPS)
				neg = true;
			else if (d > EPS)
				pos = true;
		}
		// if there are vertices on both sides of a face plane
		if (pos && neg)
			continue;

		// test if this face intersects with already added faces
		bool intersect = false;
		for (size_t k = 0; k < faces.size(); k++)
		{
			Face &f2 = faces[k];

			int ret = tr_tri_intersect3D(pointVec[f.v[0]], pointVec[f.v[1]] - pointVec[f.v[0]], pointVec[f.v[2]] - pointVec[f.v[0]], 
										pointVec[f2.v[0]], pointVec[f2.v[1]] - pointVec[f2.v[0]], pointVec[f2.v[2]] - pointVec[f2.v[0]]);
			if (ret == 1)
			{
				intersect = true;
				break;
			}
		}
		if (!intersect)
			faces.push_back(f);
	}

	return true;
}

bool MH::removeSpecialFaces(int *idx, FaceHVec & faces, FaceHVec & outfaces)
{
	// if there are special faces (faces that have all 3 verts of 12 'mc' edges), then remove them all - IF there are any other (non-special faces)
	size_t i;
	for (i = 0; i < faces.size(); i++)
	{
		if (idx[faces[i].v[0]] >= 12 || idx[faces[i].v[1]] >= 12 || idx[faces[i].v[2]] >= 12)
		{
			// we found non-special face
			for (i = 0; i < faces.size(); i++)
			{
				if (idx[faces[i].v[0]] >= 12 || idx[faces[i].v[1]] >= 12 || idx[faces[i].v[2]] >= 12)
				{
					outfaces.push_back(faces[i]);
				}
			}
			return true;
		}
	}

	return false;
}

void MH::FixNormals(const Vec3HVec & pointVec, FaceHVec & faces)
{
	static const Vec3 c(0.5f, 0.5f, 0.5f), one_third(0.333333333333f, 0.333333333333f, 0.333333333333f);

	for (size_t i = 0; i < faces.size(); i++)
	{
		Face &f = faces[i];
		Vec3 cf = (pointVec[f.v[0]] + pointVec[f.v[1]] + pointVec[f.v[2]]) * one_third;
		Vec3 n = (pointVec[f.v[2]] - pointVec[f.v[0]]).cross(pointVec[f.v[1]] - pointVec[f.v[0]]);
		if (n.dot(cf - c) > 0)
		{
			// swap
			int v = f.v[1];
			f.v[1] = f.v[0];
			f.v[0] = v;
		}
	}
}

void MH::SortOutFaces(int *idx, FaceHVec & faces, FaceHVec faceGroups[7])
{
	for (size_t i = 0; i < faces.size(); i++)
	{
		Face &f = faces[i];
		bool inGroup = false;
		for (int j = 0; j < 6; j++)
		{
			bool fv[3] = { false, false, false };
			for (int k = 0; k < 9; k++)
			{
				for (int vi = 0; vi < 3; vi++)
				{
					if (idx[f.v[vi]] == faceVertices[j][k])
						fv[vi] = true;
				}
			}
			// if all 3 face verts are from the same cube face, then put them into face group
			if (fv[0] && fv[1] && fv[2])
			{
				faceGroups[j].push_back(f);
				inGroup = true;
				break;
			}
		}
		if (!inGroup)
			faceGroups[6].push_back(f);
	}
}

int MH::AddUniqueFaceGroups(std::vector<FaceRecord> &faceStore, int *idx, FaceHVec &faces, bool *addedNew)
{
	for (size_t j = 0; j < faceStore.size(); j++)
	{
		FaceRecord & fsvec = faceStore[j];
		if (fsvec.faces.size() != faces.size())
			continue;
		bool notFound = false;
		for (size_t i = 0; i < faces.size(); i++)
		{
			int a0 = idx[faces[i].v[0]];
			int a1 = idx[faces[i].v[1]];
			int a2 = idx[faces[i].v[2]];
			bool found = false;
			for (size_t k = 0; k < fsvec.faces.size(); k++)
			{
				int b0 = fsvec.faces[k].v[0];
				int b1 = fsvec.faces[k].v[1];
				int b2 = fsvec.faces[k].v[2];
				if (a0 == b0)
				{
					if (a1 == b1)
					{
						if (a2 == b2)
						{
							found = true;
							break;
						}
					} 
					else if (a1 == b2)
					{
						if (a2 == b1)
						{
							found = true;
							break;
						}
					}
				} 
				else if (a0 == b1)
				{
					if (a1 == b0)
					{
						if (a2 == b2)
						{
							found = true;
							break;
						}
					}
					else if (a1 == b2)
					{
						if (a2 == b0)
						{
							found = true;
							break;
						}
					}
				}
				else if (a0 == b2)
				{
					if (a1 == b0)
					{
						if (a2 == b1)
						{
							found = true;
							break;
						}
					}
					else if (a1 == b1)
					{
						if (a2 == b0)
						{
							found = true;
							break;
						}
					}
				}
			}
			if (!found)
			{
				notFound = true;
				break;
			}
		}
		if (notFound)
			continue;

		// ok, we found it
		return j;
	}
	
	// not found, add it to the list
	FaceRecord newrec;
	for (size_t k = 0; k < faces.size(); k++)
	{
		newrec.faces.push_back(Face::getFace(idx[faces[k].v[0]], idx[faces[k].v[1]], idx[faces[k].v[2]]));
	}
	newrec.idx = faceStore.size();
	faceStore.push_back(newrec);
	*addedNew = true;
	return newrec.idx;
}
