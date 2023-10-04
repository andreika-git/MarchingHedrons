/*
Marching Hedrons LUT generator
author: andrei (c) 2015

*/

#include "mh.h"

const int MH::cell_edges[26][2] =
{
	// the first 12 edges are the same as in MC
	{ 0, 1 }, { 1, 5 }, { 5, 4 }, { 4, 0 },
	{ 3, 2 }, { 2, 6 }, { 6, 7 }, { 7, 3 },
	{ 0, 3 }, { 1, 2 }, { 5, 6 }, { 4, 7 },

	// there are 14 more edges coming from the central point:
	// - 8 to corner
	{ 14, 0 }, { 14, 1 }, { 14, 2 }, { 14, 3 },
	{ 14, 4 }, { 14, 5 }, { 14, 6 }, { 14, 7 },
	// - and 6 to facenormal (center of each cube face)
	{ 14, 8 }, { 14, 9 }, { 14, 10 }, { 14, 11 }, { 14, 12 }, { 14, 13 },
};


#define E01 0.05
#define E09 0.95
#define V00 0.01
#define V10 0.99

const Vec3 MH::cell_vertices[8] =
{
	Vec3(V00, V00, V00), Vec3(V10, V00, V00), Vec3(V10, V10, V00), Vec3(V00, V10, V00),
	Vec3(V00, V00, V10), Vec3(V10, V00, V10), Vec3(V10, V10, V10), Vec3(V00, V10, V10),
};

#define A05 0.2

const float MH::cell_hedron_vertices[2][26][3] =
{
	// low
	{
		// 12 cube edges
		// ++--
		// ++--
		{ A05, V00, V00 }, { V10, V00, A05 }, { A05, V00, V10 }, { V00, V00, A05 },
		{ A05, V10, V00 }, { V10, V10, A05 }, { A05, V10, V10 }, { V00, V10, A05 },
		{ V00, A05, V00 }, { V10, A05, V00 }, { V10, A05, V10 }, { V00, A05, V10 },

		// 8 center-to-corner edges
		{ E01, E01, E01 }, { E09, E01, E01 }, { E09, E09, E01 }, { E01, E09, E01 },
		{ E01, E01, E09 }, { E09, E01, E09 }, { E09, E09, E09 }, { E01, E09, E09 },

		// 6 center-to-facenormal edges
		{ 0.5, 1.0, 0.5 }, { 0.5, 0.5, 0.0 }, { 1.0, 0.5, 0.5 }, { 0.5, 0.5, 1.0 }, { 0.0, 0.5, 0.5 }, { 0.5, 0.0, 0.5 },
	},

#undef A05
#define A05 0.8
	// high
	{
		// 12 cube edges
		{ A05, V00, V00 }, { V10, V00, A05 }, { A05, V00, V10 }, { V00, V00, A05 },
		{ A05, V10, V00 }, { V10, V10, A05 }, { A05, V10, V10 }, { V00, V10, A05 },
		{ V00, A05, V00 }, { V10, A05, V00 }, { V10, A05, V10 }, { V00, A05, V10 },

		// 8 center-to-corner edges
		{ E01, E01, E01 }, { E09, E01, E01 }, { E09, E09, E01 }, { E01, E09, E01 },
		{ E01, E01, E09 }, { E09, E01, E09 }, { E09, E09, E09 }, { E01, E09, E09 },

		// 6 center-to-facenormal edges
		{ 0.5, 1.0, 0.5 }, { 0.5, 0.5, 0.0 }, { 1.0, 0.5, 0.5 }, { 0.5, 0.5, 1.0 }, { 0.0, 0.5, 0.5 }, { 0.5, 0.0, 0.5 },
	}
};

const int MH::faceVertices[6][9] =
{
	{ 4, 5, 6, 7, 14, 15, 18, 19, 20 },
	{ 0, 8, 4, 9, 12, 13, 14, 15, 21 },
	{ 1, 9, 5, 10, 13, 14, 18, 17, 22 },
	{ 2, 10, 6, 11, 16, 17, 18, 19, 23 },
	{ 3, 8, 7, 11, 12, 15, 19, 16, 24 },
	{ 0, 1, 2, 3, 12, 13, 17, 16, 25 },
};

const int MH::edgeFacesCentralVertices[12][2] = 
{
	{ 13, 9 }, { 13, 10 }, { 13, 11 }, { 13, 12 },
	{ 8, 9 }, { 8, 10 }, { 8, 11 }, { 8, 12 },
	{ 9, 12 }, { 9, 10 }, { 10, 11 }, { 11, 12 },
};

int MH::edge_indices[16][4];
