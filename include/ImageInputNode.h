// #pragma once

// #include "Node.h"
// #include <string>

// class ImageInputNode : public Node {
//     public:
//         std::string path;
//         cv::Mat image;
//         char pathBuffer[256];  // <-- add this
    
//         explicit ImageInputNode(int _id);
//         void render() override;
//         cv::Mat process(const std::vector<cv::Mat>&) override;
//     };
    
#pragma once

#include "Node.h"
#include <string>

class ImageInputNode : public Node {
public:
    std::string path;
    cv::Mat image;

    explicit ImageInputNode(int _id);
    void render() override;
    cv::Mat process(const std::vector<cv::Mat>&) override;
};