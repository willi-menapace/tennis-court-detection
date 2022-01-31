//
// Created by Chlebus, Grzegorz on 28.08.17.
// Copyright (c) Chlebus, Grzegorz. All rights reserved.
//
#include "TennisCourtFitter.h"
#include "GlobalParameters.h"
#include "TimeMeasurement.h"
#include "DebugHelpers.h"
#include "geometry.h"


using namespace cv;

bool TennisCourtFitter::debug = false;
const std::string TennisCourtFitter::windowName = "TennisCourtFitter";

TennisCourtFitter::Parameters::Parameters() {

}

TennisCourtFitter::TennisCourtFitter()
        : TennisCourtFitter(Parameters()) {

}


TennisCourtFitter::TennisCourtFitter(TennisCourtFitter::Parameters p)
        : parameters(p) {

}

TennisCourtModel TennisCourtFitter::run(const std::vector<Line> &lines, const Mat &binaryImage,
                                        const Mat &rgbImage) {
    TimeMeasurement::start("TennisCourtFitter::run");

    int rows = binaryImage.rows;
    int cols = binaryImage.cols;
    float minExclusionRow = rows * 0.4196;
    float maxExclusionRow = rows * 0.60714;
    Point2f minExclusionPoint(cols / 2, minExclusionRow);
    Point2f maxExclusionPoint(cols / 2, maxExclusionRow);

    std::vector<Line> hLines, vLines;
    getHorizontalAndVerticalLines(lines, hLines, vLines, rgbImage);
    sortHorizontalLines(hLines, rgbImage);
    sortVerticalLines(vLines, rgbImage);

    std::vector<Line> filteredHLines;
    for(Line currentLine : hLines) {
        Point2f closestPointToMin = currentLine.getPointOnLineClosestTo(minExclusionPoint);
        Point2f closestPointToMax = currentLine.getPointOnLineClosestTo(maxExclusionPoint);

        // If the line is not in the forbidden region
        if(!(currentLine.getPoint().y > minExclusionRow && currentLine.getPoint().y < maxExclusionRow)) {

            filteredHLines.push_back(currentLine);
        } else {
            std::cout << "Filtered a line" << std::endl;
            std::cout << currentLine.getPoint().x << " " << currentLine.getPoint().y << std::endl;
        }
    }

    hLinePairs = TennisCourtModel::getPossibleLinePairs(filteredHLines);
    vLinePairs = TennisCourtModel::getPossibleLinePairs(vLines);

    if (debug) {
        std::cout << "Horizontal line pairs = " << hLinePairs.size() << std::endl;
        std::cout << "Vertical line pairs = " << vLinePairs.size() << std::endl;
    }

    // TODO
    if (hLinePairs.size() < 2 || vLinePairs.size() < 2) {
        throw std::runtime_error("Not enough line candidates were found.");
    }

    TimeMeasurement::start("\tfindBestModelFit");
    findBestModelFit(binaryImage, rgbImage);
    TimeMeasurement::stop("\tfindBestModelFit");

    TimeMeasurement::stop("TennisCourtFitter::run");

    return bestModel;
}


void TennisCourtFitter::getHorizontalAndVerticalLines(const std::vector<Line> &lines,
                                                      std::vector<Line> &hLines, std::vector<Line> &vLines,
                                                      const cv::Mat &rgbImage) {
    for (auto &line: lines) {
        if (line.isVertical()) {
            vLines.push_back(line);
        } else {
            hLines.push_back(line);
        }
    }

    if (debug) {
        std::cout << "Horizontal lines = " << hLines.size() << std::endl;
        std::cout << "Vertical lines = " << vLines.size() << std::endl;
        Mat image = rgbImage.clone();
        drawLines(hLines, image, Scalar(255, 0, 0));
        drawLines(vLines, image, Scalar(0, 255, 0));
        displayImage(windowName, image);
    }
}


void TennisCourtFitter::sortHorizontalLines(std::vector<Line> &hLines, const cv::Mat &rgbImage) {
    float x = rgbImage.cols / 2.0;
    sortLinesByDistanceToPoint(hLines, Point2f(x, 0));

    if (false) {
        for (auto &line: hLines) {
            Mat image = rgbImage.clone();
            drawLine(line, image, Scalar(255, 0, 0));
            displayImage(windowName, image);
        }
    }
}

void TennisCourtFitter::sortVerticalLines(std::vector<Line> &vLines, const cv::Mat &rgbImage) {
    float y = rgbImage.rows / 2.0;
    sortLinesByDistanceToPoint(vLines, Point2f(0, y));

    if (false) {
        for (auto &line: vLines) {
            Mat image = rgbImage.clone();
            drawLine(line, image, Scalar(0, 255, 0));
            displayImage(windowName, image);
        }
    }
}


void TennisCourtFitter::findBestModelFit(const cv::Mat &binaryImage, const cv::Mat &rgbImage) {
    float bestScore = GlobalParameters().initialFitScore;
    for (auto &hLinePair: hLinePairs) {
        for (auto &vLinePair: vLinePairs) {
            TennisCourtModel model;
            float score = model.fit(hLinePair, vLinePair, binaryImage, rgbImage);
            if (score > bestScore) {
                bestScore = score;
                bestModel = model;
            }
        }
    }

    if (debug) {
        std::cout << "Best model score = " << bestScore << std::endl;
        Mat image = rgbImage.clone();
        bestModel.drawModel(image);
        displayImage(windowName, image);
    }
}
