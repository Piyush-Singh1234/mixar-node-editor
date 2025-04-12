
#pragma once

#include "Node.h"
#include <opencv2/opencv.hpp>
#include <vector>

class ColorChannelSplitterNode : public Node {
public:
    explicit ColorChannelSplitterNode(int id_);
    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;
    std::vector<int> getInputAttributes() const override;
    std::vector<int> getOutputAttrs() const;

    // Retrieve output image by attribute
    cv::Mat getOutputByAttr(int attr) const;

private:
    bool grayscaleOutput;
    std::vector<cv::Mat> outputs;
};
