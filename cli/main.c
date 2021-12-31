#include <stdio.h>

#include "../converter/h/map_converter.h"

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
	return MapConverter_Run(argv[1], argv[2]);
}
