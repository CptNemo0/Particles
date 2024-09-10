#include <chrono>
#include <vector>
#include <iostream>
#include <cassert>

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


glm::mat4 ViewMatrix(glm::vec3 cameraPos) {
    glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);

    glm::vec3 forward = glm::normalize(cameraPos - camera_target);
    glm::vec3 right = glm::normalize(cross(up, forward));
    glm::vec3 camera_up = glm::cross(forward, right);

    glm::mat4 view = glm::mat4
    (
        glm::vec4(right, 0.0),
        glm::vec4(camera_up, 0.0),
        glm::vec4(forward, 0.0),
        glm::vec4(0.0, 0.0, 0.0, 1.0)
    );

    view = transpose(view);
    view[3] = glm::vec4(-glm::dot(right, cameraPos), -glm::dot(camera_up, cameraPos), -glm::dot(forward, cameraPos), 1.0);

    return view;
}

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
    tinygltf::Model model_;
    LoadModel(model_, "..\\Models\\ball.gltf");
    auto vaoAndEbos = BindModel(model_);

    glm::mat4 view_matrix = ViewMatrix({ 450.0f, 350.0f, 250.0f });
    glm::mat4 projection_matrix = glm::perspective(90.0f, 1600.0f/900.0f, 0.01f, 10000.00f);

    shader.Use();
    shader.SetMatrix4("view_matrix", view_matrix);
    shader.SetMatrix4("projection_matrix", projection_matrix);

    NormalMover mover;
    WallCollisions3D walls{ -200.0f, 200.0f, -200.0f, 200.0f, -200.0f, 200.0f };
    SpatialHashGrid grid;
    BallCollisions2d collisions;
    SOARepository repository = SOARepository(BALL_NUMBER, D3);

    unsigned int position_ssbo;
    unsigned int binding_point = 1;
    glGenBuffers(1, &position_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, position_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * BALL_NUMBER * 3, static_cast<const void*>(&repository.output_position_), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point, position_ssbo);
    
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

        shader.Use();
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, position_ssbo);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * BALL_NUMBER * 3, static_cast<const void*>(&repository.output_position_));

        InstanceModel(vaoAndEbos, model_, BALL_NUMBER);
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
            //std::cout << "fps: " << fps << std::endl;

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


