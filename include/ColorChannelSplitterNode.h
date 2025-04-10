#pragma once
#include "Node.h"

class ColorChannelSplitterNode : public Node {
public:
    cv::Mat r, g, b;

    explicit ColorChannelSplitterNode(int id_) : Node(id_) {}
    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;

    int getOutputAttrR() const { return id * 10 + 2; }
    int getOutputAttrG() const { return id * 10 + 3; }
    int getOutputAttrB() const { return id * 10 + 4; }
};
