#pragma once

#include "INode.h"

class ComputeNode : public INode
{
    public:
    ComputeNode(std::string nodeName);
    ~ComputeNode() override = default;
    void Execute() override;
};