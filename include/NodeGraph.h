
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

    void processNodes();

private:
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<std::pair<int, int>> links;
};