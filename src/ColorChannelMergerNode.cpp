#include "ColorChannelMergerNode.h"
#include "imgui.h"
#include "imnodes.h"
#include <opencv2/opencv.hpp>

void ColorChannelMergerNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Color Channel Merger");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(id * 10 + 1);
    ImGui::Text("Red");
    ImNodes::EndInputAttribute();

    ImNodes::BeginInputAttribute(id * 10 + 2);
    ImGui::Text("Green");
    ImNodes::EndInputAttribute();

    ImNodes::BeginInputAttribute(id * 10 + 3);
    ImGui::Text("Blue");
    ImNodes::EndInputAttribute();

    ImNodes::BeginOutputAttribute(id * 10 + 4);
    ImGui::Text("Out");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
}

cv::Mat ColorChannelMergerNode::process(const std::vector<cv::Mat>& inputs) {
    std::cout << "[Merger] Inputs received: " << inputs.size() << "\n";

    if (inputs.size() < 3 || inputs[0].empty() || inputs[1].empty() || inputs[2].empty()) {
        std::cout << "[Merger] Missing or empty channels.\n";
        return {};
    }

    std::vector<cv::Mat> channels = { inputs[2], inputs[1], inputs[0] }; // BGR
    cv::merge(channels, outputImage);
    std::cout << "[Merger] Successfully merged. Size: " << outputImage.cols << "x" << outputImage.rows << "\n";
    return outputImage;
}

