#pragma once

#include "INode.h"

class SourceNode : public INode
{
    public:
    SourceNode(std::string nodeName);
    ~SourceNode() override = default;
    void Execute() override;
    
};