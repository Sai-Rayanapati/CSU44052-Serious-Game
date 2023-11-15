#include "Model.h"
#include "stb_image.h"

Model::Model(int max_instance) 
{
    MAX_INSTANCES = max_instance;
}

void Model::LoadModelInstanced(const std::string& obj_path, const std::string& material_path)
{
    std::string error_msg;
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = ""; 
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(obj_path, reader_config)) 
    {
        if (!reader.Error().empty()) 
        {
            printf("Error loading obj file");
        }
        exit(1);
    }

    tinyobj::attrib_t attrib = reader.GetAttrib();
    std::vector<tinyobj::shape_t> shapes = reader.GetShapes();
    std::vector<tinyobj::material_t> materials = reader.GetMaterials();

    // looping over shapes ( meshes )
    for (size_t s = 0; s < shapes.size(); s++) 
    {
        size_t index_offset = 0;

        //making a temporary vector that will fill each mesh separately
        std::vector<Vertex> tempVector;

        // looping over faces for each shape (mesh)
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) 
        {
            int fv = shapes[s].mesh.num_face_vertices[f];
            
            // loop over vertices of each face 
            for (size_t v = 0; v < fv; v++) 
            {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        
                Vertex vertex;
                vertex.position = glm::vec3(
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                );
                if (attrib.texcoords.size() > 0 && idx.texcoord_index != -1) 
                {
                    vertex.texcoord = glm::vec2(
                        attrib.texcoords[2 * idx.texcoord_index + 0],
                        attrib.texcoords[2 * idx.texcoord_index + 1]
                    );
                }
                if (attrib.normals.size() > 0 && idx.normal_index != -1) 
                {
                    vertex.normal = glm::vec3(
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                    );
                }

                tempVector.push_back(vertex);
            }
        
            index_offset += fv;
        }

        all_vertices.push_back(tempVector);

        // loop over the indices 
        std::vector<int> indices;

        for (size_t f = 0; f < shapes[s].mesh.indices.size() / 3; f++)
        {
            // Push face indices to indices vector
            indices.push_back(shapes[s].mesh.indices[3 * f].vertex_index);
            indices.push_back(shapes[s].mesh.indices[3 * f + 1].vertex_index);
            indices.push_back(shapes[s].mesh.indices[3 * f + 2].vertex_index);
        }
        //This should have a size of 2
        all_indices.push_back(indices);
    }

    std::vector<std::vector<float>> interleaved_data;
    interleaved_data.reserve(vertices_.size() * 8); // Each vertex has 8 floats: 3 position, 2 texcoord, 3 normal

    for (const auto& mesh: all_vertices) {
        std::vector<float> tempInterleavedData;
        for (const auto& vertex : mesh) {
            tempInterleavedData.push_back(vertex.position.x);
            tempInterleavedData.push_back(vertex.position.y);
            tempInterleavedData.push_back(vertex.position.z);
            tempInterleavedData.push_back(vertex.texcoord.x);
            tempInterleavedData.push_back(vertex.texcoord.y);
            tempInterleavedData.push_back(vertex.normal.x);
            tempInterleavedData.push_back(vertex.normal.y);
            tempInterleavedData.push_back(vertex.normal.z);
        }

        interleaved_data.push_back(tempInterleavedData);
    }

    // Extract the material data from the model
    int index = 0;
 
    for (auto& material : materials) 
    {
        int width, height, num_channels;
        std::string texture_path = material_path + "/" + material.diffuse_texname;
        unsigned char* data = stbi_load(texture_path.c_str(), &width, &height, &num_channels, 0);

        if (data) 
        {    
            Texture texture;
            texture.width = width;
            texture.height = height;
            texture.channels = num_channels;
            texture.data = data;
            texture.index = index;

            glGenTextures(1, &texture.id);
            glBindTexture(GL_TEXTURE_2D, texture.id);

            if (num_channels == 3)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            }    
            else
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            textures_.push_back(texture);
        }
        else 
        {
            printf("Error loading texture file");
            exit(1);
        }
        index++;
    }
    
    // initilise meshes 
    for (size_t s = 0; s < shapes.size(); s++) 
    {
        unsigned int VAO, VBO, instanceVBO, IBO;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &instanceVBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, interleaved_data[s].size() * sizeof(float), interleaved_data[s].data(), GL_STATIC_DRAW);

        // Vertex positions
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Texture coordinates
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Normals
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);

        for (int i = 0; i < 4; i++) 
        {
            glEnableVertexAttribArray(3 + i);
            glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
            glVertexAttribDivisor(3 + i, 1);
        }

        glGenBuffers(1, &IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, all_indices[s].size() * sizeof(int), all_indices[s].data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        vbos.push_back(VBO);
        vaos.push_back(VAO);
        ibos.push_back(IBO);
        instance_vbos.push_back(instanceVBO);
    }
}

void Model::DrawInstanced(unsigned int shader_program, const glm::mat4& model_matrix, const glm::mat4& view_matrix, const glm::mat4& projection_matrix, const std::vector<glm::mat4>& model_matrices)
{
    for (size_t i = 0; i < vaos.size(); i++) 
    {
        glUseProgram(shader_program);
        glBindVertexArray(vaos[i]);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
        int textureUnit = 0;

        for (size_t j = 0; j < textures_.size(); j++) 
        {
            if (j == i) 
            {
                glActiveTexture(GL_TEXTURE0 + j);
                glBindTexture(GL_TEXTURE_2D, textures_[j].id);
                std::string u = "diffuseTexture";
                unsigned int loc = glGetUniformLocation(shader_program, u.c_str());
                glUniform1i(loc, j);
                textureUnit++;
            }
        }

        // Set the model, view, and projection matrices
        unsigned int model_location = glGetUniformLocation(shader_program, "model");
        glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model_matrix));

        unsigned int view_location = glGetUniformLocation(shader_program, "view");
        glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view_matrix));

        unsigned int projection_location = glGetUniformLocation(shader_program, "projection");
        glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection_matrix));

        glBindBuffer(GL_ARRAY_BUFFER, instance_vbos[i]);
        glBufferData(GL_ARRAY_BUFFER, model_matrices.size() * sizeof(glm::mat4), &model_matrices[0], GL_STATIC_DRAW);

        // Set the model matrix attribute pointer
        for (size_t i = 0; i < 4; i++) 
        {
            glEnableVertexAttribArray(3 + i);
            glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
            glVertexAttribDivisor(3 + i, 1);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibos[i]);
        glDrawArraysInstanced(GL_TRIANGLES, 0, all_indices[i].size(), model_matrices.size());

        // Disable the attribute pointers
        for (size_t i = 0; i < 4; i++) 
        {
            glDisableVertexAttribArray(3 + i);
        }

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }
}
