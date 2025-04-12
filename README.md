# Mixar Node Editor - Design Documentation

## 🔎 Project Overview

The **Mixar Node Editor** is a C++ desktop application for building custom image processing pipelines using a node-based visual editor. Each node represents a distinct image operation (e.g., grayscale, blend, edge detection), allowing users to interactively create workflows by connecting nodes. Built on **OpenCV**, **ImGui**, and **ImNodes**, the app emphasizes modularity, real-time processing, and intuitive UI.

## 📊 Architecture Overview

### 1. Node System

#### • Base Class - `Node`
- Declared in `include/Node.h`
- Abstract base class for all node types.
- Core methods:
  - `render()`: Renders the UI of the node.
  - `process(const std::vector<cv::Mat>& inputs)`: Performs the image processing logic.
  - Attribute getter methods for managing links.

#### • Specialized Nodes (in `src/nodes/`)
Each inherits from `Node` and implements its logic:
- `ImageInputNode`: Loads and previews image files.
- `NoiseGenerationNode`: Generates procedural noise (Perlin, Simplex, etc.).
- `ConvolutionFilterNode`: User-defined kernel filters.
- Additional nodes include:
  - Grayscale
  - Blur
  - Blend
  - Rotate
  - Edge Detection
  - Threshold
  - Sharpen
  - Brightness & Contrast
  - Color Channel Split
  - Invert
  - ImageOutputNode: Displays processed output.

#### • Node Graph Processing (in `main.cpp`)
- Maintains a list of node instances and connection links.
- Builds a DAG of the image flow.
- Calls `process()` of each node in topological order.
- Intermediate outputs cached in a map by node ID.

### 2. UI and Interaction

#### • ImGui + ImNodes
- **ImGui** for widget rendering.
- **ImNodes** for the visual graph editor.
- Node menu for adding new nodes.
- Connection creation, deletion, and validation via mouse interactions.

#### • File Browsing
- `ImGuiFileDialog` allows file system access for image input.

## ⚖️ Design Decisions

### ✔️ Modular Node-Based System
- Encourages **reusability**, **maintainability**, and **easy extensibility**.
- Any new operation = just a new `Node` subclass.

### ✔️ OpenCV for Image Processing
- Robust, mature library.
- Handles a wide range of formats and transformations efficiently.

### ✔️ ImGui + ImNodes for UI
- Lightweight immediate-mode GUI.
- Smooth integration and real-time interaction.

### ✔️ CMake for Build System
- Cross-platform.
- Handles linking of OpenCV, ImGui, GLFW, etc.

## 📑 Code Documentation Guidelines

- **Inline comments** provided for important logic.
- **render()** and **process()** clearly separated in each node.
- Functions and variables named descriptively.

## 📊 Third-Party Libraries Used

| Library           | Purpose                                      |
|------------------|----------------------------------------------|
| OpenCV           | Core image processing tasks                  |
| ImGui            | GUI rendering (buttons, sliders, menus)      |
| ImNodes          | Node editor interface                        |
| ImGuiFileDialog  | File browsing for input nodes                |
| GLFW + OpenGL    | Window and rendering context management      |
| CMake            | Build configuration and dependency handling  |


## 🌟 Key Features Summary

- Modular C++ design
- Real-time node-based image editing
- Drag-and-drop UI
- File system access for image input
- 15+ built-in nodes for transformation, filtering, and adjustment

---

This documentation provides a full overview of the code architecture, core components, and design philosophy behind the Mixar Node Editor. For implementation details, refer to the `src/` and `include/` folders in the repository.

