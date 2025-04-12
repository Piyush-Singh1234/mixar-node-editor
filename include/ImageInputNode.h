#pragma once

#include "Node.h"
#include <string>

class ImageInputNode : public Node {
    public:
        ImageInputNode(int id_);
        virtual void render() override;
        virtual cv::Mat process(const std::vector<cv::Mat>&) override;
    private:
        std::string path;             // instance variable unique to this node
        cv::Mat image;                // instance variable for storing the loaded image
        char buffer[256];             // instance-specific buffer (each node gets its own copy)\n\n    // Alternatively, you could remove the char buffer entirely and use path directly\n};
};    