#include "GrayscaleNode.h"
#include "imgui.h"
#include "imnodes.h"
#include <opencv2/opencv.hpp>
#include <iostream>


void GrayscaleNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Grayscale Node");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(id * 10 + 1);
    ImGui::Text("In");
    ImNodes::EndInputAttribute();

    // ✅ Add slider to control gray intensity
    ImGui::SliderFloat("Gray Level", &grayLevel, 0.0f, 1.0f);

    ImNodes::BeginOutputAttribute(id * 10 + 2);
    ImGui::Text("Out");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
}



cv::Mat GrayscaleNode::process(const std::vector<cv::Mat>& inputs) {
    if (inputs.empty() || inputs[0].empty()) return {};

    cv::Mat input = inputs[0];
    cv::Mat gray, gray_bgr, blended;

    // Convert to grayscale
    cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    cv::cvtColor(gray, gray_bgr, cv::COLOR_GRAY2BGR);

    // Linear blend: (1 - grayLevel) * color + grayLevel * grayscale
    cv::addWeighted(input, 1.0f - grayLevel, gray_bgr, grayLevel, 0, blended);

    return blended;
}
