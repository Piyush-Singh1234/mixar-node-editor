#include "BlendNode.h"
#include "imgui.h"
#include "imnodes.h"
#include <opencv2/opencv.hpp>
#include <iostream>

BlendNode::BlendNode(int _id) : Node(_id), mode(BlendMode::Normal), opacity(0.5f) {}

void BlendNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Blend Node");
    ImNodes::EndNodeTitleBar();

    // Two input attributes (for two images)
    ImNodes::BeginInputAttribute(id * 10 + 1);
    ImGui::Text("In 1");
    ImNodes::EndInputAttribute();

    ImNodes::BeginInputAttribute(id * 10 + 2);
    ImGui::Text("In 2");
    ImNodes::EndInputAttribute();

    // Blend mode dropdown
    const char* blendModeNames[] = { "Normal", "Multiply", "Screen", "Overlay", "Difference" };
    int currentMode = static_cast<int>(mode);
    if (ImGui::Combo("Blend Mode", &currentMode, blendModeNames, IM_ARRAYSIZE(blendModeNames))) {
        mode = static_cast<BlendMode>(currentMode);
    }

    // Opacity slider
    ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f);

    // Use id*10+3 for output to match our overridden getOutputAttr()
    ImNodes::BeginOutputAttribute(id * 10 + 3);
    ImGui::Text("Out");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
}

cv::Mat BlendNode::process(const std::vector<cv::Mat>& inputs) {
    if (inputs.size() < 2 || inputs[0].empty() || inputs[1].empty()) {
        std::cout << "[ERROR] Not enough input images for blending." << std::endl;
        return {};
    }

    cv::Mat img1 = inputs[0];
    cv::Mat img2 = inputs[1];

    // If the images don't match in size, automatically resize img2 to match img1.
    if (img1.size() != img2.size()) {
        std::cout << "[INFO] Resizing second image to match first image's dimensions." << std::endl;
        cv::resize(img2, img2, img1.size());
    }

    cv::Mat result;
    float a = opacity; // blending factor

    switch (mode) {
        case BlendMode::Normal:
            cv::addWeighted(img1, a, img2, 1.0f - a, 0.0, result);
            break;
        case BlendMode::Multiply:
            cv::multiply(img1, img2, result);
            break;
        case BlendMode::Screen: {
            cv::Mat inv1, inv2;
            cv::bitwise_not(img1, inv1);
            cv::bitwise_not(img2, inv2);
            cv::multiply(inv1, inv2, result);
            cv::bitwise_not(result, result);
            break;
        }
        case BlendMode::Overlay: {
            // A simplified overlay implementation (pixel-by-pixel)
            result = cv::Mat::zeros(img1.size(), img1.type());
            for (int y = 0; y < img1.rows; ++y) {
                for (int x = 0; x < img1.cols; ++x) {
                    for (int c = 0; c < img1.channels(); ++c) {
                        uchar p1 = img1.at<cv::Vec3b>(y, x)[c];
                        uchar p2 = img2.at<cv::Vec3b>(y, x)[c];
                        float blended;
                        if (p1 < 128)
                            blended = 2 * p1 * p2 / 255.0f;
                        else
                            blended = 255 - 2 * (255 - p1) * (255 - p2) / 255.0f;
                        result.at<cv::Vec3b>(y, x)[c] = cv::saturate_cast<uchar>(blended);
                    }
                }
            }
            break;
        }
        case BlendMode::Difference:
            cv::absdiff(img1, img2, result);
            break;
    }
    return result;
}
