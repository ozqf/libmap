#include <stdio.h>

#include <map_parser.h>
#include <map_data.h>
#include <geo_generator.h>

static void WriteMesh()
{
	int numTextures = map_data_get_texture_count();
	int totalVertices = 0;
	int entCount = map_data_get_entity_count();
	printf("Num Ents %d, Num textures %d\n", entCount, numTextures);
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
				printf("Face %d has %d verts. texture_idx %d: %s\n",
					fi,
					vertCount,
					f->texture_idx,
					tex->name);
				for (int vi = 0; vi < vertCount; ++vi)
				{
					face_vertex* vert = &faceGeo->vertices[vi];
					printf("Vert %.3f, %.3f, %.3f. UVs %.3f, %.3f\n",
						vert->vertex.x, vert->vertex.y, vert->vertex.z,
						(float)vert->uv.u, (float)vert->uv.v);
				}
			}
		}
	}
}

int main(int argc, char** argv)
{
	printf("Launch params: ");
	for (int i = 0; i < argc; ++i)
	{
		printf("%s, ", argv[i]);
	}
	printf("\n");
	if (argc < 3)
	{
		printf("Missing arguments. Command line must be <exe> <input.map> <output.obj>\n");
		return 1;
	}
	printf("Input: \"%s\" output \"%s\"...\n", argv[1], argv[2]);
	bool result = map_parser_load(argv[1]);
	if (!result)
	{
		printf("Map load failed!\n");
		return 1;
	}
	printf("Generating Geometry...\n");
	geo_generator_run();
	printf("...Done\n");
	// printf("Map texture count: %d\n", map_data_get_texture_count());
	// geo_generator_print_entities();
	WriteMesh();
	printf("Done - success\n");
	return 0;
}
