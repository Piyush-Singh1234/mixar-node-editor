#include "NodeGraph.h"
#include <iostream>
#include <unordered_map>
#include <queue>
#include <algorithm>


void NodeGraph::addLink(int start_attr, int end_attr) {
    links.push_back({next_link_id++, start_attr, end_attr});
}

void NodeGraph::deleteNode(int node_id) {
    auto node_it = std::find_if(nodes.begin(), nodes.end(),
        [node_id](const auto& n) { return n->getId() == node_id; });
    
    if (node_it != nodes.end()) {
        // Get all attributes of the node to be deleted
        std::vector<int> node_attrs = (*node_it)->getInputAttributes();
        node_attrs.push_back((*node_it)->getOutputAttr());

        // Remove any links connected to this node
        links.erase(std::remove_if(links.begin(), links.end(),
            [&node_attrs](const Link& link) {
                return std::find(node_attrs.begin(), node_attrs.end(), link.start_attr) != node_attrs.end() ||
                       std::find(node_attrs.begin(), node_attrs.end(), link.end_attr) != node_attrs.end();
            }), links.end());

        // Remove the node itself
        nodes.erase(node_it);
    }
}

void NodeGraph::process() {
    // --- 1. Build Dependency Graph for Topological Sort ---
    std::unordered_map<int, std::vector<int>> adjacency_list;
    std::unordered_map<int, int> in_degree;

    for (const auto& node : nodes) {
        in_degree[node->getId()] = 0; // Initialize in-degree for all nodes
    }

    for (const auto& link : links) {
        int from_node = link.start_attr / 10;
        int to_node = link.end_attr / 10;
        adjacency_list[from_node].push_back(to_node);
        in_degree[to_node]++;
    }

    // --- 2. Kahn's Algorithm for Topological Sort ---
    std::queue<int> q;
    for (const auto& node : nodes) {
        if (in_degree[node->getId()] == 0) {
            q.push(node->getId());
        }
    }

    std::vector<int> processing_order;
    while (!q.empty()) {
        int current_id = q.front();
        q.pop();
        processing_order.push_back(current_id);

        for (int neighbor_id : adjacency_list[current_id]) {
            if (--in_degree[neighbor_id] == 0) {
                q.push(neighbor_id);
            }
        }
    }

    // --- 3. Process Nodes in Topological Order (Simplified) ---
    std::unordered_map<int, cv::Mat> attributeOutputs;

    // --- Pre-computation Step: Create fast lookup maps ---
    // 1. Create a map from node_id -> Node* for instant node access.
    std::unordered_map<int, Node*> node_map;    
    for (const auto& node : nodes) {
        node_map[node->getId()] = node.get();  // gives the raw pointer
    }

    // 2. Create a map from an input pin -> the output pin it's connected to.
    std::unordered_map<int, int> link_map; // Maps: end_attr -> start_attr
    for (const auto& link : links) {
        link_map[link.end_attr] = link.start_attr;
    }


    // --- Simplified Processing Loop ---
    for (int node_id : processing_order) {
        // Step 1: Find the current node (now an instant lookup).
        Node* current_node = node_map[node_id];
        if (!current_node) continue;

        // Step 2: Gather inputs (now much simpler and faster).
        std::vector<cv::Mat> inputs;
        for (int input_attr : current_node->getInputAttributes()) {
            // Find the link connected to this input pin.
            auto it = link_map.find(input_attr);
            if (it != link_map.end()) {
                // 'it->second' gives us the source output pin (start_attr).
                int source_attr = it->second;
                inputs.push_back(attributeOutputs[source_attr]);
            }
        }

        // Step 3: Process the node and store its output.
        cv::Mat output = current_node->process(inputs);
        attributeOutputs[current_node->getOutputAttr()] = output;
    }

}

const std::vector<std::unique_ptr<Node>>& NodeGraph::getNodes() const {
    return nodes;
}

const std::vector<Link>& NodeGraph::getLinks() const {
    return links;
}