#pragma once
#include "Node.h"
#include <opencv2/opencv.hpp>

class EdgeDetectionNode : public Node {
public:
    enum class Method { Sobel, Canny };
    Method method;

    int sobelKernel;
    int cannyThresh1;
    int cannyThresh2;
    bool overlayEdges;

    explicit EdgeDetectionNode(int id_);

    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;
};
