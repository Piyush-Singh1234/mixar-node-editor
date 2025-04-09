#pragma once
#include "Node.h"
#include <opencv2/opencv.hpp>
#include <vector>

class ThresholdNode : public Node {
public:
    float thresholdValue = 128.0f;  // Intensity threshold for binarization

    explicit ThresholdNode(int id_) : Node(id_) {}
    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;
};
