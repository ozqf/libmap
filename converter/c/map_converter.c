#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <vector.h>
#include <map_parser.h>
#include <map_data.h>
#include <geo_generator.h>

#define DEG2RAD 3.141593f / 180.0f
#define RAD2DEG 57.2958f

typedef struct SortIndex
{
	int index;
	float angle;
} SortIndex;

typedef struct MeshTri
{
	vec3 va, vb, vc;
	vertex_uv ta, tb, tc;
	vec3 n;
} MeshTri;

typedef struct MeshTexture
{
	int textureIndex;
	char* name;
	int trisCount;
	MeshTri* tris;
} MeshTexture;

// face iteration callback
typedef void(FaceIterationCallback)(face* f, face_geometry* faceGeo);


static int g_numTextures;
static MeshTexture* g_textures;

extern void ListTextures()
{
	int numTextures = map_data_get_texture_count();
	printf("--- Textures (%d) ---\n", numTextures);
	for (int i = 0; i < numTextures; ++i)
	{
		texture_data* tex = map_data_get_texture(i);
		printf("\"%s\" size %d by %d\n",
			tex->name, tex->width, tex->height);
	}
}

static int TrianglesForFace(int faceVertexCount)
{
	return faceVertexCount - 2;
}

static void Callback_TallyTextureTriangles(face* f, face_geometry* faceGeo)
{
	MeshTexture* meshTex = &g_textures[f->texture_idx];
	meshTex->trisCount += TrianglesForFace(faceGeo->vertex_count);
}

static void TallyTextureTriangles(MeshTexture* meshTextures, int numTextures)
{
	int entCount = map_data_get_entity_count();
	for (int ei = 0; ei < entCount; ++ei)
	{
		char* propValue = map_data_get_entity_property(ei, "classname");
		if (propValue == NULL) { continue; }
		if (strcmp(propValue, "func_group") != 0) { continue; }

		entity *ent = &entities[ei];
		const entity_geometry* entGeo = &geo_generator_get_entities()[ei];
		int brushCount = ent->brush_count;
		if (brushCount == 0) { continue; }

		for (int bi = 0; bi < brushCount; ++bi)
		{
			brush *entBrush = &ent->brushes[bi];
            brush_geometry *brushGeo = &entGeo->brushes[bi];
			int faceCount = entBrush->face_count;

			for (int fi = 0; fi < faceCount; ++fi)
			{
				face* f = &entBrush->faces[fi];
				face_geometry* faceGeo = &brushGeo->faces[fi];
				int vertCount = faceGeo->vertex_count;

				MeshTexture* meshTex = &meshTextures[f->texture_idx];
				meshTex->trisCount += TrianglesForFace(faceGeo->vertex_count);
				// faceGeo->vertex_count
			}
		}
	}
	
	for (int i = 0; i < numTextures; ++i)
	{
		MeshTexture* tex = &meshTextures[i];
		printf("Tex %d \"%s\" has %d tris (%lld bytes)\n",
			i, tex->name, tex->trisCount, sizeof(MeshTri) * tex->trisCount);
		tex->tris = (MeshTri*)malloc(sizeof(MeshTri) * tex->trisCount);
	}
}

static void AllocMeshTextures()
{
	// allocate textures
	g_numTextures = map_data_get_texture_count();
	g_textures = (MeshTexture*)malloc(sizeof(MeshTexture) * g_numTextures);
	for (int i = 0; i < g_numTextures; ++i)
	{
		texture_data* tex = map_data_get_texture(i);
		g_textures[i].textureIndex = i;
		g_textures[i].name = tex->name;
	}

	// tally triangles required and alloc tri arrays
	TallyTextureTriangles(g_textures, g_numTextures);

	// for (int i = 0; i < g_numTextures; ++i)
	// {
	// 	MeshTexture* tex = &g_textures[i];
	// 	tex->tris = (MeshTri*)malloc(sizeof(MeshTri) * tex->trisCount);
	// }

	// fill tri arrays
}

