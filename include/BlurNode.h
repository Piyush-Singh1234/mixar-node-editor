#pragma once
#include "Node.h"

class BlurNode : public Node {
public:
    int blurAmount = 5;  // kernel size, must be odd

    explicit BlurNode(int id_) : Node(id_) {}
    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;
};
