#include "TextureModelConverter.hpp"
#include "SkeletonModelConverter.hpp"

int main(int argc, char* argv[])
{
    // Check for the proper number of arguments
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] << " -m <mode> -i <input file> -o <output file>" << std::endl;
        return 1;
    }

    std::string mode;
    std::string inputFile;
    std::string outputFile;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-m") == 0) {
            if (i + 1 < argc) {
                mode = argv[++i];
            } else {
                std::cerr << "Error: Missing mode value after -m" << std::endl;
                return 1;
            }
        } else if (strcmp(argv[i], "-i") == 0) {
            if (i + 1 < argc) {
                inputFile = argv[++i];
            } else {
                std::cerr << "Error: Missing input file after -i" << std::endl;
                return 1;
            }
        } else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "Error: Missing output file after -o" << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Error: Unknown argument: " << argv[i] << std::endl;
            return 1;
        }
    }

    // Call the appropriate converter function based on mode
    if (mode == "skelMod") {
        Rx::Converter::convertToSkeletonModelFile(inputFile, outputFile);
    } else if (mode == "texMod") {
        Rx::Converter::convertToTextureModelFile(inputFile, outputFile);
    } else {
        std::cerr << "Error: Unknown mode: " << mode << std::endl;
        return 1;
    }

    std::cout << "Conversion completed successfully." << std::endl;
    return 0;
}