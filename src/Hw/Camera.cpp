#include "Hw/Camera.h"
#include <iostream>
#include <chrono>

Camera::Camera(std::string cameraName)
    : cameraName(std::move(cameraName))
{
}

Camera::~Camera() {
    StopCapture();
}

void Camera::StartCapture() {
    if(running) {
        std::cerr << "Camera is already running." << std::endl;
        return;
    }   
    running = true;
    cameraThread = std::thread([this]() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(11)); 
        }
        std::cout << "Capturing frame from camera done : " << cameraName << std::endl;
    });
}

void Camera::StopCapture() {
    running = false;
    if (cameraThread.joinable()) {
        cameraThread.join();
    }
}

bool Camera::InitializeCamera() {

    std::cout << "Initializing camera: " << cameraName << std::endl;
    return true;
}

