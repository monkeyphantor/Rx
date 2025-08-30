#include "SkeletonModelAnalyzer.hpp"


int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <skeleton_model_file>" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
    Rx::Analyzer::analyzeSkeleton(filePath);

    return 0;
}