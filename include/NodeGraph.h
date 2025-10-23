#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include "Node.h" 

struct Link {
    int id;
    int start_attr;
    int end_attr;
};

class NodeGraph {
public:
    NodeGraph() = default;

    // --- Graph Modification Methods ---
    template <typename T>
    void addNode() {
        nodes.push_back(std::make_unique<T>(next_node_id++));
    }
    
    void addLink(int start_attr, int end_attr);
    void deleteNode(int node_id);

    // --- Core Processing ---
    void process();

    // --- Data Access for Rendering ---
    const std::vector<std::unique_ptr<Node>>& getNodes() const;
    const std::vector<Link>& getLinks() const;

private:
    std::vector<std::unique_ptr<Node>> nodes;
    std::vector<Link> links;

    int next_node_id = 1;
    int next_link_id = 100;
};