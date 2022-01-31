#include <opencv2/opencv.hpp>
#include <cmath>

#include "TimeMeasurement.h"
#include "Line.h"
#include "CourtLinePixelDetector.h"
#include "CourtLineCandidateDetector.h"
#include "TennisCourtFitter.h"
#include "DebugHelpers.h"

using namespace cv;

int process_file(const std::string &in_filename, const std::string &out_filename) {
    /**
     * Finds and outputs lines in a specified file
     *
     * Returns -1 if the file could not be found
     * Returns 1 if there was a processing error
     * Returns 0 in case of success
     */

    Mat frame;
    frame = imread(in_filename);
    if(frame.empty()) {
        return -1;
    }

    std::cout << "Elaborating file " << in_filename << std::endl;

    CourtLinePixelDetector courtLinePixelDetector;
    CourtLineCandidateDetector courtLineCandidateDetector;
    TennisCourtFitter tennisCourtFitter;

    try {
        TimeMeasurement::start("LineDetection");
        Mat binaryImage = courtLinePixelDetector.run(frame);
        std::vector<Line> candidateLines = courtLineCandidateDetector.run(binaryImage, frame);
        TennisCourtModel model = tennisCourtFitter.run(candidateLines, binaryImage, frame);
        int elapsed_seconds = TimeMeasurement::stop("LineDetection");
        std::cout << "Completed " << in_filename << " in: " << elapsed_seconds << "s." << std::endl;

        model.writeToFile(out_filename);
        model.drawModel(frame);
        imwrite(out_filename + ".with_lines.png", frame);

    } catch (std::runtime_error &e) {
        std::cerr << "Processing error " << " in file " << in_filename << ": " << e.what()  << std::endl;
        return 1;
    }

    return 0;
}

std::string generate_base_filename(int idx) {
    int digits = std::to_string(idx).length();
    int padding_zeros = 5 - digits;

    std::string base_filename = "";
    for(int i = 0; i < padding_zeros; ++i) {
        base_filename += "0";
    }

    base_filename += std::to_string(idx);


    return base_filename;
}

int main(int argc, char **argv) {
    TimeMeasurement::debug = false;
    CourtLinePixelDetector::debug = false;
    CourtLineCandidateDetector::debug = false;
    TennisCourtFitter::debug = false;

    std::string image_extension = ".png";
    std::string output_extension = ".txt";

    if (argc != 2) {
        std::cout << "Usage: ./detect directory_path" << std::endl;
        std::cout << "       directory_path:  path to an input image directory." << std::endl;
        return -1;
    }
    std::string directory_name(argv[1]);

    int result = 0;
    int current_idx = 0;
    while(result != -1) {
        std::string base_name = directory_name + "/" + generate_base_filename(current_idx);
        std::string frame_name = base_name + image_extension;
        std::string output_name = base_name + output_extension;

        result = process_file(frame_name, output_name);

        ++current_idx;
    }
}

