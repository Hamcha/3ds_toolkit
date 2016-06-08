#include <string>

#include <cstdio>

#include <formatters/CGFX.h>

bool readFile(const char* path, uint8_t** buffer, size_t* bufsize) {
	// Open file handle
	FILE* file = fopen(path, "rb");
	if (file == nullptr) {
		return false;
	}

	// Get filesize
	fseek(file, 0, SEEK_END);
	*bufsize = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Allocate buffer and read file into it
	*buffer = (uint8_t*)malloc(*bufsize);
	if (*buffer == nullptr) {
		fclose(file);
		return false;
	}
	fread(*buffer, *bufsize, 1, file);

	// Close handle and return true
	fclose(file);
	return true;
}

void printInfo(const cgfx::CGFXData& cgfx) {
	std::printf("CGFX ver. %d - %d blocks\r\n",
				cgfx.version,
				cgfx.blockCount);
	std::printf("%d model%s, %d texture%s\r\n",
				cgfx.models.size(),
				cgfx.models.size() > 1 ? "s" : "",
				cgfx.textures.size(),
				cgfx.textures.size() > 1 ? "s" : "");
}

void printUsage(const char* filename) {
	std::fprintf(stderr, "Usage: %s [-i] <cgfx file>\r\n\r\n" \
		"Available flags:\r\n" \
		"  -i  Output info about the CGFX file instead of showing it\r\n",
		filename
	);
}

enum CmdFlags : uint8_t {
	FlPrintInfo = 1 << 0
};

int main(int argc, char *argv[]) {
	std::string filePath;
	uint8_t flags = 0;

	// Parse argv
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			size_t argl = strlen(argv[i]);
			for (int c = 1; c < argl; c++) {
				switch (argv[i][c]) {
				case 'i':
					flags |= FlPrintInfo;
					break;
				default:
					std::fprintf(stderr, "Invalid option specified: %c\r\n", argv[i][c]);
					printUsage(argv[0]);
					return EXIT_FAILURE;
				}
			}
		} else {
			if (filePath.empty()) {
				filePath = std::string(argv[i]);
			} else {
				printUsage(argv[0]);
				return EXIT_FAILURE;
			}
		}
	}

	// Filepath is required
	if (filePath.empty()) {
		printUsage(argv[0]);
		return EXIT_FAILURE;
	}

	uint8_t* data = nullptr;
	size_t   size = 0;
	if (!readFile(filePath.c_str(), &data, &size)) {
		fprintf(stderr, "FATAL: Could not read file\r\n");
		return EIO;
	}

	cgfx::CGFX cgfxFile;
	if (!cgfxFile.loadFile(data, size)) {
		fprintf(stderr, "FATAL: CGFX parse failure\r\n");
		return EXIT_FAILURE;
	}

	if ((flags | FlPrintInfo) != 0) {
		std::printf("Info for %s:\r\n\r\n", filePath.c_str());
		printInfo(cgfxFile.data());

		// Hacky, but easier than setting up a better debugging env
		std::getchar();
	}

	return EXIT_SUCCESS;
}