#include "SharpenNode.h"
#include "imgui.h"
#include "imnodes.h"
#include <opencv2/opencv.hpp>

void SharpenNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Sharpen Node");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(id * 10 + 1);
    ImGui::Text("In");
    ImNodes::EndInputAttribute();

    ImNodes::BeginOutputAttribute(id * 10 + 2);
    ImGui::Text("Out");
    ImNodes::EndOutputAttribute();

    // Add a slider to control sharpen strength
    ImGui::SliderFloat("Sharpen Strength", &sharpenStrength, 0.0f, 3.0f);

    ImNodes::EndNode();
}

cv::Mat SharpenNode::process(const std::vector<cv::Mat>& inputs) {
    if (inputs.empty() || inputs[0].empty()) return {};

    cv::Mat output;
    
    // Define a sharpening kernel
    cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
        0, -1, 0,
        -1, 5, -1,
        0, -1, 0);

    // Apply sharpening filter to the input image
    cv::filter2D(inputs[0], output, -1, kernel);
    
    // If sharpening strength is less than 1, blend the original and sharpened images
    if (sharpenStrength < 1.0f) {
        cv::Mat blended;
        cv::addWeighted(inputs[0], 1 - sharpenStrength, output, sharpenStrength, 0, blended);
        return blended;
    }

    return output;
}
