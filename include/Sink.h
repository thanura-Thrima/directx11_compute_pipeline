#pragma once

#include "INode.h"


class SinkNode : public INode
{
    public:
    SinkNode(std::string nodeName);
    ~SinkNode() override = default;
    void Execute() override;
};