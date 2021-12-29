#include <stdio.h>

#include <map_parser.h>
#include <map_data.h>
#include <geo_generator.h>

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
	geo_generator_print_entities();

	printf("Done - success\n");
	return 0;
}
