#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

// GL Includes
#define GLEW_STATIC
#include "GLEW/glew.h" // Contains all the necessery OpenGL includes
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shader.h"
using namespace std;
struct Vertex {
    // Position
    glm::vec3 Position;
    // Normal
    glm::vec3 Normal;
    // TexCoords
    glm::vec2 TexCoords;
    // Tangent
    glm::vec3 Tangent;
    // Bitangent
    glm::vec3 Bitangent;
};

struct Texture {
    GLuint id;
    string type;
    aiString path;
};

class Mesh {
public:
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;
    GLuint VAO;
	
	GLuint m_VAO;
	GLuint m_VBOpos, m_VBOcolor, m_VBOnormal, m_VBOtex2d, m_VBOtex3d;
	vector<GLuint> m_texs3d;
	vector<GLfloat> m_pos3d;
	vector<GLfloat> m_normals;

    /*  Functions  */
    // Constructor
    Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // Now that we have all the required data, set the vertex buffers and its attribute pointers.

		// 
		this->initVertex();
        this->setupMesh();
    }
	void initVertex()
	{
		int  nVertex = indices.size();
		m_pos3d.resize(3 * nVertex);
		m_texs3d.resize(3 * nVertex);
		m_normals.resize(3 * nVertex);
		int i0,i1,i2;
		for (int k = 0; k < nVertex; k+=3)
		{
			// n faces -->  3n vertices    --->   3n tex3ds     --->  3n*3  elements (float)
			
			i0 = indices[k];
			i1 = indices[k + 1];
			i2 = indices[k + 2];

			//pos
			m_pos3d[3 * k + 0] = vertices[i0].Position.x;
			m_pos3d[3 * k + 1] = vertices[i0].Position.y;
			m_pos3d[3 * k + 2] = vertices[i0].Position.z;

			m_pos3d[3 * k + 3] = vertices[i1].Position.x;
			m_pos3d[3 * k + 4] = vertices[i1].Position.y;
			m_pos3d[3 * k + 5] = vertices[i1].Position.z;

			m_pos3d[3 * k + 6] = vertices[i2].Position.x;
			m_pos3d[3 * k + 7] = vertices[i2].Position.y;
			m_pos3d[3 * k + 8] = vertices[i2].Position.z;

			//norm
			m_normals[3 * k + 0] = vertices[i0].Normal.x;
			m_normals[3 * k + 1] = vertices[i0].Normal.y;
			m_normals[3 * k + 2] = vertices[i0].Normal.z;

			m_normals[3 * k + 3] = vertices[i1].Normal.x;
			m_normals[3 * k + 4] = vertices[i1].Normal.y;
			m_normals[3 * k + 5] = vertices[i1].Normal.z;

			m_normals[3 * k + 6] = vertices[i2].Normal.x;
			m_normals[3 * k + 7] = vertices[i2].Normal.y;
			m_normals[3 * k + 8] = vertices[i2].Normal.z;
			
			//texs3d
			
			m_texs3d[3 * k + 0] = i0;
			m_texs3d[3 * k + 1] = i1;
			m_texs3d[3 * k + 2] = i2;

			m_texs3d[3 * k + 3] = i0;
			m_texs3d[3 * k + 4] = i1;
			m_texs3d[3 * k + 5] = i2;

			m_texs3d[3 * k + 6] = i0;
			m_texs3d[3 * k + 7] = i1;
			m_texs3d[3 * k + 8] = i2;
		}
			
	}
    // Render the mesh
    void Draw(Shader shader) 
    {
        // Bind appropriate textures
        GLuint diffuseNr = 1;
        GLuint specularNr = 1;
        GLuint normalNr = 1;
        GLuint heightNr = 1;
        for(GLuint i = 0; i < this->textures.size(); i++)
        {
			break;
            glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
            // Retrieve texture number (the N in diffuse_textureN)
            stringstream ss;
            string number;
            string name = this->textures[i].type;
            if(name == "texture_diffuse")
                ss << diffuseNr++; // Transfer GLuint to stream
            else if(name == "texture_specular")
                ss << specularNr++; // Transfer GLuint to stream
            else if(name == "texture_normal")
                ss << normalNr++; // Transfer GLuint to stream
             else if(name == "texture_height")
                ss << heightNr++; // Transfer GLuint to stream
            number = ss.str(); 
            // Now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.Program, (name + number).c_str()), i);
            // And finally bind the texture
            glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
        }
        
//         // Draw mesh
//         glBindVertexArray(this->VAO);
//         glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
// 		glBindVertexArray(0);

		glBindVertexArray(m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, this->indices.size());
		glBindVertexArray(0);

        // Always good practice to set everything back to defaults once configured.
        for (GLuint i = 0; i < this->textures.size(); i++)
        {
			break;
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

private:
    /*  Render data  */
    GLuint VBO, EBO;

    /*  Functions    */
    // Initializes all the buffer objects/arrays
    void setupMesh()
    {
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBOpos);
		glGenBuffers(1, &m_VBOnormal);
		glGenBuffers(1, &m_VBOcolor);
		glGenBuffers(1, &m_VBOtex2d);
		glGenBuffers(1, &m_VBOtex3d);
		glBindVertexArray(m_VAO);
		
		// vertex pos
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOpos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_pos3d.size(), m_pos3d.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(INDEX_VERTEX);
		glVertexAttribPointer(INDEX_VERTEX, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

		// vertex norm
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOnormal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m_normals.size(), m_normals.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(INDEX_NORMAL);
		glVertexAttribPointer(INDEX_NORMAL, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

		//vertex tex3d
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOtex3d);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) *m_texs3d.size(), m_texs3d.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(INDEX_TEX3D);
		glVertexAttribIPointer(INDEX_TEX3D, 3, GL_UNSIGNED_INT, 3 * sizeof(GLuint), (GLvoid*)0);

		glBindVertexArray(0);










		return;
#ifdef USE_OLD
        // Create buffers/arrays
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
        glGenBuffers(1, &this->EBO);

        glBindVertexArray(this->VAO);
        // Load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);  

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

        // Set the vertex attribute pointers
        // Vertex Positions
        glEnableVertexAttribArray(VBO_INDEX_VERTEX);	
		glVertexAttribPointer(VBO_INDEX_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
        // Vertex Normals
        glEnableVertexAttribArray(VBO_INDEX_NORMAL);	
		glVertexAttribPointer(VBO_INDEX_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
        //Vertex Colors
		//glEnableVertexAttribArray(VBO_INDEX_COLOR);
		
		// Vertex Texture Coords
        glEnableVertexAttribArray(VBO_INDEX_TEX2D);	
		glVertexAttribPointer(VBO_INDEX_TEX2D, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

		// Vertex Tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));
        // Vertex Bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0);
#endif
    }
};



