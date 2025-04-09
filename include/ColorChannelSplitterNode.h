#pragma once
#include "Node.h"
#include <opencv2/opencv.hpp>
#include <vector>

class ColorChannelSplitterNode : public Node {
public:
    bool splitAlpha = true;  // Option to split Alpha channel (for RGBA images)
    bool grayscaleOutput = false;  // Option to output grayscale representation of each channel

    explicit ColorChannelSplitterNode(int id_) : Node(id_) {}
    void render() override;
    std::vector<cv::Mat> process(const std::vector<cv::Mat>& inputs) override;
};
