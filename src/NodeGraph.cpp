
#include "NodeGraph.h"

void NodeGraph::addNode(std::shared_ptr<Node> node) {
    nodes.push_back(node);
}

void NodeGraph::addLink(int start_attr, int end_attr) {
    links.emplace_back(start_attr, end_attr);
}

 // Gather inputs from linked nodes
 void NodeGraph::processNodes() {
    for (const auto& node : nodes) {
        std::vector<cv::Mat> inputs;
        bool inputsCollected = false;

        // Gather inputs from linked nodes
        for (const auto& link : links) {
            if (link.second == node->getInputAttr()) {
                for (const auto& otherNode : nodes) {
                    if (otherNode->getOutputAttr() == link.first) {
                        inputs.push_back(otherNode->process({}));
                        inputsCollected = true;
                    }
                }
            }
        }

        // Debug prints
        if (!inputsCollected) {
            std::cout << "[Graph] Warning: No inputs for node ID: " << node->getId() << std::endl;
        } else {
            std::cout << "[Graph] Inputs collected for node ID: " << node->getId() << std::endl;
        }

        // Process the current node only if it has valid inputs
        if (inputsCollected) {
            node->process(inputs);
        }
    }
}




const std::vector<std::shared_ptr<Node>>& NodeGraph::getNodes() const {
    return nodes;
}

const std::vector<std::pair<int, int>>& NodeGraph::getLinks() const {
    return links;
}