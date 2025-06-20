#pragma once
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <stack>

class INode;
class ComputeGraph
{
public:
    void AddNode(std::string nodeName, std::shared_ptr<INode> node);
    void RemoveNode(std::string nodeName);
    void AddEdge(std::string nodeFrom, std::string nodeTo);
    void RemoveEdge(std::string nodeFrom, std::string nodeTo);
    void Initialize();
    void Execute();
    void Clear();
    void TopologicalSortAndExecute();

private:
    bool CanProcessNode(const std::shared_ptr<INode>& node) const;
    void TopologicalSortUtil(const std::shared_ptr<INode>& node, std::unordered_set<std::string>& visited,
        std::stack<std::string>& stack);

private:
    std::unordered_map<std::string, std::shared_ptr<INode>> nodes; // Maps node ID to node object
    std::unordered_set<std::string> processed; // Set to track processed nodes
};