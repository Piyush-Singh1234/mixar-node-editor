#pragma once
#include "Node.h"
#include <opencv2/opencv.hpp>
#include <vector>

class RotateNode : public Node {
public:
    float angle = 0.0f;  // Angle to rotate the image by

    explicit RotateNode(int id_) : Node(id_) {}
    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;
};
