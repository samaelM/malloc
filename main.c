#include <stdio.h>
#include "malloc.h"

int main(int argc, char const *argv[])
{
	char *memory = malloc(5);
	memory[0] = 0;
	memory[4] = 0;
	return 0;
}
