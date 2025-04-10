#pragma once
#include "Node.h"

class ColorChannelMergerNode : public Node {
public:
    cv::Mat outputImage;
    explicit ColorChannelMergerNode(int id_) : Node(id_) {}
    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;
};
