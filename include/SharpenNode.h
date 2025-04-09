#pragma once
#include "Node.h"
#include <opencv2/opencv.hpp>
#include <vector>

class SharpenNode : public Node {
public:
    float sharpenStrength = 1.0f;

    explicit SharpenNode(int id_) : Node(id_) {}
    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;
};
