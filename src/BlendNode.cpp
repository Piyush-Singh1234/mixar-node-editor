#include "BlendNode.h"
#include "imgui.h"
#include "imnodes.h"
#include <opencv2/opencv.hpp>
#include <iostream>

BlendNode::BlendNode(int _id) : Node(_id) {}

void BlendNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Blend Node");
    ImNodes::EndNodeTitleBar();

    // Two input attributes for the two images
    ImNodes::BeginInputAttribute(id * 10 + 1);
    ImGui::Text("In 1");
    ImNodes::EndInputAttribute();

    ImNodes::BeginInputAttribute(id * 10 + 2);
    ImGui::Text("In 2");
    ImNodes::EndInputAttribute();

    // Main opacity slider (for Normal blend, controls overall mix)
    ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f);

    // Blend mode selection
    const char* blendModeNames[] = { "Normal", "Multiply", "Screen", "Overlay", "Difference" };
    int currentMode = static_cast<int>(mode);
    if (ImGui::Combo("Blend Mode", &currentMode, blendModeNames, IM_ARRAYSIZE(blendModeNames))) {
        mode = static_cast<BlendMode>(currentMode);
    }

    // Additional sliders for non-Normal modes:
    switch(mode) {
        case BlendMode::Multiply:
            ImGui::SliderFloat("Multiply Intensity", &multiplyIntensity, 0.0f, 2.0f);
            break;
        case BlendMode::Screen:
            ImGui::SliderFloat("Screen Intensity", &screenIntensity, 0.0f, 2.0f);
            break;
        case BlendMode::Overlay:
            ImGui::SliderFloat("Overlay Intensity", &overlayIntensity, 0.0f, 2.0f);
            break;
        case BlendMode::Difference:
            ImGui::SliderFloat("Difference Threshold", &differenceThreshold, 0.0f, 1.0f);
            break;
        default:
            break;
    }

    // Output attribute
    ImNodes::BeginOutputAttribute(id * 10 + 3);
    ImGui::Text("Out");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
}

cv::Mat BlendNode::process(const std::vector<cv::Mat>& inputs) {
    if (!validateInputs(inputs)) {
        std::cout << "[BlendNode] Invalid input images!" << std::endl;
        return {};
    }

    cv::Mat img1 = inputs[0];
    cv::Mat img2 = inputs[1];

    // Ensure images are the same size; if not, resize img2 to match img1.
    if (img1.size() != img2.size()) {
        std::cout << "[BlendNode] Resizing second image to match the first." << std::endl;
        cv::resize(img2, img2, img1.size());
    }

    cv::Mat result;
    float a = opacity;  // Base opacity

    switch (mode) {
        case BlendMode::Normal:
            cv::addWeighted(img1, a, img2, 1.0f - a, 0.0, result);
            break;
        case BlendMode::Multiply:
            // Multiply each pixel by the intensity slider, then combine with a weighted average.
            cv::multiply(img1, img2, result);
            result.convertTo(result, -1, multiplyIntensity);
            cv::addWeighted(result, a, img1, 1.0f - a, 0.0, result);
            break;
        case BlendMode::Screen:
            // Screen: invert, multiply, invert; then adjust with intensity.
            {
                cv::Mat inv1, inv2;
                cv::bitwise_not(img1, inv1);
                cv::bitwise_not(img2, inv2);
                cv::multiply(inv1, inv2, result);
                cv::bitwise_not(result, result);
                result.convertTo(result, -1, screenIntensity);
                cv::addWeighted(result, a, img1, 1.0f - a, 0.0, result);
            }
            break;
        case BlendMode::Overlay:
            // Overlay: a combination of multiply and screen; adjust with intensity.
            {
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
                            result.at<cv::Vec3b>(y, x)[c] = cv::saturate_cast<uchar>(overlayIntensity * blended + (1.0f - overlayIntensity) * p1);
                        }
                    }
                }
                cv::addWeighted(result, a, img1, 1.0f - a, 0.0, result);
            }
            break;
        case BlendMode::Difference:
            cv::absdiff(img1, img2, result);
            result.convertTo(result, -1, differenceThreshold);
            cv::addWeighted(result, a, img1, 1.0f - a, 0.0, result);
            break;
        default:
            result = img1;
            break;
    }

    return result;
}
