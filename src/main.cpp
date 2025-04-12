// main.cpp — Docking-free UI Enhancement for Mixar Node Editor (with scrolling)

#include "imgui.h"
#include "imnodes.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>

#include "ImageInputNode.h"
#include "GrayscaleNode.h"
#include "NoiseGenerationNode.h"
#include "ConvolutionFilterNode.h"
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

#include <iostream>
#include <string>
#include <vector>
#include <queue>     // For Kahn's algorithm
#include <algorithm> // For std::find_if
#include <memory>
#include <unordered_map>
#include <algorithm>

struct Link {
    int id;
    int start_attr;
    int end_attr;
};

static int id_counter = 1;
std::vector<std::unique_ptr<Node>> nodes;
std::vector<Link> links;
int link_id_counter = 100;

void SetupCustomStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.ScrollbarRounding = 5.0f;
    style.GrabRounding = 3.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg]          = ImVec4(0.12f, 0.12f, 0.13f, 1.00f);
    colors[ImGuiCol_TitleBg]           = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);
    colors[ImGuiCol_TitleBgActive]     = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
    colors[ImGuiCol_Header]            = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
    colors[ImGuiCol_HeaderHovered]     = ImVec4(0.25f, 0.30f, 0.35f, 1.00f);
    colors[ImGuiCol_HeaderActive]      = ImVec4(0.30f, 0.35f, 0.40f, 1.00f);
}

