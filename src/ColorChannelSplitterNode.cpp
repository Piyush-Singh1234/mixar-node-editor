// #include "ColorChannelSplitterNode.h"
// #include "imgui.h"
// #include "imnodes.h"
// #include <iostream>
// #include <opencv2/opencv.hpp>
// #include <algorithm>
// #include <filesystem>
// namespace fs = std::filesystem;

// ColorChannelSplitterNode::ColorChannelSplitterNode(int id_)
//     : Node(id_), outputGrayscale(false) {}

// void ColorChannelSplitterNode::render() {
//     ImNodes::BeginNode(id);
//     ImNodes::BeginNodeTitleBar();
//     ImGui::Text("Color Channel Splitter");
//     ImNodes::EndNodeTitleBar();

//     // Single input attribute for the image.
//     ImNodes::BeginInputAttribute(id * 10 + 1);
//     ImGui::Text("Image In");
//     ImNodes::EndInputAttribute();

//     // Three output attributes, labeled as Red, Green, and Blue.
//     ImNodes::BeginOutputAttribute(id * 10 + 2);
//     ImGui::Text("Red");
//     ImNodes::EndOutputAttribute();

//     ImNodes::BeginOutputAttribute(id * 10 + 3);
//     ImGui::Text("Green");
//     ImNodes::EndOutputAttribute();

//     ImNodes::BeginOutputAttribute(id * 10 + 4);
//     ImGui::Text("Blue");
//     ImNodes::EndOutputAttribute();

//     ImGui::Checkbox("Grayscale Output", &outputGrayscale);

//     ImNodes::EndNode();
// }

// // In the process() override, we now return the first channel output (Red) by default.
// cv::Mat ColorChannelSplitterNode::process(const std::vector<cv::Mat>& inputs) {
//     std::vector<cv::Mat> outs = getChannelOutputs(inputs);
//     if(outs.empty()){
//         std::cerr << "[ERROR] ColorChannelSplitter: No channels obtained." << std::endl;
//         return {};
//     }
//     std::cout << "[DEBUG] ColorChannelSplitter: Returning 'Red' channel (output index 0) as default output." << std::endl;
//     return outs[0];
// }

// // This method splits the image into BGR channels, reorders them to output as Red, Green, Blue.
// std::vector<cv::Mat> ColorChannelSplitterNode::getChannelOutputs(const std::vector<cv::Mat>& inputs) {
//     std::vector<cv::Mat> outputs;

//     if (inputs.empty() || inputs[0].empty()) {
//         std::cerr << "[ERROR] ColorChannelSplitter: Input image is empty." << std::endl;
//         return outputs;
//     }

//     cv::Mat input = inputs[0];
//     std::vector<cv::Mat> channels;
//     cv::split(input, channels);

//     // For an RGB display, we need 3 channels.
//     if (channels.size() < 3) {
//         std::cerr << "[ERROR] ColorChannelSplitter: Expected at least 3 channels, got " << channels.size() << std::endl;
//         return outputs;
//     }

//     std::cout << "[DEBUG] ColorChannelSplitter: Input image size: " << input.cols 
//               << "x" << input.rows << ", channels: " << channels.size() << std::endl;

//     // Since OpenCV loads BGR, reorder the channels to present them as Red, Green, Blue:
//     //    Output 0: Red   (which is channels[2])
//     //    Output 1: Green (which is channels[1])
//     //    Output 2: Blue  (which is channels[0])
//     int order[3] = {2, 1, 0};

//     for (int j = 0; j < 3; ++j) {
//         int chIndex = order[j];
//         if (outputGrayscale) {
//             // For grayscale, simply return the 1-channel image.
//             outputs.push_back(channels[chIndex]);
//         } else {
//             // Merge into a 3-channel image where only the desired channel has the data.
//             std::vector<cv::Mat> merged(3, cv::Mat::zeros(input.size(), CV_8UC1));
//             // Place the channel data in its correct position for color display.
//             // For display, OpenCV expects BGR order:
//             //    Blue is index 0, Green is index 1, and Red is index 2.
//             // So, if we want the output to be "Red", then we want the red data at index 2.
//             // Since our order array is already [2, 1, 0], we can simply assign:
//             merged[order[j]] = channels[chIndex];
//             cv::Mat color;
//             cv::merge(merged, color);
//             outputs.push_back(color);
//         }
//     }

//     std::cout << "[DEBUG] ColorChannelSplitter: Generated " << outputs.size() << " channel outputs." << std::endl;
//     return outputs;
// }



#include "ColorChannelSplitterNode.h"
#include "imgui.h"
#include "imnodes.h"
#include <opencv2/opencv.hpp>
#include <iostream>

ColorChannelSplitterNode::ColorChannelSplitterNode(int id_)
    : Node(id_), grayscaleOutput(false) {}

void ColorChannelSplitterNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Color Channel Splitter");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(id * 10 + 1);
    ImGui::Text("In");
    ImNodes::EndInputAttribute();

    ImGui::Checkbox("Grayscale Output", &grayscaleOutput);

    ImNodes::BeginOutputAttribute(id * 10 + 2);
    ImGui::Text("Blue");
    ImNodes::EndOutputAttribute();

    ImNodes::BeginOutputAttribute(id * 10 + 3);
    ImGui::Text("Green");
    ImNodes::EndOutputAttribute();

    ImNodes::BeginOutputAttribute(id * 10 + 4);
    ImGui::Text("Red");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
}

cv::Mat ColorChannelSplitterNode::process(const std::vector<cv::Mat>& inputs) {
    outputs.clear();

    if (inputs.empty() || inputs[0].empty()) {
        std::cout << "[SplitterNode] Empty input\n";
        outputs = { {}, {}, {} };
        return {};
    }

    std::vector<cv::Mat> channels;
    cv::split(inputs[0], channels);

    // for (int i = 0; i < 3; ++i) {
    //     if (grayscaleOutput) {
    //         outputs.push_back(channels[i]); // 1-channel
    //     } else {
    //         std::vector<cv::Mat> merged(3, cv::Mat::zeros(channels[i].size(), CV_8UC1));
    //         merged[i] = channels[i];
    //         cv::Mat out;
    //         cv::merge(merged, out);
    //         outputs.push_back(out);
    //     }
    // }
    // OpenCV: channels[0]=Blue, [1]=Green, [2]=Red
    std::vector<int> bgr_to_rgb = {2, 1, 0};

    for (int i = 0; i < 3; ++i) {
        int ch = bgr_to_rgb[i]; // correct channel mapping
        if (grayscaleOutput) {
            outputs.push_back(channels[ch]);
        } else {
            std::vector<cv::Mat> merged(3, cv::Mat::zeros(channels[ch].size(), CV_8UC1));
            merged[i] = channels[ch];  // preserve output position: R, G, B
            cv::Mat out;
            cv::merge(merged, out);
            outputs.push_back(out);
        }
    }


    return {}; // Main output unused
}

cv::Mat ColorChannelSplitterNode::getOutputByAttr(int attr) const {
    int base = id * 10 + 2;
    int index = attr - base;
    if (index >= 0 && index < 3) return outputs[index];
    return {};
}

std::vector<int> ColorChannelSplitterNode::getInputAttributes() const {
    return { id * 10 + 1 };
}

std::vector<int> ColorChannelSplitterNode::getOutputAttrs() const {
    return { id * 10 + 2, id * 10 + 3, id * 10 + 4 };
}
