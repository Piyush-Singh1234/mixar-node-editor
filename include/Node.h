#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

class Node {
    public:
        int id;
        std::string name;  // Add a name attribute to store the node's name
    
        explicit Node(int _id, const std::string& _name = "") : id(_id), name(_name) {}
    
        virtual void render() = 0;
        virtual cv::Mat process(const std::vector<cv::Mat>& inputs) = 0;
    
        virtual int getInputAttr() const { return id * 10 + 1; }
        virtual int getOutputAttr() const { return id * 10 + 2; }
    
        int getId() const { return id; }
    
        virtual ~Node() = default;
    };
    