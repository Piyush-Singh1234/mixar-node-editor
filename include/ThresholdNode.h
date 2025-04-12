
#pragma once
#include "Node.h"
#include <opencv2/opencv.hpp>
#include <vector>

class ThresholdNode : public Node {
public:
    enum ThresholdMethod {
        BINARY,
        ADAPTIVE,
        OTSU
    };

    float thresholdValue = 128.0f;      // Default threshold value
    ThresholdMethod method = BINARY;    // Current threshold method
    int blockSize = 3;                  // For adaptive threshold
    float adaptiveConstant = 2.0f;      // For adaptive threshold
    
    std::vector<float> histogramData;   // Store histogram values

    explicit ThresholdNode(int id_) : Node(id_) {}
    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;

private:
    void calculateHistogram(const cv::Mat& image);
};
