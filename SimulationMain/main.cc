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

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "../SimulationLogic/glad.h"

#include "tiny_gltf.h"

#include <omp.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

bool LoadModel(tinygltf::Model& model, const char* filename)
{
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
    if (!warn.empty())
    {
        std::cout << "WARN: " << warn << std::endl;
    }

    if (!err.empty())
    {
        std::cout << "ERR: " << err << std::endl;
    }

    if (!res)
    {
        std::cout << "Failed to load glTF: " << filename << std::endl;
    }
    else
    {
        std::cout << "Loaded glTF: " << filename << std::endl;
    }

    return res;
}

void BindMesh(std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh)
{
    int buffer_view_number = static_cast<int>(model.bufferViews.size());
    for (int i = 0; i < buffer_view_number; ++i)
    {
        const tinygltf::BufferView& buffer_view = model.bufferViews[i];
        if (buffer_view.target == 0)
        {
            std::cout << "WARN: bufferView.target is zero" << std::endl;
            continue;
        }

        const tinygltf::Buffer& buffer = model.buffers[buffer_view.buffer];
        std::cout << "bufferview.target " << buffer_view.target << std::endl;

        GLuint vbo;
        glGenBuffers(1, &vbo);
        vbos[i] = vbo;
        glBindBuffer(buffer_view.target, vbo);

        std::cout << "buffer.data.size = " << buffer.data.size()
            << ", bufferview.byteOffset = " << buffer_view.byteOffset
            << std::endl;

        glBufferData(buffer_view.target, buffer_view.byteLength, &buffer.data.at(0) + buffer_view.byteOffset, GL_STATIC_DRAW);
    }

    int primitives_number = static_cast<int>(mesh.primitives.size());
    for (int i = 0; i < primitives_number; ++i)
    {
        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor index_accessor = model.accessors[primitive.indices];

        for (auto& attrib : primitive.attributes)
        {
            tinygltf::Accessor accessor = model.accessors[attrib.second];
            int byte_stride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);

            glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

            int size = 1;
            if (accessor.type != TINYGLTF_TYPE_SCALAR)
            {
                size = accessor.type;
            }

            int vaa = -1;
            if (attrib.first.compare("POSITION") == 0) vaa = 0;
            if (attrib.first.compare("NORMAL") == 0) vaa = 1;
            if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
            if (vaa > -1)
            {
                glEnableVertexAttribArray(vaa);
                glVertexAttribPointer(vaa, size, accessor.componentType, accessor.normalized ? GL_TRUE : GL_FALSE, byte_stride, BUFFER_OFFSET(accessor.byteOffset));
            }
            else
            {
                std::cout << "vaa missing: " << attrib.first << std::endl;
            }
        }

        int texture_number = static_cast<int>(model.textures.size());
        if (texture_number > 0)
        {
            // fixme: Use material's baseColor
            tinygltf::Texture& tex = model.textures[0];

            if (tex.source > -1)
            {

                GLuint texid;
                glGenTextures(1, &texid);

                tinygltf::Image& image = model.images[tex.source];

                glBindTexture(GL_TEXTURE_2D, texid);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                GLenum format = GL_RGBA;

                if (image.component == 1)
                {
                    format = GL_RED;
                }
                else if (image.component == 2)
                {
                    format = GL_RG;
                }
                else if (image.component == 3)
                {
                    format = GL_RGB;
                }

                GLenum type = GL_UNSIGNED_BYTE;
                if (image.bits == 8)
                {
                    // ok
                }
                else if (image.bits == 16)
                {
                    type = GL_UNSIGNED_SHORT;
                }

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, format, type, &image.image.at(0));
            }
        }
    }
}

void BindModelNodes(std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Node& node)
{
    if ((node.mesh >= 0) && (node.mesh < static_cast<int>(model.meshes.size())))
    {
        BindMesh(vbos, model, model.meshes[node.mesh]);
    }

    for (int i = 0; i < node.children.size(); i++)
    {
        assert((node.children[i] >= 0) && (node.children[i] < static_cast<int>(model.nodes.size())));
        BindModelNodes(vbos, model, model.nodes[node.children[i]]);
    }
}

std::pair<GLuint, std::map<int, GLuint>> BindModel(tinygltf::Model& model)
{
    std::map<int, GLuint> vbos;
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    int nodes_number = static_cast<int>(scene.nodes.size());
    for (int i = 0; i < nodes_number; ++i)
    {
        assert((scene.nodes[i] >= 0) && (scene.nodes[i] < static_cast<int>(model.nodes.size())));
        BindModelNodes(vbos, model, model.nodes[scene.nodes[i]]);
    }

    glBindVertexArray(0);

    for (auto it = vbos.cbegin(); it != vbos.cend();)
    {
        tinygltf::BufferView bufferView = model.bufferViews[it->first];
        if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER)
        {
            glDeleteBuffers(1, &vbos[it->first]);
            vbos.erase(it++);
        }
        else
        {
            ++it;
        }
    }

    return { vao, vbos };
}

void DrawMesh(const std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh)
{
    int primitives_num = static_cast<int>(mesh.primitives.size());
    for (int i = 0; i < primitives_num; ++i)
    {
        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.at(indexAccessor.bufferView));

        glDrawElementsInstanced(primitive.mode,
            static_cast<int>(indexAccessor.count),
            indexAccessor.componentType,
            BUFFER_OFFSET(indexAccessor.byteOffset), BALL_NUMBER);
    }
}

