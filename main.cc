#include <chrono>
#include <vector>
#include <iostream>
#include <cassert>

#include "Logic/BallManager.h"
#include "Logic/BallRepository.h"
#include "Logic/Mover.h"
#include "Logic/global.h"
#include "Logic/IDManager.h"
#include "Logic/WallCollisions.h"
#include "Logic/SpatialHashGrid.h"
#include <omp.h>
#include "Logic/BallCollisions.h"
#include "Logic/Shader.h"
#include "Logic/glad.h"
#include "GLFW/glfw3.h"
#include <numeric>
#include "tiny_gltf.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

bool loadModel(tinygltf::Model& model, const char* filename) {
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
    if (!warn.empty()) {
        std::cout << "WARN: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cout << "ERR: " << err << std::endl;
    }

    if (!res)
        std::cout << "Failed to load glTF: " << filename << std::endl;
    else
        std::cout << "Loaded glTF: " << filename << std::endl;

    return res;
}

void bindMesh(std::map<int, GLuint>& vbos,
    tinygltf::Model& model, tinygltf::Mesh& mesh) {
    for (size_t i = 0; i < model.bufferViews.size(); ++i)
    {
        const tinygltf::BufferView& bufferView = model.bufferViews[i];
        if (bufferView.target == 0)
        {  // TODO impl drawarrays
            std::cout << "WARN: bufferView.target is zero" << std::endl;
            continue;  // Unsupported bufferView.
            /*
              From spec2.0 readme:
              https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
                       ... drawArrays function should be used with a count equal to
              the count            property of any of the accessors referenced by the
              attributes            property            (they are all equal for a given
              primitive).
            */
        }

        const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
        std::cout << "bufferview.target " << bufferView.target << std::endl;

        GLuint vbo;
        glGenBuffers(1, &vbo);
        vbos[i] = vbo;
        glBindBuffer(bufferView.target, vbo);

        std::cout << "buffer.data.size = " << buffer.data.size()
            << ", bufferview.byteOffset = " << bufferView.byteOffset
            << std::endl;

        glBufferData(bufferView.target, bufferView.byteLength,
            &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
    }

    for (size_t i = 0; i < mesh.primitives.size(); ++i)
    {
        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

        for (auto& attrib : primitive.attributes)
        {
            tinygltf::Accessor accessor = model.accessors[attrib.second];
            int byteStride =
                accessor.ByteStride(model.bufferViews[accessor.bufferView]);
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
                glVertexAttribPointer(vaa, size, accessor.componentType,
                    accessor.normalized ? GL_TRUE : GL_FALSE,
                    byteStride, BUFFER_OFFSET(accessor.byteOffset));
            }
            else
                std::cout << "vaa missing: " << attrib.first << std::endl;
        }

        if (model.textures.size() > 0) {
            // fixme: Use material's baseColor
            tinygltf::Texture& tex = model.textures[0];

            if (tex.source > -1) {

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

                if (image.component == 1) {
                    format = GL_RED;
                }
                else if (image.component == 2) {
                    format = GL_RG;
                }
                else if (image.component == 3) {
                    format = GL_RGB;
                }
                else {
                    // ???
                }

                GLenum type = GL_UNSIGNED_BYTE;
                if (image.bits == 8) {
                    // ok
                }
                else if (image.bits == 16) {
                    type = GL_UNSIGNED_SHORT;
                }
                else {
                    // ???
                }

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
                    format, type, &image.image.at(0));
            }
        }
    }
}

// bind models
void bindModelNodes(std::map<int, GLuint>& vbos, tinygltf::Model& model,
    tinygltf::Node& node) {
    if ((node.mesh >= 0) && (node.mesh < model.meshes.size()))
    {
        bindMesh(vbos, model, model.meshes[node.mesh]);
    }

    for (size_t i = 0; i < node.children.size(); i++) {
        assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
        bindModelNodes(vbos, model, model.nodes[node.children[i]]);
    }
}

std::pair<GLuint, std::map<int, GLuint>> bindModel(tinygltf::Model& model) {
    std::map<int, GLuint> vbos;
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i)
    {
        assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
        bindModelNodes(vbos, model, model.nodes[scene.nodes[i]]);
    }

    glBindVertexArray(0);
    // cleanup vbos but do not delete index buffers yet
    for (auto it = vbos.cbegin(); it != vbos.cend();) {
        tinygltf::BufferView bufferView = model.bufferViews[it->first];
        if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER) {
            glDeleteBuffers(1, &vbos[it->first]);
            vbos.erase(it++);
        }
        else {
            ++it;
        }
    }

    return { vao, vbos };
}

void drawMesh(const std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh)
{

    for (size_t i = 0; i < mesh.primitives.size(); ++i)
    {
        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.at(indexAccessor.bufferView));

        glDrawElementsInstanced(primitive.mode,
            indexAccessor.count,
            indexAccessor.componentType,
            BUFFER_OFFSET(indexAccessor.byteOffset), BALL_NUMBER);
    }
}

// recursively draw node and children nodes of model
void drawModelNodes(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos,
    tinygltf::Model& model, tinygltf::Node& node) {
    if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
        drawMesh(vaoAndEbos.second, model, model.meshes[node.mesh]);
    }
    for (size_t i = 0; i < node.children.size(); i++) {
        drawModelNodes(vaoAndEbos, model, model.nodes[node.children[i]]);
    }
}
void drawModel(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos,
    tinygltf::Model& model) {
    glBindVertexArray(vaoAndEbos.first);

    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        drawModelNodes(vaoAndEbos, model, model.nodes[scene.nodes[i]]);
    }

    glBindVertexArray(0);
}

