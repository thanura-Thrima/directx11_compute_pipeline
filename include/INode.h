#pragma once
#include <string>
#include <memory>
#include <vector>

class INode
{
    public:
    INode(std::string nodeName) : m_nodeName(nodeName) {}
    virtual ~INode() = default;
    virtual void Execute() = 0; // Method to execute the node's functionality
    public:
    std::vector<std::shared_ptr<INode>> children; // List of child nodes
    std::vector<std::shared_ptr<INode>> parents; // List of parent nodes
    std::string m_nodeName; // Name of the node
};