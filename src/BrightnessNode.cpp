#include "BrightnessNode.h"
#include "imgui.h"
#include "imnodes.h"
BrightnessNode::BrightnessNode(int _id) : Node(_id), brightness(0) {}

void BrightnessNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Brightness Node");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(id * 10 + 1);
    ImGui::Text("In");
    ImNodes::EndInputAttribute();

    ImGui::SliderInt("Brightness", &brightness, -100, 100);

    ImNodes::BeginOutputAttribute(id * 10 + 2);
    ImGui::Text("Out");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
}


cv::Mat BrightnessNode::process(const std::vector<cv::Mat>& inputs) {
    if (inputs.empty()) return {};
    cv::Mat output;
    inputs[0].convertTo(output, -1, 1, brightness);
    return output;
}
