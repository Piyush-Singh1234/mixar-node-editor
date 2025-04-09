#include "ImageOutputNode.h"
#include "imgui.h"
#include "imnodes.h"
#include <opencv2/opencv.hpp>
#include <GL/gl.h>  // OpenGL texture handling
#include <iostream>

ImageOutputNode::ImageOutputNode(int id_) : Node(id_) {
    textureID = 0;
}

void ImageOutputNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Output Node");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(id * 10 + 1);
    ImGui::Text("In");
    ImNodes::EndInputAttribute();

    if (textureID != 0) {
        ImGui::Text("Preview:");
        ImGui::Image((ImTextureID)(intptr_t)textureID, ImVec2(200, 200));

        // ✅ Save button
        if (ImGui::Button("Save Image")) {
            std::string filename = "output_" + std::to_string(id) + ".png";
            cv::imwrite(filename, lastImage);
            std::cout << "[💾] Image saved to " << filename << "\n";
        }
    }

    ImNodes::EndNode();
}


cv::Mat ImageOutputNode::process(const std::vector<cv::Mat>& inputs) {
    if (inputs.empty() || inputs[0].empty()) return {};

    lastImage = inputs[0];

    // Convert BGR to RGB
    cv::Mat rgb;
    cv::cvtColor(lastImage, rgb, cv::COLOR_BGR2RGB);

    if (textureID == 0)
        glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rgb.cols, rgb.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb.data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return lastImage;
}