int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Mixar Node Editor", NULL, NULL);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    SetupCustomStyle();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImNodes::CreateContext();
    // nodes.push_back(std::make_unique<ImageInputNode>(id_counter++));

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(1280, 720), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
        ImGui::Begin("Node Editor", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Add Node")) {
                if (ImGui::MenuItem("Image Input Node")) nodes.push_back(std::make_unique<ImageInputNode>(id_counter++));
                if (ImGui::MenuItem("Grayscale Node")) nodes.push_back(std::make_unique<GrayscaleNode>(id_counter++));
                if (ImGui::MenuItem("Noise Generation Node")) nodes.push_back(std::make_unique<NoiseGenerationNode>(id_counter++));
                if (ImGui::MenuItem("Convolution Filter Node")) nodes.push_back(std::make_unique<ConvolutionFilterNode>(id_counter++));
                if (ImGui::MenuItem("Blur Node")) nodes.push_back(std::make_unique<BlurNode>(id_counter++));
                if (ImGui::MenuItem("Blend Node")) nodes.push_back(std::make_unique<BlendNode>(id_counter++));
                if (ImGui::MenuItem("Rotate Node")) nodes.push_back(std::make_unique<RotateNode>(id_counter++));
                if (ImGui::MenuItem("Edge Detection Node")) nodes.push_back(std::make_unique<EdgeDetectionNode>(id_counter++));
                if (ImGui::MenuItem("Threshold Node")) nodes.push_back(std::make_unique<ThresholdNode>(id_counter++));
                if (ImGui::MenuItem("Sharpen Node")) nodes.push_back(std::make_unique<SharpenNode>(id_counter++));
                if (ImGui::MenuItem("Brightness Node")) nodes.push_back(std::make_unique<BrightnessNode>(id_counter++));
                if (ImGui::MenuItem("Contrast Node")) nodes.push_back(std::make_unique<ContrastNode>(id_counter++));
                if (ImGui::MenuItem("Color Splitter Node")) nodes.push_back(std::make_unique<ColorChannelSplitterNode>(id_counter++));
                if (ImGui::MenuItem("Color Merger Node")) nodes.push_back(std::make_unique<ColorChannelMergerNode>(id_counter++));
                if (ImGui::MenuItem("Invert Node")) nodes.push_back(std::make_unique<InvertNode>(id_counter++));
                if (ImGui::MenuItem("Output Node")) nodes.push_back(std::make_unique<ImageOutputNode>(id_counter++));
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::BeginChild("ScrollableCanvas", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        ImNodes::BeginNodeEditor();
        for (auto& node : nodes) node->render();
        for (const auto& link : links) ImNodes::Link(link.id, link.start_attr, link.end_attr);
        ImNodes::EndNodeEditor();

        int start_attr, end_attr;
        if (ImNodes::IsLinkCreated(&start_attr, &end_attr)) {
            links.push_back({link_id_counter++, start_attr, end_attr});
        }

        // int hovered_node = -1;
        // if (ImNodes::IsNodeHovered(&hovered_node) && ImGui::IsMouseDoubleClicked(0)) {
        //     nodes.erase(std::remove_if(nodes.begin(), nodes.end(),
        //         [hovered_node](const auto& node) { return node->getId() == hovered_node; }), nodes.end());

        //     const int node_input = hovered_node * 10 + 1;
        //     const int node_output = hovered_node * 10 + 2;
        //     links.erase(std::remove_if(links.begin(), links.end(),
        //         [node_input, node_output](const Link& link) {
        //             return link.end_attr == node_input || link.start_attr == node_output;
        //         }), links.end());
        // }

        // int hovered_link = -1;
        // if (ImNodes::IsLinkHovered(&hovered_link) && ImGui::IsMouseDoubleClicked(0)) {
        //     links.erase(std::remove_if(links.begin(), links.end(),
        //         [hovered_link](const Link& link) { return link.id == hovered_link; }), links.end());
        // }
        // Modified node deletion logic
        int hovered_node = -1;
        if (ImNodes::IsNodeHovered(&hovered_node) && ImGui::IsMouseDoubleClicked(0)) {
            auto node_it = std::find_if(nodes.begin(), nodes.end(),
                [hovered_node](const auto& n) { return n->getId() == hovered_node; });
            
            if (node_it != nodes.end()) {
                // Get ALL input/output attributes
                std::vector<int> node_attrs = (*node_it)->getInputAttributes();
                node_attrs.push_back((*node_it)->getOutputAttr());

                // Remove connected links
                links.erase(std::remove_if(links.begin(), links.end(),
                    [&node_attrs](const Link& link) {
                        return std::find(node_attrs.begin(), node_attrs.end(), link.start_attr) != node_attrs.end() ||
                            std::find(node_attrs.begin(), node_attrs.end(), link.end_attr) != node_attrs.end();
                    }), links.end());

                // Remove the node
                nodes.erase(node_it);
            }
        }


        ImGui::EndChild();
        ImGui::End();


        std::unordered_map<int, std::vector<int>> adjacency_list;
        std::unordered_map<int, int> in_degree;

        // Build dependency graph
        for (const auto& link : links) {
            int from_node = link.start_attr / 10; // Extract node ID from attribute
            int to_node = link.end_attr / 10;
            adjacency_list[from_node].push_back(to_node);
            in_degree[to_node]++;
        }

        // Kahn's algorithm for topological sort
        std::queue<int> q;
        std::vector<int> processing_order;
        for (const auto& node : nodes) {
            int node_id = node->getId();
            if (in_degree[node_id] == 0) {
                q.push(node_id);
            }
        }

        while (!q.empty()) {
            int current = q.front();
            q.pop();
            processing_order.push_back(current);

            for (int neighbor : adjacency_list[current]) {
                if (--in_degree[neighbor] == 0) {
                    q.push(neighbor);
                }
            }
        }

        // Process nodes in topological order
        std::unordered_map<int, cv::Mat> attributeOutputs;
        for (int node_id : processing_order) {
            auto it = std::find_if(nodes.begin(), nodes.end(), 
                [node_id](const auto& node) { return node->getId() == node_id; });
            if (it == nodes.end()) continue;

            Node* node = it->get();
            std::vector<cv::Mat> inputs;

            // Collect inputs from all connected attributes
            for (int input_attr : node->getInputAttributes()) { // Get ALL input attributes
                for (const auto& link : links) { // Check all links
                    if (link.end_attr == input_attr) { // Match attribute ID
                        inputs.push_back(attributeOutputs[link.start_attr]); // Fetch output from connected node
                    }
                }
            }


            cv::Mat output = node->process(inputs);
            attributeOutputs[node->getOutputAttr()] = output;
        }

        std::cout << "\n=== Processing Order ===";
        for (int node_id : processing_order) {
            auto node_it = std::find_if(nodes.begin(), nodes.end(),
                [node_id](const auto& n) { return n->getId() == node_id; });
            if (node_it != nodes.end()) {
                std::cout << "\nNode " << node_id << " (" << (*node_it)->name << ")";
                std::cout << "\n  Output attr: " << (*node_it)->getOutputAttr();
            }
        }

        std::cout << "\n\n=== Links ===";
        for (const auto& link : links) {
            std::cout << "\nLink " << link.id << ": " << link.start_attr << " → " << link.end_attr;
        }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImNodes::DestroyContext();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}