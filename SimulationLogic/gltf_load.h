#pragma once

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#endif // !BUFFER_OFFSET

#include "tiny_gltf.h"
#include <iostream>
#include "glad.h"

inline bool LoadModel(tinygltf::Model& model, const char* filename)
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

inline void BindMesh(std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh)
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

inline void BindModelNodes(std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Node& node)
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

inline std::pair<GLuint, std::map<int, GLuint>> BindModel(tinygltf::Model& model)
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

inline void DrawMesh(const std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh)
{
    int primitives_num = static_cast<int>(mesh.primitives.size());
    for (int i = 0; i < primitives_num; ++i)
    {
        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.at(indexAccessor.bufferView));

        glDrawElements(primitive.mode,
            static_cast<int>(indexAccessor.count),
            indexAccessor.componentType,
            BUFFER_OFFSET(indexAccessor.byteOffset));
    }
}

inline void DrawModelNodes(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos, tinygltf::Model& model, tinygltf::Node& node)
{
    if ((node.mesh >= 0) && (node.mesh < static_cast<int>(model.meshes.size())))
    {
        DrawMesh(vaoAndEbos.second, model, model.meshes[node.mesh]);
    }
    for (size_t i = 0; i < node.children.size(); i++) {
        DrawModelNodes(vaoAndEbos, model, model.nodes[node.children[i]]);
    }
}

inline void DrawModel(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos, tinygltf::Model& model)
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

inline void InstanceMesh(const std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh, const unsigned int instnace_num)
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
            BUFFER_OFFSET(indexAccessor.byteOffset),
            instnace_num);
    }
}

inline void InstanceDrawModelNodes(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos, tinygltf::Model& model, tinygltf::Node& node, const unsigned int instnace_num)
{
    if ((node.mesh >= 0) && (node.mesh < static_cast<int>(model.meshes.size())))
    {
        InstanceMesh(vaoAndEbos.second, model, model.meshes[node.mesh], instnace_num);
    }
    for (size_t i = 0; i < node.children.size(); i++) {
        InstanceDrawModelNodes(vaoAndEbos, model, model.nodes[node.children[i]], instnace_num);
    }
}

inline void InstanceModel(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos, tinygltf::Model& model, const unsigned int instnace_num)
{
    glBindVertexArray(vaoAndEbos.first);

    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    int node_number = static_cast<int>(scene.nodes.size());
    for (int i = 0; i < node_number; ++i)
    {
        InstanceDrawModelNodes(vaoAndEbos, model, model.nodes[scene.nodes[i]], instnace_num);
    }

    glBindVertexArray(0);
}

inline void DebugModel(tinygltf::Model& model)
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
