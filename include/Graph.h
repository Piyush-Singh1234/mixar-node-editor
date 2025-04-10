#pragma once

#include "Node.h"
#include <unordered_map>

struct Edge {
    int fromNodeId;
    int toNodeId;
};

class NodeGraph {
public:
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<Edge> connections;

    void addNode(std::shared_ptr<Node> node) {
        nodes.push_back(node);
    }

    void addConnection(int fromId, int toId) {
        connections.push_back({fromId, toId});
    }

    std::shared_ptr<Node> getNodeById(int id) {
        for (auto& node : nodes)
            if (node->id == id)
                return node;
        return nullptr;
    }
};