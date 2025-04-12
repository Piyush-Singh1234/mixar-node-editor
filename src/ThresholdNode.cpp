

#include "ThresholdNode.h"
#include "imgui.h"
#include "imnodes.h"
#include <opencv2/opencv.hpp>

void ThresholdNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Threshold Node");
    ImNodes::EndNodeTitleBar();

    // Input/Output ports
    ImNodes::BeginInputAttribute(id * 10 + 1);
    ImGui::Text("In");
    ImNodes::EndInputAttribute();

    ImNodes::BeginOutputAttribute(id * 10 + 2);
    ImGui::Text("Out");
    ImNodes::EndOutputAttribute();

    // Threshold method selection
    const char* methods[] = { "Binary", "Adaptive", "Otsu" };
    ImGui::Combo("Method", (int*)&method, methods, IM_ARRAYSIZE(methods));

    // Parameters based on selected method
    if (method == BINARY || method == OTSU) {
        ImGui::SliderFloat("Threshold", &thresholdValue, 0.0f, 255.0f);
    }
    else if (method == ADAPTIVE) {
        ImGui::SliderInt("Block Size", &blockSize, 3, 51);
        blockSize = blockSize % 2 == 0 ? blockSize + 1 : blockSize; // Ensure odd
        ImGui::SliderFloat("Constant", &adaptiveConstant, 0.0f, 10.0f);
    }

    // Histogram display
    if (!histogramData.empty()) {
        ImGui::PlotHistogram("##Histogram", histogramData.data(), 
                           histogramData.size(), 0, NULL, 
                           0.0f, FLT_MAX, ImVec2(200, 50));
        
        // Click to set threshold
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
            ImVec2 mousePos = ImGui::GetMousePos();
            ImVec2 regionMin = ImGui::GetItemRectMin();
            float relativeX = mousePos.x - regionMin.x;
            thresholdValue = (relativeX / 200.0f) * 255.0f;
        }
    }

    ImNodes::EndNode();
}

cv::Mat ThresholdNode::process(const std::vector<cv::Mat>& inputs) {
    if (inputs.empty() || inputs[0].empty()) return {};

    cv::Mat gray, binary;
    cv::cvtColor(inputs[0], gray, cv::COLOR_BGR2GRAY);
    
    // Calculate histogram for visualization
    calculateHistogram(gray);

    switch (method) {
        case BINARY:
            cv::threshold(gray, binary, thresholdValue, 255, cv::THRESH_BINARY);
            break;
            
        case ADAPTIVE:
            cv::adaptiveThreshold(gray, binary, 255,
                                 cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                                 cv::THRESH_BINARY,
                                 blockSize, adaptiveConstant);
            break;
            
        case OTSU:
            cv::threshold(gray, binary, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
            thresholdValue = cv::threshold(gray, binary, 0, 255, 
                                         cv::THRESH_BINARY + cv::THRESH_OTSU);
            break;
    }

    return binary;
}

void ThresholdNode::calculateHistogram(const cv::Mat& image) {
    // Calculate histogram
    const int channels[] = {0};
    const int histSize[] = {256};
    const float range[] = {0, 256};
    const float* ranges[] = {range};
    
    cv::Mat hist;
    cv::calcHist(&image, 1, channels, cv::Mat(), hist, 1, histSize, ranges);
    
    // Convert to normalized float array for ImGui
    cv::normalize(hist, hist, 0.0f, 1.0f, cv::NORM_MINMAX);
    histogramData.resize(256);
    for (int i = 0; i < 256; ++i) {
        histogramData[i] = hist.at<float>(i);
    }
}