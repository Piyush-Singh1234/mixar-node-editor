

#include "ColorChannelSplitterNode.h"
#include "imgui.h"
#include "imnodes.h"
#include <opencv2/opencv.hpp>
#include <iostream>

ColorChannelSplitterNode::ColorChannelSplitterNode(int id_)
    : Node(id_), grayscaleOutput(false) {}

void ColorChannelSplitterNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Color Channel Splitter");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(id * 10 + 1);
    ImGui::Text("In");
    ImNodes::EndInputAttribute();

    ImGui::Checkbox("Grayscale Output", &grayscaleOutput);

    ImNodes::BeginOutputAttribute(id * 10 + 2);
    ImGui::Text("Blue");
    ImNodes::EndOutputAttribute();

    ImNodes::BeginOutputAttribute(id * 10 + 3);
    ImGui::Text("Green");
    ImNodes::EndOutputAttribute();

    ImNodes::BeginOutputAttribute(id * 10 + 4);
    ImGui::Text("Red");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
}

cv::Mat ColorChannelSplitterNode::process(const std::vector<cv::Mat>& inputs) {
    outputs.clear();

    if (inputs.empty() || inputs[0].empty()) {
        std::cout << "[SplitterNode] Empty input\n";
        outputs = { {}, {}, {} };
        return {};
    }

    std::vector<cv::Mat> channels;
    cv::split(inputs[0], channels);

    
    // OpenCV: channels[0]=Blue, [1]=Green, [2]=Red
    std::vector<int> bgr_to_rgb = {2, 1, 0};

    for (int i = 0; i < 3; ++i) {
        int ch = bgr_to_rgb[i]; // correct channel mapping
        if (grayscaleOutput) {
            outputs.push_back(channels[ch]);
        } else {
            std::vector<cv::Mat> merged(3, cv::Mat::zeros(channels[ch].size(), CV_8UC1));
            merged[i] = channels[ch];  // preserve output position: R, G, B
            cv::Mat out;
            cv::merge(merged, out);
            outputs.push_back(out);
        }
    }


    return {}; // Main output unused
}

cv::Mat ColorChannelSplitterNode::getOutputByAttr(int attr) const {
    int base = id * 10 + 2;
    int index = attr - base;
    if (index >= 0 && index < 3) return outputs[index];
    return {};
}

std::vector<int> ColorChannelSplitterNode::getInputAttributes() const {
    return { id * 10 + 1 };
}

std::vector<int> ColorChannelSplitterNode::getOutputAttrs() const {
    return { id * 10 + 2, id * 10 + 3, id * 10 + 4 };
}
