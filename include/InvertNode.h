#pragma once
#include "Node.h"

class InvertNode : public Node {
public:
    explicit InvertNode(int id_) : Node(id_) {}
    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;
};
