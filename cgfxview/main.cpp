#include <cstdio>

void printUsage(const char* filename) {
	std::fprintf(stderr, "Usage: %s [-i] <cgfx file>\r\n\r\n" \
		"Available flags:\r\n" \
		"  -i  Output info about the CGFX file instead of showing it\r\n"
	);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printUsage(argv[0]);
		return 1;
	}

	return 0;
}