#include "ConvolutionFilterNode.h"
#include "imgui.h"
#include "imnodes.h"
#include <opencv2/opencv.hpp>

ConvolutionFilterNode::ConvolutionFilterNode(int id)
    : Node(id, "Convolution Filter") {
    createStandardKernels();
    kernel.fill({0});
    kernel[1][1] = 1.0f; // Identity kernel
}

void ConvolutionFilterNode::render() {
    ImNodes::BeginNode(id);

    // Set a fixed width for the node to prevent overflow
    float nodeWidth = 300.0f; // Adjust this width to your needs
    ImGui::SetNextItemWidth(nodeWidth);  // This sets the width for UI components in the node

    // Push item width before node rendering
    ImGui::PushItemWidth(nodeWidth);

    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Convolution Filter");
    ImNodes::EndNodeTitleBar();

    // Input/Output ports
    ImNodes::BeginInputAttribute(id * 10 + 1);
    ImGui::Text("In");
    ImNodes::EndInputAttribute();
    
    ImNodes::BeginOutputAttribute(id * 10 + 2);
    ImGui::Text("Out");
    ImNodes::EndOutputAttribute();

    // Create a scrollable area for kernel size and preset UI elements
    ImGui::BeginChild("##scrollable", ImVec2(nodeWidth, 0), true);  // Adjust width here

    // Kernel size radio buttons with constrained width
    ImGui::BeginGroup();
    int currentSize = static_cast<int>(kernelSize);
    ImGui::RadioButton("3x3", &currentSize, 0);
    ImGui::SameLine();
    ImGui::RadioButton("5x5", &currentSize, 1);
    kernelSize = static_cast<KernelSize>(currentSize);
    ImGui::EndGroup();

    // Display the kernel size dynamically
    ImGui::Text("Kernel Size: %s", kernelSize == KernelSize::SIZE_3x3 ? "3x3" : "5x5");

    // Preset combo box
    const char* presets[] = {"Custom", "Sharpen", "Emboss", "Edge Enhance"};
    int preset = static_cast<int>(currentPreset);
    ImGui::SetNextItemWidth(150);
    if (ImGui::Combo("Presets", &preset, presets, IM_ARRAYSIZE(presets))) {
        currentPreset = static_cast<FilterPreset>(preset);
        updatePresetKernel();
        previewDirty = true;
    }

    // Kernel matrix with constrained columns
    const int size = kernelSize == KernelSize::SIZE_3x3 ? 3 : 5;
    ImGui::Text("Kernel Values:");
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));
    for (int i = 0; i < size; i++) {
        ImGui::Columns(size, "##kernel", false);
        for (int j = 0; j < size; j++) {
            ImGui::PushID(i * size + j);
            
            // Add the +/- buttons for each kernel value
            if (ImGui::Button("-", ImVec2(20, 20))) {
                kernel[i][j] -= 0.1f;
            }
            ImGui::SameLine();
            ImGui::Text("%.1f", kernel[i][j]); // Show the current kernel value
            ImGui::SameLine();
            if (ImGui::Button("+", ImVec2(20, 20))) {
                kernel[i][j] += 0.1f;
            }
            
            ImGui::PopID();
            ImGui::NextColumn();
        }
        ImGui::Columns(1);
    }
    ImGui::PopStyleVar();

    // End the scrollable area
    ImGui::EndChild();

    // Preview image with constrained size
    if (previewDirty) {
        generatePreview();
        previewDirty = false;
    }
    if (!previewImage.empty()) {
        // Display the preview image with reduced size
        ImGui::Image(reinterpret_cast<ImTextureID>(previewImage.data), 
                    ImVec2(120, 120)); // Reduced size
    }

    ImNodes::EndNode();

    // Pop item width after rendering the node
    ImGui::PopItemWidth();  // Correctly pop item width once after node rendering
}



cv::Mat ConvolutionFilterNode::process(const std::vector<cv::Mat>& inputs) {
    if (inputs.empty() || inputs[0].empty()) return cv::Mat();
    
    const int size = kernelSize == KernelSize::SIZE_3x3 ? 3 : 5;
    cv::Mat cvKernel(size, size, CV_32F);
    
    // Copy kernel values with normalization
    float sum = 0.0f;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            cvKernel.at<float>(i, j) = kernel[i][j];
            sum += kernel[i][j];
        }
    }
    
    // Normalize if not zero-sum (edge detection)
    if (abs(sum) > 1e-4 && currentPreset != FilterPreset::EDGE_ENHANCE) {
        cvKernel /= sum;
    }

    cv::Mat result;
    cv::filter2D(inputs[0], result, -1, cvKernel);
    return result;
}

void ConvolutionFilterNode::createStandardKernels() {
    presetKernels = {
        {FilterPreset::SHARPEN, {{{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}}, KernelSize::SIZE_3x3}},
        {FilterPreset::EMBOSS, {{{-2, -1, 0}, {-1, 1, 1}, {0, 1, 2}}, KernelSize::SIZE_3x3}},
        {FilterPreset::EDGE_ENHANCE, {{{-1, -1, -1}, {-1, 9, -1}, {-1, -1, -1}}, KernelSize::SIZE_3x3}}
    };
}

void ConvolutionFilterNode::updatePresetKernel() {
    if (currentPreset == FilterPreset::CUSTOM) return;
    
    auto& [presetKernel, presetSize] = presetKernels[currentPreset];
    kernelSize = presetSize;
    
    const int size = static_cast<int>(presetSize) == 0 ? 3 : 5;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (i < presetKernel.size() && j < presetKernel[i].size()) {
                kernel[i][j] = presetKernel[i][j];
            } else {
                kernel[i][j] = 0;
            }
        }
    }
}

void ConvolutionFilterNode::generatePreview() {
    // Create test pattern
    cv::Mat testPattern(128, 128, CV_8UC3);
    testPattern.setTo(cv::Scalar(128, 128, 128));
    cv::rectangle(testPattern, cv::Rect(30, 30, 68, 68), cv::Scalar(200, 200, 200), -1);
    
    // Process preview
    cv::Mat input, processed;
    cv::cvtColor(testPattern, input, cv::COLOR_BGR2GRAY);
    input.convertTo(input, CV_32F, 1.0/255.0);
    
    // Apply current kernel
    cv::Mat result = process({input});
    
    // Normalize for display
    double minVal, maxVal;
    cv::minMaxLoc(result, &minVal, &maxVal);
    result.convertTo(processed, CV_8U, 255.0/(maxVal - minVal), -minVal * 255.0/(maxVal - minVal));
    cv::cvtColor(processed, previewImage, cv::COLOR_GRAY2BGR);
}