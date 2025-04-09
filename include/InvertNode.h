#pragma once
#include "Node.h"
#include <opencv2/opencv.hpp>
#include <vector>

class InvertNode : public Node {
public:
    float inversionStrength = 1.0f;

    explicit InvertNode(int id_) : Node(id_) {}
    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;
};
