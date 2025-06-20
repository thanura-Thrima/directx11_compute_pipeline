#include "ComputeGraph.h"
#include "INode.h"

#include <algorithm>
#include <iostream>

ComputeGraph::ComputeGraph() {
    // Constructor implementation if needed
}

void ComputeGraph::AddNode(std::string nodeName, std::shared_ptr<INode> node) {
    nodes[nodeName] = node;
}

void ComputeGraph::RemoveNode(std::string nodeName) {
    nodes.erase(nodeName);
    adjacencyList.erase(nodeName);
    processed.erase(nodeName);
}

void ComputeGraph::AddEdge(std::string nodeFrom, std::string nodeTo) {
    if (nodes.find(nodeFrom) != nodes.end() && nodes.find(nodeTo) != nodes.end()) {
         nodes[nodeFrom]->children.push_back(nodes[nodeTo]);
         nodes[nodeTo]->parents.push_back(nodes[nodeFrom]);
    } else {
        std::cerr << "Error: One or both nodes not found in the graph." << std::endl;

    }
}

void ComputeGraph::RemoveEdge(std::string nodeFrom, std::string nodeTo) {
    if (nodes.find(nodeFrom) != nodes.end() && nodes.find(nodeTo) != nodes.end()) {
        auto& children = nodes[nodeFrom]->children;
        children.erase(std::remove(children.begin(), children.end(), nodes[nodeTo]), children.end());
        auto& parents = nodes[nodeTo]->parents;
        parents.erase(std::remove(parents.begin(), parents.end(), nodes[nodeFrom]), parents.end());
    }
}

void ComputeGraph::Execute() {
}

void ComputeGraph::Clear() {
    processed.clear();
}

void ComputeGraph::TopologicalSortUtil(const std::shared_ptr<INode>& node, std::unordered_set<std::string>& visited, std::stack<std::string>& stack) {
    visited.insert(node->m_nodeName);

    for (auto child : node->children) {
        if (visited.find(child->m_nodeName) == visited.end()) {
            TopologicalSortUtil(child, visited, stack);
        }
    }

    stack.push(node->m_nodeName);
}

void ComputeGraph::TopologicalSortAndExecute() {
    std::unordered_set<std::string> visited;
    std::stack<std::string> tempMark;

    for (const auto& pair : nodes) {
        if (visited.find(pair.first) == visited.end()) {
            TopologicalSortUtil(pair.second, visited, tempMark);
        }
    }

    for (const auto& node : processed) {
        nodes[node]->Execute();
    }
}

bool ComputeGraph::CanProcessNode(const std::shared_ptr<INode>& node) const {
    // Implement logic to determine if a node can be processed
    return true;
}


void ComputeGraph::Initialize() {
    // Initialize the graph, if needed
    Clear();
    nodes.clear();
}
