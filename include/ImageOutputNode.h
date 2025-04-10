#pragma once
#include "Node.h"
#include <opencv2/opencv.hpp>

class ImageOutputNode : public Node {
public:
    unsigned int textureID = 0;
    cv::Mat lastImage;

    explicit ImageOutputNode(int id_);
    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;
};