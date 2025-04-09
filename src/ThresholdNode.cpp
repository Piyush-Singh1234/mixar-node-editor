#include "ThresholdNode.h"
#include "imgui.h"
#include "imnodes.h"
#include <opencv2/opencv.hpp>

void ThresholdNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Threshold Node");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(id * 10 + 1);
    ImGui::Text("In");
    ImNodes::EndInputAttribute();

    ImNodes::BeginOutputAttribute(id * 10 + 2);
    ImGui::Text("Out");
    ImNodes::EndOutputAttribute();

    // Slider to control the threshold value
    ImGui::SliderFloat("Threshold", &thresholdValue, 0.0f, 255.0f);

    ImNodes::EndNode();
}

cv::Mat ThresholdNode::process(const std::vector<cv::Mat>& inputs) {
    if (inputs.empty() || inputs[0].empty()) return {};

    cv::Mat gray, binary;
    cv::cvtColor(inputs[0], gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, binary, thresholdValue, 255, cv::THRESH_BINARY);
    return binary;
}
