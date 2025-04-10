
#include "ColorChannelSplitterNode.h"
#include "imgui.h"
#include "imnodes.h"
#include <opencv2/opencv.hpp>

void ColorChannelSplitterNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Color Channel Splitter");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(id * 10 + 1);
    ImGui::Text("In");
    ImNodes::EndInputAttribute();

    ImNodes::BeginOutputAttribute(getOutputAttrR());
    ImGui::Text("Red");
    ImNodes::EndOutputAttribute();

    ImNodes::BeginOutputAttribute(getOutputAttrG());
    ImGui::Text("Green");
    ImNodes::EndOutputAttribute();

    ImNodes::BeginOutputAttribute(getOutputAttrB());
    ImGui::Text("Blue");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
}

cv::Mat ColorChannelSplitterNode::process(const std::vector<cv::Mat>& inputs) {
    if (inputs.empty() || inputs[0].empty()) return {};

    std::vector<cv::Mat> channels;
    cv::split(inputs[0], channels);  // BGR

    b = channels[0];
    g = channels[1];
    r = channels[2];

    return {};  // not used directly — use individual outputs below
}
