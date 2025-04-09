// #include "NodeGraph.h"
// #include <iostream>
// #include <set>

// void NodeGraph::addNode(std::shared_ptr<Node> node) {
//     nodes[node->id] = node;
// }

// void NodeGraph::connectNodes(int fromId, int toId) {
//     edges[fromId].push_back(toId);
// }

// void NodeGraph::evaluate() {
//     std::set<int> visited;

//     for (auto& [id, node] : nodes) {
//         if (visited.find(id) == visited.end()) {
//             std::vector<cv::Mat> dummyInput;
//             std::function<void(int, std::vector<cv::Mat>)> dfs;

//             dfs = [&](int currentId, std::vector<cv::Mat> input) {
//                 if (visited.count(currentId)) return;
//                 visited.insert(currentId);

//                 auto currentNode = nodes[currentId];
//                 cv::Mat output = currentNode->process(input);

//                 for (int nextId : edges[currentId]) {
//                     dfs(nextId, {output});
//                 }
//             };

//             dfs(id, dummyInput);
//         }
//     }

//     std::cout << "Node graph evaluation complete.\n";
// }


#include "NodeGraph.h"

void NodeGraph::addNode(std::shared_ptr<Node> node) {
    nodes.push_back(node);
}

void NodeGraph::addLink(int start_attr, int end_attr) {
    links.emplace_back(start_attr, end_attr);
}

const std::vector<std::shared_ptr<Node>>& NodeGraph::getNodes() const {
    return nodes;
}

const std::vector<std::pair<int, int>>& NodeGraph::getLinks() const {
    return links;
}
