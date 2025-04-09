// ImGui and ImNodes core
#include "imgui.h"
#include "imnodes.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// OpenGL / GLFW
#include <GLFW/glfw3.h>

// OpenCV
#include <opencv2/opencv.hpp>

// Node system headers
#include "ImageInputNode.h"
#include "GrayscaleNode.h"
#include "BlurNode.h"
#include "SharpenNode.h"
#include "BrightnessNode.h"
#include "ContrastNode.h"
#include "InvertNode.h"
#include "ImageOutputNode.h"

// Optional: C++ std includes
#include <iostream>
#include <string>

#include <vector>
#include <memory>

// for std::pair
#include <utility>  
std::vector<std::pair<int, int>> links;  // Stores (from_attr, to_attr)
int link_id_counter = 100;               // Unique ID for links

static int id_counter = 1;
std::vector<std::unique_ptr<Node>> nodes;
// nodes.push_back(std::make_unique<ImageInputNode>(id_counter++));
// static ImageInputNode input(id_counter++);
static BlurNode blur(id_counter++);
static BrightnessNode bright(id_counter++);
static ImageOutputNode output(id_counter++);


int main() {
    // Init GLFW
    if (!glfwInit()) return -1;

    // OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Mixar Node Editor", NULL, NULL);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();

    // Init ImNodes
    ImNodes::CreateContext();

    nodes.push_back(std::make_unique<ImageInputNode>(id_counter++));


    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Node Editor", nullptr, ImGuiWindowFlags_MenuBar);

        // Add Node Menu
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Add Node")) {
                if (ImGui::MenuItem("Invert Node")) {
                    nodes.push_back(std::make_unique<InvertNode>(id_counter++));
                }                
                if (ImGui::MenuItem("Grayscale Node")) {
                    nodes.push_back(std::make_unique<GrayscaleNode>(id_counter++));
                }
                if (ImGui::MenuItem("Sharpen Node")) {
                    nodes.push_back(std::make_unique<SharpenNode>(id_counter++));
                }                
                if (ImGui::MenuItem("Contrast Node")) {
                    nodes.push_back(std::make_unique<ContrastNode>(id_counter++));
                }                
                if (ImGui::MenuItem("Blur Node")) {
                    nodes.push_back(std::make_unique<BlurNode>(id_counter++));
                }
                if (ImGui::MenuItem("Brightness Node")) {
                    nodes.push_back(std::make_unique<BrightnessNode>(id_counter++));
                }
                if (ImGui::MenuItem("Output Node")) {
                    nodes.push_back(std::make_unique<ImageOutputNode>(id_counter++));
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Begin node canvas
        ImNodes::BeginNodeEditor();

        // Render all dynamic nodes
        for (auto& node : nodes) {
            node->render();
        }

        // Draw existing links
        for (const auto& link : links) {
            ImNodes::Link(link_id_counter++, link.first, link.second);
        }

        ImNodes::EndNodeEditor();  // ✅ END before calling IsLinkCreated

        // Now safe to detect new links
        int start_attr, end_attr;
        if (ImNodes::IsLinkCreated(&start_attr, &end_attr)) {
            links.emplace_back(start_attr, end_attr);
            std::cout << "Link created: " << start_attr << " ➝ " << end_attr << "\n";
        }

        ImGui::End(); // end node editor window

        // --- Hardcoded Processing (still temporary) ---
        // cv::Mat img1 = input.process({});
        // if (!img1.empty()) {
        //     cv::Mat img2 = blur.process({img1});
        //     cv::Mat img3 = bright.process({img2});
        //     output.process({img3});
        // } else {
        //     std::cout << "[Info] No valid image loaded yet.\n";
        // }
        // 🟡 You already have link detection here:
        // int start_attr, end_attr;
        if (ImNodes::IsLinkCreated(&start_attr, &end_attr)) {
            links.emplace_back(start_attr, end_attr);
            std::cout << "Link created: " << start_attr << " ➝ " << end_attr << "\n";
        }

        // 🔁 1. Build map of input ➝ output
        std::unordered_map<int, int> inputToOutputMap;
        for (const auto& link : links) {
            inputToOutputMap[link.second] = link.first;
        }

        // 📦 2. Store image outputs from each node
        std::unordered_map<int, cv::Mat> attributeOutputs;

        // 🧠 3. Evaluate each node based on links
        for (auto& node : nodes) {
            int inputAttr = node->getInputAttr();
            int outputAttr = node->getOutputAttr();

            std::vector<cv::Mat> inputs;

            // if this node has a connected input
            if (inputToOutputMap.count(inputAttr)) {
                int sourceOutputAttr = inputToOutputMap[inputAttr];
                if (attributeOutputs.count(sourceOutputAttr)) {
                    inputs.push_back(attributeOutputs[sourceOutputAttr]);
                }
            }

            // 🚀 Process node
            cv::Mat output = node->process(inputs);
            std::cout << "[Graph] Processing node ID: " << node->id
            << " | Inputs: " << (inputs.empty() ? "none" : "yes") << "\n";


            // 💾 Save output for next connected node
            attributeOutputs[outputAttr] = output;
        }

        // Render to screen
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }


    // Cleanup
    ImNodes::DestroyContext();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
