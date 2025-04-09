#pragma once
#include "Node.h"

class GrayscaleNode : public Node {
    public:
        float grayLevel = 1.0f;  // 0 = color, 1 = full gray
    
        explicit GrayscaleNode(int id_) : Node(id_) {}
        void render() override;
        cv::Mat process(const std::vector<cv::Mat>& inputs) override;
    };
    