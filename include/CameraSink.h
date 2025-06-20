#pragma once
#include "Sink.h"

#include <thread>
#include <atomic>

class CameraSink : public SinkNode
{
    public:
    CameraSink(std::string nodeName);
    ~CameraSink() override = default;
    void Execute() override;

    void Start();
    void Stop();
    private:
    std::thread cameraThread; // Thread for camera processing
    std::atomic<bool> running{ true }; // Flag to control the camera thread
};