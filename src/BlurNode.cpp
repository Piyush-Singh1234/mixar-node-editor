#include "BlurNode.h"
#include "imgui.h"
#include "imnodes.h"
#include <iostream>

void BlurNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Blur Node");
    ImNodes::EndNodeTitleBar();

    // Input attribute
    ImNodes::BeginInputAttribute(id * 10 + 1);
    ImGui::Text("In");
    ImNodes::EndInputAttribute();

    // Blur control
    ImGui::SliderInt("Kernel Size", &blurAmount, 1, 25);
    if (blurAmount % 2 == 0) blurAmount++; // kernel must be odd

    // Output attribute
    ImNodes::BeginOutputAttribute(id * 10 + 2);
    ImGui::Text("Out");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
}


cv::Mat BlurNode::process(const std::vector<cv::Mat>& inputs) {
    if (inputs.empty() || inputs[0].empty()) return {};

    cv::Mat output;
    int kernel = std::max(1, blurAmount | 1);  // ensure odd
    std::cout << "[BlurNode] Applying GaussianBlur with kernel size: " << kernel << "\n";
    cv::GaussianBlur(inputs[0], output, cv::Size(kernel, kernel), 0);
    return output;
}

