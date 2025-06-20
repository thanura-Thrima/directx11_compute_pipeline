#include <gtest/gtest.h>
#include "Hw/Camera.h"



TEST(CameraTest, Initialization) {
    Camera camera("Test Camera");
    EXPECT_TRUE(camera.InitializeCamera());
}


TEST(CameraTest, Running) {
    Camera camera("Test Camera");
    EXPECT_TRUE(camera.InitializeCamera());

    camera.StartCapture();
    std::this_thread::sleep_for(std::chrono::seconds(5)); // Simulate some processing time
    camera.StopCapture();
    std::cout << "Camera stopped successfully." << std::endl;
}

TEST(CameraTest, StopFirst) {
    try{
    Camera camera("Test Camera");
    EXPECT_TRUE(camera.InitializeCamera());
    camera.StopCapture();
    std::cout << "Camera stopped successfully." << std::endl;
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown during StopCapture: " << e.what();
    }
}


TEST(CameraTest, CycleTest) {
    try{
    Camera camera("Test Camera");
    EXPECT_TRUE(camera.InitializeCamera());
    for(int i = 0; i < 30; ++i) {
        camera.StartCapture();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Simulate some processing time
        camera.StopCapture();
    }
    std::cout << "Camera cycle completed successfully." << std::endl;
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown during StopCapture: " << e.what();
    }
}