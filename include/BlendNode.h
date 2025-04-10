#pragma once

#include "Node.h"
#include <opencv2/opencv.hpp>
#include <string>

enum class BlendMode {
    Normal = 0,
    Multiply,
    Screen,
    Overlay,
    Difference
};

class BlendNode : public Node {
public:
    BlendMode mode = BlendMode::Normal;
    float opacity = 0.5f;  // Opacity/mix value (0.0 to 1.0)

    explicit BlendNode(int _id);

    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;

    // Override to match the output attribute used in render()
    int getOutputAttr() const override { return id * 10 + 3; }
};
