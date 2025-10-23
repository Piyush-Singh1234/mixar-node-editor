
#include "imgui.h"
#include "imnodes.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>

// Core engine header
#include "NodeGraph.h"

// All node type headers are needed for the menu
#include "ImageInputNode.h"
#include "GrayscaleNode.h"
#include "NoiseGenerationNode.h"
#include "ConvolutionFilterNode.h"
#include "BlurNode.h"
#include "RotateNode.h"
#include "EdgeDetectionNode.h"
#include "ThresholdNode.h"
#include "SharpenNode.h"
#include "BrightnessNode.h"
#include "ContrastNode.h"
#include "InvertNode.h"
#include "ImageOutputNode.h"


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
    // SetupCustomStyle(); // Uncomment if you have this function

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImNodes::CreateContext();

    // Create a single instance of the node graph engine
    NodeGraph graph;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(1280, 720), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
        ImGui::Begin("Node Editor", nullptr, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Add Node")) {
                if (ImGui::MenuItem("Image Input")) graph.addNode<ImageInputNode>();
                if (ImGui::MenuItem("Grayscale")) graph.addNode<GrayscaleNode>();
                if (ImGui::MenuItem("Noise Generation")) graph.addNode<NoiseGenerationNode>();
                if (ImGui::MenuItem("Convolution Filter")) graph.addNode<ConvolutionFilterNode>();
                if (ImGui::MenuItem("Blur")) graph.addNode<BlurNode>();
                if (ImGui::MenuItem("Rotate")) graph.addNode<RotateNode>();
                if (ImGui::MenuItem("Edge Detection")) graph.addNode<EdgeDetectionNode>();
                if (ImGui::MenuItem("Threshold")) graph.addNode<ThresholdNode>();
                if (ImGui::MenuItem("Sharpen")) graph.addNode<SharpenNode>();
                if (ImGui::MenuItem("Brightness")) graph.addNode<BrightnessNode>();
                if (ImGui::MenuItem("Contrast")) graph.addNode<ContrastNode>();
                if (ImGui::MenuItem("Invert")) graph.addNode<InvertNode>();
                if (ImGui::MenuItem("Image Output")) graph.addNode<ImageOutputNode>();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImNodes::BeginNodeEditor();
        // Render nodes by getting them from the graph
        for (const auto& node : graph.getNodes()) {
            node->render();
        }
        // Render links by getting them from the graph
        for (const auto& link : graph.getLinks()) {
            ImNodes::Link(link.id, link.start_attr, link.end_attr);
        }
        ImNodes::EndNodeEditor();

        // Add new links to the graph
        int start_attr, end_attr;
        if (ImNodes::IsLinkCreated(&start_attr, &end_attr)) {
            graph.addLink(start_attr, end_attr);
        }

        // Delete nodes from the graph
        int hovered_node_id = -1;
        if (ImNodes::IsNodeHovered(&hovered_node_id) && ImGui::IsMouseDoubleClicked(0)) {
            graph.deleteNode(hovered_node_id);
        }
        
        ImGui::End();

        // Tell the graph to process the nodes
        graph.process();

        // --- RENDERING ---
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