/*
Convert an unsorted list of vertices of an arbitrary length to
correctly ordered triangles.
> sort polygon vertices - clockwise winding
> triangulate polygon - eg 4 vertices == 2 triangles

*/
static void TriangulateFace(
	face_vertex* vertices,
	int vertCount,
	vec3 normal)
{
	SortIndex* sorting = (SortIndex*)malloc(sizeof(SortIndex) * vertCount);
	
	// calc centre of face
	vec3 centre = { 0, 0, 0 };
	for (int vi = 0; vi < vertCount; ++vi)
	{
		face_vertex* vert = &vertices[vi];
		#if 1
		printf("Vert %d: %.3f, %.3f, %.3f. UVs %.3f, %.3f\n",
			vi,
			vert->vertex.x, vert->vertex.y, vert->vertex.z,
			(float)vert->uv.u, (float)vert->uv.v);
		#endif
		centre.x += vert->vertex.x;
		centre.y += vert->vertex.y;
		centre.z += vert->vertex.z;
	}
	centre.x /= vertCount;
	centre.y /= vertCount;
	centre.z /= vertCount;
	printf("\tFace centre: %.3f, %.3f, %.3f. Normal: %.3f, %.3f, %.3f\n",
		centre.x, centre.y, centre.z,
		normal.x, normal.y, normal.z);
	
	// select the first 'spoke'. Spokes to other vertices will
	// be compared to this one
	// vec3 rootSpoke;
	// rootSpoke.x = vertices[0].vertex.x - centre.x;
	// rootSpoke.y = vertices[0].vertex.y - centre.y;
	// rootSpoke.z = vertices[0].vertex.z - centre.z;
	for (int vi = 1; vi < vertCount; ++vi)
	{
		// vec3 vertA = vertices[vi - 1].vertex;
		vec3 vertA = vertices[0].vertex;
		vec3 vertB = vertices[vi].vertex;
		vec3 a, b;
		a.x = vertA.x - centre.x;
		a.y = vertA.y - centre.y;
		a.z = vertA.z - centre.z;
		b.x = vertB.x - centre.x;
		b.y = vertB.y - centre.y;
		b.z = vertB.z - centre.z;
		a = vec3_normalize(a);
		b = vec3_normalize(b);
		printf("Testing centre spokes: %.3f, %.3f, %.3f and %.3f, %.3f, %.3f\n",
			a.x, a.y, a.z, b.x, b.y, b.z);
		double dotA = vec3_dot(a, b);
		double radians = acos(dotA);
		// dotA /= vec3_length(a);
		// dotA /= vec3_length(b);
		if (radians < 0)
		{
			radians += 180.f * DEG2RAD;
		}
		else if (radians == 0)
		{
			radians += 90.f * DEG2RAD;
		}
		// double lenA = vec3_length(a);
		// double lenB = vec3_length(b);
		printf("\tDot %d -> %d: %.3f. Angle:  %.3f (%.3f degrees)\n",
			0, vi, dotA, radians,  radians * RAD2DEG);
	}
}

static void WriteMesh()
{
	AllocMeshTextures();

	int numTextures = map_data_get_texture_count();
	int totalVertices = 0;
	int entCount = map_data_get_entity_count();
	printf("Num Ents %d\n", entCount);

	#if 0
	for (int ei = 0; ei < entCount; ++ei)
	{
		entity *ent = &entities[ei];
		const entity_geometry* entGeo = &geo_generator_get_entities()[ei];
		printf("Ent %d has %d brushes\n", ei, ent->brush_count);
		int brushCount = ent->brush_count;
		if (brushCount == 0)
		{
			printf("\tEnt has no brushes\n");
			continue;
		}
		for (int bi = 0; bi < brushCount; ++bi)
		{
			brush *entBrush = &ent->brushes[bi];
            brush_geometry *brushGeo = &entGeo->brushes[bi];
			int faceCount = entBrush->face_count;
            printf("Brush %d has %d faces\n", bi, faceCount);
			// face texture_idx
			for (int fi = 0; fi < faceCount; ++fi)
			{
				face* f = &entBrush->faces[fi];
				face_geometry* faceGeo = &brushGeo->faces[fi];
				int vertCount = faceGeo->vertex_count;
				if (vertCount > 3000)
				{
					printf("Excessive vert count %d\n", vertCount);
					continue;
				}
				totalVertices += vertCount;
				texture_data* tex = &map_data_get_textures()[f->texture_idx];
				printf("> Face %d has %d verts. texture_idx %d: %s\n",
					fi,
					vertCount,
					f->texture_idx,
					tex->name);
				TriangulateFace(faceGeo->vertices, vertCount, f->plane_normal);
				#if 0
				for (int vi = 0; vi < vertCount; ++vi)
				{
					face_vertex* vert = &faceGeo->vertices[vi];
					printf("Vert %.3f, %.3f, %.3f. UVs %.3f, %.3f\n",
						vert->vertex.x, vert->vertex.y, vert->vertex.z,
						(float)vert->uv.u, (float)vert->uv.v);
				}
				#endif
			}
		}
	}
	#endif
}

extern int MapConverter_Run(const char* inputPath, const char* outputPath)
{
	printf("Input: \"%s\" output \"%s\"...\n", inputPath, outputPath);
	// 1 - parse map file
	bool result = map_parser_load(inputPath);
	if (!result)
	{
		printf("Map load failed!\n");
		return 1;
	}

	// 2 - setup textures - required for UV coordinates
	ListTextures();
	printf("Generating Geometry...\n");
	
	// 3 - generate mesh geometry
	geo_generator_run();
	printf("...Done\n");
	
	// 4 - write output
	WriteMesh();
	printf("Done - success\n");
	return 0;
}
