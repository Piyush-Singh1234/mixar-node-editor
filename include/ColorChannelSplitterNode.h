#pragma once
#include "Node.h"
#include <opencv2/opencv.hpp>
#include <vector>

class ColorChannelSplitterNode : public Node {
public:
    explicit ColorChannelSplitterNode(int id_);

    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;  // required override
    std::vector<cv::Mat> getChannelOutputs(const std::vector<cv::Mat>& inputs);

private:
    bool outputGrayscale;
};
