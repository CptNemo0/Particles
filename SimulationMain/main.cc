#include <chrono>
#include <vector>
#include <iostream>
#include <cassert>

#include "../SimulationLogic/BallRenderer.h"
#include "../SimulationLogic/BallRepository.h"
#include "../SimulationLogic/BallCollisions.h"
#include "../SimulationLogic/Mover.h"
#include "../SimulationLogic/global.h"
#include "../SimulationLogic/SpatialHashGrid.h"
#include "../SimulationLogic/Shader.h"
#include "../SimulationLogic/WallCollisions.h"

#include "../SimulationLogic/gltf_load.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "../SimulationLogic/glad.h"

#include <omp.h>

int main(int argv, const char** argc)
{
#pragma region Setup
    GLFWwindow* window = nullptr;
    GLFWmonitor* monitor = nullptr;
    GLFWvidmode* mode = nullptr;

    if (!glfwInit())
    {
        exit(-1);
    }

    monitor = glfwGetPrimaryMonitor();
    mode = (GLFWvidmode*)glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);

    if (window == nullptr)
    {
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        exit(-1);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);

    omp_set_num_threads(4);
    omp_set_dynamic(0);
#pragma endregion

    Shader shader("..\\Shaders\\BasicShader.vert", "..\\Shaders\\BasicShader.frag");
    NormalMover mover;
    WallCollisions3D walls { -200.0f, 200.0f, -200.0f, 200.0f, -200.0f, 200.0f };
    SpatialHashGrid grid;
    BallCollisions2d collisions;
    SOARepository repository = SOARepository(BALL_NUMBER, D3);
    BallRenderer renderer{ &repository, &shader };
    renderer.UpdateProjectionPatrix(glm::perspective(45.0f, 1600.0f / 900.0f, 0.01f, 10000.00f));
    renderer.UpdateViewMatrix({ 350.0f, 350.0f, 350.0f }, { 0.0f, 0.0f, 0.0f });
    
    assert(!(BALL_NUMBER % SIMD_BLOCK_SIZE));

    std::vector<double> durations_;
    int idx = 0;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.175f, 0.2525f, 0.27f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

        mover.UpdateVelocities(repository);
        mover.PredictPositions(repository);
        walls.CollideWalls(repository);
        mover.UpdatePositions(repository);
        renderer.Draw();
        
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        
#pragma region FPS
        durations_.push_back(static_cast<double>(chrono::duration_cast<chrono::microseconds>(end - begin).count()));
        if (idx == 100)
        {
            auto sum = [](std::vector<double>& durations)
                {
                    double return_value = 0.0;
                    for (auto duration : durations)
                    {
                        return_value += duration;
                    }
                    return return_value;
                };
            auto result = sum(durations_);
            result *= 0.01;
            auto fps = 1000000.0 / result;
            std::cout << "fps: " << fps << std::endl;

            idx = 0;
            durations_.clear();
        }
        else
        {
            idx++;
        }
#pragma endregion
        glfwSwapBuffers(window);
    }
    glfwTerminate();

    return 0;
}


