#include "ConvolutionFilterNode.h"
#include "imgui.h"
#include "imnodes.h"
#include <opencv2/opencv.hpp>

ConvolutionFilterNode::ConvolutionFilterNode(int id)
    : Node(id, "Convolution Filter") {
    createStandardKernels();
    kernel.fill({0});
    kernel[1][1] = 1.0f; // Default identity kernel
}

void ConvolutionFilterNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Convolution Filter");
    ImNodes::EndNodeTitleBar();

    // Input/Output attributes
    ImNodes::BeginInputAttribute(id * 10 + 1);
    ImGui::Text("In");
    ImNodes::EndInputAttribute();
    
    ImNodes::BeginOutputAttribute(id * 10 + 2);
    ImGui::Text("Out");
    ImNodes::EndOutputAttribute();

    // Kernel size selection
    int currentSize = static_cast<int>(kernelSize);
    ImGui::RadioButton("3x3", &currentSize, 0);
    ImGui::SameLine();
    ImGui::RadioButton("5x5", &currentSize, 1);
    kernelSize = static_cast<KernelSize>(currentSize);

    // Preset selection
    const char* presets[] = {"Custom", "Sharpen", "Emboss", "Edge Enhance"};
    int preset = static_cast<int>(currentPreset);
    if (ImGui::Combo("Presets", &preset, presets, IM_ARRAYSIZE(presets))) {
        currentPreset = static_cast<FilterPreset>(preset);
        updatePresetKernel();
        previewDirty = true;
    }

    // Kernel matrix input
    const int size = kernelSize == KernelSize::SIZE_3x3 ? 3 : 5;
    ImGui::Text("Kernel Matrix:");
    for (int i = 0; i < size; i++) {
        ImGui::Columns(size, nullptr, false);
        for (int j = 0; j < size; j++) {
            ImGui::PushID(i * size + j);
            float* val = &kernel[i][j];
            ImGui::InputFloat("", val, 0.1f, 1.0f, "%.1f");
            ImGui::PopID();
            ImGui::NextColumn();
        }
        ImGui::Columns(1);
    }

    // Preview
    if (previewDirty) {
        generatePreview();
        previewDirty = false;
    }
    ImGui::Image((void*)(intptr_t)previewImage.ptr, ImVec2(128, 128));

    ImNodes::EndNode();
}

cv::Mat ConvolutionFilterNode::process(const std::vector<cv::Mat>& inputs) {
    if (inputs.empty() || inputs[0].empty()) return cv::Mat();
    
    const int size = kernelSize == KernelSize::SIZE_3x3 ? 3 : 5;
    cv::Mat cvKernel(size, size, CV_32F);
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            cvKernel.at<float>(i, j) = kernel[i][j];
    
    cv::Mat result;
    cv::filter2D(inputs[0], result, -1, cvKernel);
    return result;
}

void ConvolutionFilterNode::createStandardKernels() {
    presetKernels = {
        {FilterPreset::SHARPEN, {{{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}}, KernelSize::SIZE_3x3}},
        {FilterPreset::EMBOSS, {{{-2, -1, 0}, {-1, 1, 1}, {0, 1, 2}}, KernelSize::SIZE_3x3}},
        {FilterPreset::EDGE_ENHANCE, {{{0, 0, 0}, {-1, 1, 0}, {0, 0, 0}}, KernelSize::SIZE_3x3}}
    };
}

void ConvolutionFilterNode::updatePresetKernel() {
    if (currentPreset == FilterPreset::CUSTOM) return;
    
    auto& [presetKernel, presetSize] = presetKernels[currentPreset];
    kernelSize = presetSize;
    
    const int size = static_cast<int>(presetSize) + 3; // 3x3 or 5x5
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            kernel[i][j] = i < presetKernel.size() && j < presetKernel[i].size() ? presetKernel[i][j] : 0;
}

void ConvolutionFilterNode::generatePreview() {
    // Create test pattern
    cv::Mat testPattern(128, 128, CV_8UC3);
    testPattern.setTo(cv::Scalar(128, 128, 128));
    cv::rectangle(testPattern, cv::Rect(30, 30, 68, 68), cv::Scalar(200, 200, 200), -1);
    
    // Apply current kernel
    cv::Mat processed;
    cv::Mat input;
    cv::cvtColor(testPattern, input, cv::COLOR_BGR2GRAY);
    input.convertTo(input, CV_32F, 1/255.0);
    process({input}).convertTo(processed, CV_8U, 255);
    cv::cvtColor(processed, previewImage, cv::COLOR_GRAY2BGR);
}
