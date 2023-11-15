#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "tiny_obj_loader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>

class Model 
{
public:
    Model(int max_instance);

    void LoadModelInstanced(const std::string& obj_path, const std::string& material_path);

    void DrawInstanced(unsigned int shader_program, const glm::mat4& model_matrix, const glm::mat4& view_matrix, const glm::mat4& projection_matrix, const std::vector<glm::mat4> & model_matrices);

private:

    struct Texture 
    {
        int index;
        unsigned int id;
        int width;
        int height;
        int channels;
        unsigned char* data;
    };
    
    std::vector<Texture> textures_;

    int MAX_INSTANCES;
    
    struct Vertex 
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;
    };
    
    std::vector<Vertex> vertices_;
    std::vector<std::vector<Vertex>> all_vertices;
    std::vector<std::vector<int>> all_indices;

    int noOfVertices = 0;

    std::vector<unsigned int> vbos;
    std::vector<unsigned int> ibos;
    std::vector<unsigned int> vaos;
    std::vector<unsigned int> instance_vbos;
};
