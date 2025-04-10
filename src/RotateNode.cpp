#include "RotateNode.h"
#include "imgui.h"
#include "imnodes.h"
#include <opencv2/opencv.hpp>

void RotateNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Rotate Node");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(id * 10 + 1);
    ImGui::Text("In");
    ImNodes::EndInputAttribute();

    ImNodes::BeginOutputAttribute(id * 10 + 2);
    ImGui::Text("Out");
    ImNodes::EndOutputAttribute();

    ImGui::SliderFloat("Angle", &angle, 0.0f, 360.0f);
    ImNodes::EndNode();
}

cv::Mat RotateNode::process(const std::vector<cv::Mat>& inputs) {
    if (inputs.empty() || inputs[0].empty()) return {};

    cv::Mat output;
    cv::Point2f center(inputs[0].cols / 2.0f, inputs[0].rows / 2.0f);
    cv::Mat rotation_matrix = cv::getRotationMatrix2D(center, angle, 1.0); // Rotation matrix
    cv::warpAffine(inputs[0], output, rotation_matrix, inputs[0].size());  // Apply rotation

    return output;
}