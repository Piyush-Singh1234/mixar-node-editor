#include "ImageInputNode.h"
#include "imgui.h"
#include "imnodes.h"
#include <opencv2/opencv.hpp>
#include <iostream>

ImageInputNode::ImageInputNode(int id_) : Node(id_) {}


void ImageInputNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Image Input");
    ImNodes::EndNodeTitleBar();

    // Path input
    static char buffer[256];
    strncpy(buffer, path.c_str(), sizeof(buffer));
    
    // ✅ Check if Enter is pressed after typing path
    if (ImGui::InputText("Image Path", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
        path = std::string(buffer);
        image = cv::imread(path);
        if (image.empty()) {
            std::cout << "[❌] Failed to load image at: " << path << std::endl;
        } else {
            std::cout << "[✅] Loaded image: " << path
                      << " (" << image.cols << "x" << image.rows << ")\n";
        }
    }

    // Show status in UI
    if (!image.empty()) {
        ImGui::Text("Image Loaded: %dx%d", image.cols, image.rows);
    } else {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "No image loaded.");
    }

    ImNodes::BeginOutputAttribute(id * 10 + 2);  // output attr
    ImGui::Text("Out");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
}

// ✅ Processing: just return the loaded image
cv::Mat ImageInputNode::process(const std::vector<cv::Mat>&) {
    return image;
}
