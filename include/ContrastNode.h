#pragma once
#include "Node.h"

class ContrastNode : public Node {
public:
    float contrast = 1.0f;

    explicit ContrastNode(int id_) : Node(id_) {}
    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;
};
