#include "EdgeDetectionNode.h"
#include "imgui.h"
#include "imnodes.h"
#include <opencv2/imgproc.hpp>

EdgeDetectionNode::EdgeDetectionNode(int id_)
    : Node(id_), method(Method::Sobel),
      sobelKernel(3), cannyThresh1(100), cannyThresh2(200),
      overlayEdges(false) {}

void EdgeDetectionNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Edge Detection");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(id * 10 + 1);
    ImGui::Text("In");
    ImNodes::EndInputAttribute();

    const char* methods[] = { "Sobel", "Canny" };
    int current = static_cast<int>(method);
    if (ImGui::Combo("Method", &current, methods, IM_ARRAYSIZE(methods))) {
        method = static_cast<Method>(current);
    }

    if (method == Method::Canny) {
        ImGui::SliderInt("Threshold 1", &cannyThresh1, 0, 255);
        ImGui::SliderInt("Threshold 2", &cannyThresh2, 0, 255);
    } else if (method == Method::Sobel) {
        ImGui::SliderInt("Kernel Size", &sobelKernel, 1, 7);
        if (sobelKernel % 2 == 0) sobelKernel += 1;
    }

    ImGui::Checkbox("Overlay on Image", &overlayEdges);

    ImNodes::BeginOutputAttribute(id * 10 + 2);
    ImGui::Text("Out");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
}

cv::Mat EdgeDetectionNode::process(const std::vector<cv::Mat>& inputs) {
    if (inputs.empty() || inputs[0].empty()) return {};

    cv::Mat gray, edges, output;
    cv::cvtColor(inputs[0], gray, cv::COLOR_BGR2GRAY);

    if (method == Method::Canny) {
        cv::Canny(gray, edges, cannyThresh1, cannyThresh2);
    } else {  // Sobel
        cv::Mat grad_x, grad_y, abs_x, abs_y;
        cv::Sobel(gray, grad_x, CV_16S, 1, 0, sobelKernel);
        cv::Sobel(gray, grad_y, CV_16S, 0, 1, sobelKernel);
        cv::convertScaleAbs(grad_x, abs_x);
        cv::convertScaleAbs(grad_y, abs_y);
        cv::addWeighted(abs_x, 0.5, abs_y, 0.5, 0, edges);
    }

    if (overlayEdges) {
        output = inputs[0].clone();
        output.setTo(cv::Scalar(0, 0, 255), edges);  // red overlay
    } else {
        cv::cvtColor(edges, output, cv::COLOR_GRAY2BGR);  // keep output 3-channel
    }

    return output;
}