void dbgModel(tinygltf::Model& model) {
    for (auto& mesh : model.meshes) {
        std::cout << "mesh : " << mesh.name << std::endl;
        for (auto& primitive : mesh.primitives) {
            const tinygltf::Accessor& indexAccessor =
                model.accessors[primitive.indices];

            std::cout << "indexaccessor: count " << indexAccessor.count << ", type "
                << indexAccessor.componentType << std::endl;

            tinygltf::Material& mat = model.materials[primitive.material];
            for (auto& mats : mat.values) {
                std::cout << "mat : " << mats.first.c_str() << std::endl;
            }

            for (auto& image : model.images) {
                std::cout << "image name : " << image.uri << std::endl;
                std::cout << "  size : " << image.image.size() << std::endl;
                std::cout << "  w/h : " << image.width << "/" << image.height
                    << std::endl;
            }

            std::cout << "indices : " << primitive.indices << std::endl;
            std::cout << "mode     : "
                << "(" << primitive.mode << ")" << std::endl;

            for (auto& attrib : primitive.attributes) {
                std::cout << "attribute : " << attrib.first.c_str() << std::endl;
            }
        }
    }
}

glm::mat4 ViewMatrix(glm::vec3 cameraPos) {
    glm::vec3 camera_target = glm::vec3(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.5f, 0.0);  // Camera looks at the origin
    glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);            // Up direction is along Y-axis

    // Forward, right, and up vectors
    glm::vec3 forward = glm::normalize(cameraPos - camera_target);
    glm::vec3 right = glm::normalize(cross(up, forward));
    glm::vec3 camera_up = glm::cross(forward, right);

    // Build the view matrix
    glm::mat4 view = glm::mat4(
        glm::vec4(right, 0.0),
        glm::vec4(camera_up, 0.0),
        glm::vec4(forward, 0.0),
        glm::vec4(0.0, 0.0, 0.0, 1.0)
    );

    // Apply camera position
    view = transpose(view); // To convert the matrix into a column-major order
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

    Shader shader("C:\\Users\\pawel\\Desktop\\Programowanie\\RaylibFun\\BasicShader.vert", "C:\\Users\\pawel\\Desktop\\Programowanie\\RaylibFun\\BasicShader.frag");
    tinygltf::Model model_;
    loadModel(model_, "C:\\Users\\pawel\\Desktop\\Programowanie\\RaylibFun\\Models\\ball.gltf");
    auto vaoAndEbos = bindModel(model_);
    
    glm::vec4 view_matrix_c1 = glm::vec4(1.0, 0.0,  0.0, 0.0);
    glm::vec4 view_matrix_c2 = glm::vec4(0.0, 1.0,  0.0, 0.0);
    glm::vec4 view_matrix_c3 = glm::vec4(0.0, 0.0, -1.0, 0.0);
    glm::vec4 view_matrix_c4 = glm::vec4(800.0, 450.0, -100.0, 1.0);
    //glm::mat4 view_matrix(view_matrix_c1, view_matrix_c2, view_matrix_c3, view_matrix_c4);
    glm::mat4 view_matrix = ViewMatrix({ 800.0, 450.0, -300.0 });
    //glm::mat4 projection_matrix = glm::orthoRH(0.0, 1600.0, 900.0, 0.0, 0.01, 1000.00);
    glm::mat4 projection_matrix = glm::perspective(180.0f, 1600.0f/900.0f, 0.01f, 1000.00f);
    shader.Use();
    shader.SetMatrix4("view_matrix", view_matrix);
    shader.SetMatrix4("projection_matrix", projection_matrix);

    NormalMover mover;
    LinearWallCollisions walls {30.0f, WINDOW_HEIGHT - 30.0f, 30.0f, WINDOW_WIDTH - 30.0f};
    SpatialHashGrid grid;
    BallCollisions collisions;
    SOARepository repository { BALL_NUMBER };
    
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
        glClearColor(0.37f, 0.55, 0.54, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        
        mover.UpdateVelocities(repository);
        mover.PredictPositions(repository);

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        repository.nx_[0] = xpos;
        repository.ny_[0] = ypos;
        repository.radius_[0] = 20;

        grid.UpdateGrid(repository);        
        collisions.SeperateBalls(grid, repository);
        walls.CollideWalls(repository);
        walls.CollideWalls(repository);
        mover.UpdatePositions(repository);

        //DRAW
        shader.Use();
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, position_ssbo);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * BALL_NUMBER * 2, static_cast<const void*>(&repository.output_position_));
        
        drawModel(vaoAndEbos, model_);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        
        durations_.push_back(static_cast<double>(chrono::duration_cast<chrono::microseconds>(end - begin).count()));
        
        if (idx == 100)
        {
            auto a = std::accumulate(durations_.begin(), durations_.end(), 0);
            auto fps = 100000000.0 / a;
            std::cout << "fps: " << fps << std::endl;
            std::cout << repository.x_[BALL_NUMBER / 2] << " " << repository.y_[BALL_NUMBER / 2] << std::endl;
            idx = 0;
            durations_.clear();
        }

        idx++;
        
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    
    return 0;
}


