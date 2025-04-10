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
#include "NoiseGenerationNode.h"
#include "BlurNode.h"
#include "BlendNode.h"
#include "RotateNode.h"
#include "EdgeDetectionNode.h"
#include "ThresholdNode.h"
#include "SharpenNode.h"
#include "BrightnessNode.h"
#include "ContrastNode.h"
#include "ColorChannelSplitterNode.h"
#include "ColorChannelMergerNode.h"
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
                if (ImGui::MenuItem("Image Input Node")) {
                    nodes.push_back(std::make_unique<ImageInputNode>(id_counter++));
                }                
                if (ImGui::MenuItem("Invert Node")) {
                    nodes.push_back(std::make_unique<InvertNode>(id_counter++));
                }  
                if (ImGui::MenuItem("Noise Generation Node")) {
                    nodes.push_back(std::make_unique<NoiseGenerationNode>(id_counter++));
                }                                
                if (ImGui::MenuItem("Blend Node")) {
                    nodes.push_back(std::make_unique<BlendNode>(id_counter++));
                }               
                if (ImGui::MenuItem("Grayscale Node")) {
                    nodes.push_back(std::make_unique<GrayscaleNode>(id_counter++));
                }
                if (ImGui::MenuItem("Sharpen Node")) {
                    nodes.push_back(std::make_unique<SharpenNode>(id_counter++));
                }              
                if (ImGui::MenuItem("Rotate Node")) {
                    nodes.push_back(std::make_unique<RotateNode>(id_counter++));
                }  
                if (ImGui::MenuItem("Color Channel Splitter")) {
                    nodes.push_back(std::make_unique<ColorChannelSplitterNode>(id_counter++));
                } 
                if (ImGui::MenuItem("Color Channel Merger")) {
                    nodes.push_back(std::make_unique<ColorChannelMergerNode>(id_counter++));
                }                
                if (ImGui::MenuItem("Edge Detection Node")) {
                    nodes.push_back(std::make_unique<EdgeDetectionNode>(id_counter++));
                }                
                if (ImGui::MenuItem("Threshold Node")) {
                    nodes.push_back(std::make_unique<ThresholdNode>(id_counter++));
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
        // Build a map of destination attribute → source attribute based on detected links
        std::unordered_map<int, int> inputToOutputMap;
        for (const auto& link : links) {
            inputToOutputMap[link.second] = link.first;
        }

        // A map to store node outputs keyed by the node's output attribute
        std::unordered_map<int, cv::Mat> attributeOutputs;

        // Process each node in the graph
        for (auto& node : nodes) {
            std::vector<cv::Mat> inputs;

            // Check if the node is a BlendNode (which needs two separate inputs)
            if (BlendNode* blend = dynamic_cast<BlendNode*>(node.get())) {
                // First input attribute is: id * 10 + 1
                int inAttr1 = blend->id * 10 + 1;
                // Second input attribute is: id * 10 + 2
                int inAttr2 = blend->id * 10 + 2;

                // For first input, if there's a link then collect the source image
                if (inputToOutputMap.count(inAttr1)) {
                    int srcAttr = inputToOutputMap[inAttr1];
                    if (attributeOutputs.count(srcAttr)) {
                        inputs.push_back(attributeOutputs[srcAttr]);
                    }
                }
                // For second input, if there's a link then collect the source image
                if (inputToOutputMap.count(inAttr2)) {
                    int srcAttr = inputToOutputMap[inAttr2];
                    if (attributeOutputs.count(srcAttr)) {
                        inputs.push_back(attributeOutputs[srcAttr]);
                    }
                }
            } else {
                // For regular nodes expecting one input:
                int inAttr = node->getInputAttr();
                if (inputToOutputMap.count(inAttr)) {
                    int srcAttr = inputToOutputMap[inAttr];
                    if (attributeOutputs.count(srcAttr)) {
                        inputs.push_back(attributeOutputs[srcAttr]);
                    }
                }
            }

            // Process the node only if we have some valid inputs (or let the node handle empty vector)
            cv::Mat outImage = node->process(inputs);
            
            // Debug print: you can print node id and whether it got any inputs
            std::cout << "[Graph] Processing node ID: " << node->id 
                    << " | Inputs: " << (inputs.empty() ? "none" : std::to_string(inputs.size()))
                    << "\n";

            // Store the output image in the map using the node's output attribute
            int outAttr = node->getOutputAttr();
            attributeOutputs[outAttr] = outImage;
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