// #pragma once

// #include "Node.h"
// #include "ImageInputNode.h"
// #include "ImageOutputNode.h"
// #include "BlurNode.h"
// #include "BrightnessNode.h"

// enum class NodeType {
//     ImageInput,
//     ImageOutput,
//     Blur,
//     Brightness
// };

// struct NodeWrapper {
//     int id;
//     NodeType type;
//     Node* instance;
// };




#pragma once

#include <vector>
#include <memory>
#include <utility>
#include "Node.h"

class NodeGraph {
public:
    void addNode(std::shared_ptr<Node> node);
    void addLink(int start_attr, int end_attr);

    const std::vector<std::shared_ptr<Node>>& getNodes() const;
    const std::vector<std::pair<int, int>>& getLinks() const;

private:
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<std::pair<int, int>> links;
};
