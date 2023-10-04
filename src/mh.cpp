/*
Marching Hedrons LUT generator
author: andrei (c) 2015

*/

#include <stdio.h>
#include <iostream>
#include <iomanip>

#include "mh.h"

#define OUTPUT_BIN

//#define OUTPUT_TEXT

const int deviceIdx = 0;
//const Distribution dist = UniformDistribution;

//////////////////////////////////////////////////////////////////////////


void addFace(FaceHVec *faces, int v0, int v1, int v2, int skipVert)
{
	if (v0 == skipVert || v1 == skipVert || v2 == skipVert)
		return;
	faces->push_back(Face::getFace(v0, v1, v2));
}


//void setPoints(Point3HVec *pointNum, int *idx, int mask)
void setPoints(Vec3HVec *pointNum, int *idx, int mask, int inv, int index)
{
	for (int i = 0, j = 0; i < 26; i++)
	{
		int e0 = mask & (1 << MH::cell_edges[i][0]);
		int e1 = mask & (1 << MH::cell_edges[i][1]);
		if (MH::cell_edges[i][0] == 14)	// special case for central edges
		{
			if (!e0)
				continue;
		}
		if ((e0 && !e1) || (!e0 && e1))
		{
			Vec3 p;
			if (index >= 0 && i < 8)
			{
				float coeff;
				coeff = MH::edge_indices[index][i & 3] == 0 ? 0.2f : 0.8f;
				p = MH::cell_vertices[MH::cell_edges[i][0]].interp(MH::cell_vertices[MH::cell_edges[i][1]], coeff);
			}
			else
			{
				int k;
				if (inv == 0)
					k = 0;
				else if (i >= 8)
					k = 1;
				else
					k = 0;
				if (i < 12)
				{
					if ((mask & (1 << MH::edgeFacesCentralVertices[i][0])) && (mask & (1 << MH::edgeFacesCentralVertices[i][1])))
					{
						if (inv == 0)
							k = 1;
						else if (i >= 8)
							k = 0;
						else
							k = 1;
					}
				}
				p.x = MH::cell_hedron_vertices[k][i][0];
				p.y = MH::cell_hedron_vertices[k][i][1];
				p.z = MH::cell_hedron_vertices[k][i][2];
			}
			

			idx[j++] = i;
			pointNum->push_back(p);
		}
	}
}

//////////////////////////////////////////////////////

int main(int argc, const char* argv[])
{
#ifdef OUTPUT_BIN
	FILE *fpi = fopen("mh_idx.bin", "wb");
	FILE *fpi0 = fopen("mh_offs.bin", "wb");
	FILE *fpih = fopen("mh_h_idx.bin", "wb");
	FILE *fp = fopen("mh.bin", "wb");
	int offs = 0;
#endif

	MH::initConvexHull();

	std::vector<FaceRecord> faceGroupStore[7];
	int totalIdx = 0;

	for (int inv = 0; inv < 2; inv++)
	{
		for (int idx = 0; idx < 16; idx++)
		{
			if (inv == 0)
				std::cout << "Starting Hedrons #" << idx << "...\n";
			else
				std::cout << "Starting Holes #" << idx << "...\n";

			for (int i = 0; i < (1 << 14); i++)
			{
				Vec3HVec   pointVec;
				int indices[26];

				if (i > 0 && (i % 1000) == 0)
				{
					std::cout << "// " << i << std::endl;
				}

				int mask = i | (1 << 14);

				setPoints(&pointVec, indices, mask, inv, idx);

				FaceHVec faces;
				FaceHVec faceGroups[7];

				if (pointVec.size() == 3)
				{
					addFace(&faces, 0, 1, 2, -1);
					faceGroups[0].push_back(faces[0]);
				}
				else
				{
					FaceHVec fac;
					MH::computeConvexHull(pointVec, fac);
					MH::removeSpecialFaces(indices, fac, faces);
					MH::FixNormals(pointVec, faces);
					MH::SortOutFaces(indices, faces, faceGroups);
				}

#ifdef OUTPUT_TEXT
				std::cout << "    /*" << i << " (N=" << faces.size() << ") */ { ";
				for (int j = 0; j < faces.size(); j++)
				{
					std::cout << "{ " << indices[faces[j].v[0]] << " /*" << faces[j].v[0] << "*/, " << indices[faces[j].v[1]] << "/*" << faces[j].v[1] << "*/, " << indices[faces[j].v[2]] << " /*" << faces[j].v[2] << "*/ }, ";
				}

				std::cout << "{ -1, -1, -1 } }, " << std::endl;
#endif

#ifdef OUTPUT_BIN
				for (int f = 0; f < 7; f++)
				{
					IntHVec offsList;
					bool addedNew = false;
					int fidx = MH::AddUniqueFaceGroups(faceGroupStore[f], indices, faceGroups[f], &addedNew);

					FaceRecord &fr = faceGroupStore[f][fidx];
					if (addedNew)	// new record added
					{
						fr.totalIdx = totalIdx;
						fwrite(&offs, sizeof(offs), 1, fpi0);
						fputc(fr.faces.size(), fp);
						offs ++;
						for (size_t j = 0; j < fr.faces.size(); j++)
						{
							fputc(fr.faces[j].v[0], fp);
							fputc(fr.faces[j].v[1], fp);
							fputc(fr.faces[j].v[2], fp);
							offs += 3;
						}
						totalIdx++;
					}

					fputc(fr.totalIdx & 0xff, inv == 0 ? fpi : fpih);
					fputc((fr.totalIdx >> 8) & 0xff, inv == 0 ? fpi : fpih);
					//fputc((fr.totalIdx >> 16) & 0xff, inv == 0 ? fpi : fpih);
				}
#endif
			}
		}
	}

#ifdef OUTPUT_BIN
	fclose(fp);
	fclose(fpi);
	fclose(fpi0);
	fclose(fpih);
#endif

	std::cout << "NUM: " << faceGroupStore[0].size() << " " << faceGroupStore[1].size() << " " << faceGroupStore[2].size()
		<< " " << faceGroupStore[3].size() << " " << faceGroupStore[4].size() << " " << faceGroupStore[5].size() 
		<< " " << faceGroupStore[6].size() << std::endl;
	std::cout << "TOTALIDX = " << totalIdx << std::endl;

	return 0;
}


