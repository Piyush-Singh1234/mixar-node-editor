#include "ImageInputNode.h"
#include "imgui.h"
#include "imnodes.h"
#include "ImGuiFileDialog.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <filesystem>
namespace fs = std::filesystem;


ImageInputNode::ImageInputNode(int id_) : Node(id_) {}

void ImageInputNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Image Input");
    ImNodes::EndNodeTitleBar();

    // Path input
    // static char buffer[256];
    strncpy(buffer, path.c_str(), sizeof(buffer));
    
    // ✅ Check if Enter is pressed after typing path
    if (ImGui::InputText("Image Path", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
        path = std::string(buffer);
        
        // Validate file extension (basic check)
        std::string ext = "";
        auto pos = path.find_last_of(".");
        if (pos != std::string::npos)
            ext = path.substr(pos);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);  // normalize extension

        if(ext != ".jpg" && ext != ".jpeg" && ext != ".png" && ext != ".bmp") {
            std::cout << "[❌] Unsupported file format: " << ext << std::endl;
            image.release();
        } else {
            image = cv::imread(path);
            if (image.empty()) {
                std::cout << "[❌] Failed to load image at: " << path << std::endl;
            } else {
                std::cout << "[✅] Loaded image: " << path
                          << " (" << image.cols << "x" << image.rows << ")\n";
            }
        }
    }
    if (ImGui::Button("Browse")) {
        
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose Image", ".png,.jpg,.jpeg,.bmp", {});
    }
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            path = filePathName;
            image = cv::imread(path);
            // Handle image loading success or failure
        }
        ImGuiFileDialog::Instance()->Close();
    }
    

    // Show status in UI
    if (!image.empty()) {
        // Get file size using filesystem
        std::string fileInfo;
        try {
            auto fsize = fs::file_size(path);
            std::stringstream ss;
            ss << "File Size: " << fsize / 1024 << " KB";  // Display in KB
            fileInfo = ss.str();
        } catch (const fs::filesystem_error &e) {
            fileInfo = "File size not available";
        }
        // Display metadata: dimensions, file size, and format
        ImGui::Text("Dimensions: %dx%d", image.cols, image.rows);
        ImGui::Text("%s", fileInfo.c_str());
        ImGui::Text("Format: %s", path.substr(path.find_last_of(".") + 1).c_str());
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
