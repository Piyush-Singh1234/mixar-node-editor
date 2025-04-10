#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

class Node {
public:
    int id;

    explicit Node(int _id) : id(_id) {}

    virtual void render() = 0;
    virtual cv::Mat process(const std::vector<cv::Mat>& inputs) = 0;

    // ✅ Add these methods — required for linking
    virtual int getInputAttr() const { return id * 10 + 1; }
    virtual int getOutputAttr() const { return id * 10 + 2; }

    // Add getId() method
    int getId() const { return id; }

    virtual ~Node() = default;
};
