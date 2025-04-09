#pragma once

#include "Node.h"

class BrightnessNode : public Node {
public:
    int brightness;
    explicit BrightnessNode(int _id);
    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;
};
