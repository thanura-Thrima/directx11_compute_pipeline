#include "CameraSink.h"


CameraSink::CameraSink(std::string nodeName)
    : SinkNode(nodeName)
{

    cameraThread = std::thread([this]() {
        while (running) {

            
        }
    });
}

void CameraSink::Execute() {
    // Implementation of the camera sink's execution logic
}