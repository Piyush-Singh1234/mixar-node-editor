#pragma once
#include "Node.h"
#include <opencv2/opencv.hpp>

class BlendNode : public Node {
public:
    enum class BlendMode { Normal, Multiply, Screen, Overlay, Difference };
    BlendMode mode;
    float opacity;

    explicit BlendNode(int id_);
    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;
};