// recursively draw node and children nodes of model
void DrawModelNodes(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos, tinygltf::Model& model, tinygltf::Node& node)
{
    if ((node.mesh >= 0) && (node.mesh < static_cast<int>(model.meshes.size())))
    {
        DrawMesh(vaoAndEbos.second, model, model.meshes[node.mesh]);
    }
    for (size_t i = 0; i < node.children.size(); i++) {
        DrawModelNodes(vaoAndEbos, model, model.nodes[node.children[i]]);
    }
}

void DrawModel(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos, tinygltf::Model& model)
{
    glBindVertexArray(vaoAndEbos.first);

    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    int node_number = static_cast<int>(scene.nodes.size());
    for (int i = 0; i < node_number; ++i)
    {
        DrawModelNodes(vaoAndEbos, model, model.nodes[scene.nodes[i]]);
    }

    glBindVertexArray(0);
}

void DebugModel(tinygltf::Model& model)
{
    for (auto& mesh : model.meshes)
    {
        std::cout << "mesh : " << mesh.name << std::endl;
        for (auto& primitive : mesh.primitives)
        {
            const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];

            std::cout << "indexaccessor: count " << indexAccessor.count << ", type " << indexAccessor.componentType << std::endl;

            tinygltf::Material& mat = model.materials[primitive.material];
            for (auto& mats : mat.values)
            {
                std::cout << "mat : " << mats.first.c_str() << std::endl;
            }

            for (auto& image : model.images)
            {
                std::cout << "image name : " << image.uri << std::endl;
                std::cout << "  size : " << image.image.size() << std::endl;
                std::cout << "  w/h : " << image.width << "/" << image.height << std::endl;

            }

            std::cout << "indices : " << primitive.indices << std::endl;
            std::cout << "mode     : " << "(" << primitive.mode << ")" << std::endl;

            for (auto& attrib : primitive.attributes)
            {
                std::cout << "attribute : " << attrib.first.c_str() << std::endl;
            }
        }
    }
}

glm::mat4 ViewMatrix(glm::vec3 cameraPos) {
    glm::vec3 camera_target = glm::vec3(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.5f, 0.0);
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

    Shader shader("C:\\Users\\pawel\\Desktop\\Programowanie\\RaylibFun\\Shaders\\BasicShader.vert", "C:\\Users\\pawel\\Desktop\\Programowanie\\RaylibFun\\Shaders\\BasicShader.frag");
    tinygltf::Model model_;
    LoadModel(model_, "C:\\Users\\pawel\\Desktop\\Programowanie\\RaylibFun\\Models\\ball.gltf");
    auto vaoAndEbos = BindModel(model_);

    glm::vec4 view_matrix_c1 = glm::vec4(1.0, 0.0, 0.0, 0.0);
    glm::vec4 view_matrix_c2 = glm::vec4(0.0, 1.0, 0.0, 0.0);
    glm::vec4 view_matrix_c3 = glm::vec4(0.0, 0.0, -1.0, 0.0);
    glm::vec4 view_matrix_c4 = glm::vec4(0.0, 0.0, -1.0, 1.0);
    glm::mat4 view_matrix(view_matrix_c1, view_matrix_c2, view_matrix_c3, view_matrix_c4);
    //glm::mat4 view_matrix = ViewMatrix({ 800.0, 450.0, -300.0 });
    glm::mat4 projection_matrix = glm::orthoRH(0.0, 1600.0, 900.0, 0.0, 0.01, 1000.00);
    //glm::mat4 projection_matrix = glm::perspective(180.0f, 1600.0f/900.0f, 0.01f, 1000.00f);
    shader.Use();
    shader.SetMatrix4("view_matrix", view_matrix);
    shader.SetMatrix4("projection_matrix", projection_matrix);

    NormalMover mover;
    LinearWallCollisions walls{ 30.0f, WINDOW_HEIGHT - 30.0f, 30.0f, WINDOW_WIDTH - 30.0f };
    SpatialHashGrid grid;
    BallCollisions collisions;
    SOARepository repository{ BALL_NUMBER };

    unsigned int position_ssbo;
    unsigned int binding_point = 1;
    glGenBuffers(1, &position_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, position_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 2 * BALL_NUMBER, static_cast<const void*>(&repository.output_position_), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point, position_ssbo);

    assert(!(BALL_NUMBER % SIMD_BLOCK_SIZE));

    std::vector<double> durations_;
    int idx = 0;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.37f, 0.55f, 0.54f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

        mover.UpdateVelocities(repository);
        mover.PredictPositions(repository);

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        repository.nx_[0] = static_cast<float>(xpos);
        repository.ny_[0] = static_cast<float>(ypos);
        repository.radius_[0] = 20.0f;

        grid.UpdateGrid(repository);
        collisions.SeperateBalls(grid, repository);
        walls.CollideWalls(repository);
        walls.CollideWalls(repository);
        mover.UpdatePositions(repository);

        //DRAW
        shader.Use();
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, position_ssbo);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * BALL_NUMBER * 2, static_cast<const void*>(&repository.output_position_));

        DrawModel(vaoAndEbos, model_);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

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

        idx++;

        glfwSwapBuffers(window);
    }
    glfwTerminate();

    return 0;
}


