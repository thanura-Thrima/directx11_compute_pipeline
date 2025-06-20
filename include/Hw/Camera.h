#pragma once

#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>

class Camera
{
public:
    Camera(std::string cameraName);
    ~Camera();
    void StartCapture();
    void StopCapture(); 
    bool InitializeCamera();
private:
    std::string cameraName;
    std::thread cameraThread; // Thread for camera processing
    std::atomic<bool> running{ false }; // Flag to control the camera thread
    std::condition_variable cv; // Condition variable for thread synchronization
    std::mutex mtx; // Mutex for thread safety
};