#include "InvertNode.h"
#include "imgui.h"
#include "imnodes.h"
#include <opencv2/opencv.hpp>

void InvertNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Invert Node");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(id * 10 + 1);
    ImGui::Text("In");
    ImNodes::EndInputAttribute();

    ImNodes::BeginOutputAttribute(id * 10 + 2);
    ImGui::Text("Out");
    ImNodes::EndOutputAttribute();

    // Add a slider to control inversion strength
    ImGui::SliderFloat("Inversion Strength", &inversionStrength, 0.0f, 1.0f);

    ImNodes::EndNode();
}

cv::Mat InvertNode::process(const std::vector<cv::Mat>& inputs) {
    if (inputs.empty() || inputs[0].empty()) return {};

    cv::Mat output;

    // Apply inversion based on the slider value
    cv::bitwise_not(inputs[0], output);

    // If inversion strength is less than 1, blend the original and inverted images
    if (inversionStrength < 1.0f) {
        cv::Mat blended;
        cv::addWeighted(inputs[0], 1 - inversionStrength, output, inversionStrength, 0, blended);
        return blended;
    }

    return output;
}
