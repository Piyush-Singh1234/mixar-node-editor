#include "ColorChannelSplitterNode.h"
#include "imgui.h"
#include "imnodes.h"
#include <iostream>

ColorChannelSplitterNode::ColorChannelSplitterNode(int id_)
    : Node(id_), outputGrayscale(false) {}

void ColorChannelSplitterNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Color Channel Splitter");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(id * 10 + 1);
    ImGui::Text("Image In");
    ImNodes::EndInputAttribute();

    ImNodes::BeginOutputAttribute(id * 10 + 2);
    ImGui::Text("Red");
    ImNodes::EndOutputAttribute();

    ImNodes::BeginOutputAttribute(id * 10 + 3);
    ImGui::Text("Green");
    ImNodes::EndOutputAttribute();

    ImNodes::BeginOutputAttribute(id * 10 + 4);
    ImGui::Text("Blue");
    ImNodes::EndOutputAttribute();

    ImGui::Checkbox("Grayscale Output", &outputGrayscale);

    ImNodes::EndNode();
}

cv::Mat ColorChannelSplitterNode::process(const std::vector<cv::Mat>& inputs) {
    return {};  // not used directly — handled via getChannelOutputs()
}

std::vector<cv::Mat> ColorChannelSplitterNode::getChannelOutputs(const std::vector<cv::Mat>& inputs) {
    std::vector<cv::Mat> outputs;

    if (inputs.empty() || inputs[0].empty())
        return outputs;

    cv::Mat input = inputs[0];
    std::vector<cv::Mat> channels;

    int numChannels = input.channels();
    cv::split(input, channels);

    for (int i = 0; i < std::min(3, (int)channels.size()); ++i) {
        if (outputGrayscale) {
            outputs.push_back(channels[i]);  // 1-channel
        } else {
            std::vector<cv::Mat> merged(3, cv::Mat::zeros(input.size(), CV_8UC1));
            merged[i] = channels[i];
            cv::Mat color;
            cv::merge(merged, color);
            outputs.push_back(color);  // 3-channel
        }
    }

    return outputs;
}
