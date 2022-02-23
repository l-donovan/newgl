#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>

#include <plog/Log.h>

#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "engine/global.h"
#include "engine/mesh.h"

using std::vector;

struct face_t {
    GLushort v0;
    GLushort v1;
    GLushort v2;
    GLushort uv0 = 0;
    GLushort uv1 = 0;
    GLushort uv2 = 0;
};

void trim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));

    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

Mesh Mesh::from_obj(string filename) {
    std::ifstream in_file;
    in_file.open(filename);

    vector<glm::vec4> rvertices;
    vector<glm::vec2> ruvs;
    vector<glm::vec3> rnormals;
    vector<face_t> rfaces;
    vector<int> vec_count;
    bool preset_normals = false;

    std::regex face("f\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)");
    std::regex face_uv("f\\s+(\\d+)/(\\d+)\\s+(\\d+)/(\\d+)\\s+(\\d+)/(\\d+)");
    std::regex face_uv_norm("f\\s+(\\d+)/(\\d*)/(\\d+)\\s+(\\d+)/(\\d*)/(\\d+)\\s+(\\d+)/(\\d*)/(\\d+)");
    std::smatch match;

    std::map<GLushort, vector<GLushort>> vertex_normals;

    Mesh mesh;

    if (!in_file) {
        PLOGE << "Error: can't open " << filename;
        return {};
    }

    for (std::string line; std::getline(in_file, line);) {
        trim(line);

        if (line.empty() || line[0] == '#')
            continue;

        if (line.substr(0, 2) == "v ") {
            std::istringstream s(line.substr(2));

            glm::vec4 v;
            s >> v.x;
            s >> v.y;
            s >> v.z;
            v.w = 1.0f;

            rvertices.push_back(v);
        } else if (std::regex_match(line, match, face)) {
            struct face_t face = {
                .v0 = (GLushort) std::stoi(match[1], nullptr),
                .v1 = (GLushort) std::stoi(match[2], nullptr),
                .v2 = (GLushort) std::stoi(match[3], nullptr)
            };

            rfaces.push_back(face);
        } else if (std::regex_match(line, match, face_uv)) {
            struct face_t face = {
                .v0 = (GLushort) std::stoi(match[1], nullptr),
                .v1 = (GLushort) std::stoi(match[3], nullptr),
                .v2 = (GLushort) std::stoi(match[5], nullptr),
                .uv0 = (GLushort) std::stoi(match[2], nullptr),
                .uv1 = (GLushort) std::stoi(match[4], nullptr),
                .uv2 = (GLushort) std::stoi(match[6], nullptr),
            };

            rfaces.push_back(face);
        } else if (std::regex_match(line, match, face_uv_norm)) {
            GLushort uv0 = 0, uv1 = 0, uv2 = 0;

            if (match[2] != "")
                uv0 = std::stoi(match[2], nullptr);

            if (match[5] != "")
                uv1 = std::stoi(match[5], nullptr);

            if (match[8] != "")
                uv2 = std::stoi(match[8], nullptr);

            struct face_t face = {
                .v0 = (GLushort) std::stoi(match[1], nullptr),
                .v1 = (GLushort) std::stoi(match[4], nullptr),
                .v2 = (GLushort) std::stoi(match[7], nullptr),
                .uv0 = uv0,
                .uv1 = uv1,
                .uv2 = uv2
            };

            GLushort norm0 = std::stoi(match[3], nullptr);
            GLushort norm1 = std::stoi(match[6], nullptr);
            GLushort norm2 = std::stoi(match[9], nullptr);

            auto it = vertex_normals.find(face.v0);
            if (it != vertex_normals.end())
                vertex_normals[face.v0].push_back(norm0);
            else
                vertex_normals[face.v0] = { norm0 };

            it = vertex_normals.find(face.v1);
            if (it != vertex_normals.end())
                vertex_normals[face.v1].push_back(norm1);
            else
                vertex_normals[face.v1] = { norm1 };

            it = vertex_normals.find(face.v2);
            if (it != vertex_normals.end())
                vertex_normals[face.v2].push_back(norm2);
            else
                vertex_normals[face.v2] = { norm2 };

            rfaces.push_back(face);
        } else if (line.substr(0, 3) == "vn ") {
            std::istringstream s(line.substr(3));

            glm::vec3 n;
            s >> n.x;
            s >> n.y;
            s >> n.z;

            rnormals.push_back(glm::normalize(n));
        } else if (line.substr(0, 3) == "vt ") {
            std::istringstream s(line.substr(3));

            glm::vec2 uv;
            s >> uv.x;
            s >> uv.y;

            ruvs.push_back(uv);
        } else {
            //PLOGW << "Unknown line: \"" << line << "\"";
        }
    }

    in_file.close();

    for (auto it = vertex_normals.begin(); it != vertex_normals.end(); ++it) {
        glm::vec3 final_normal(0.0);
        break;
        //preset_normals = true;

        for (GLushort norm_idx : it->second) {
            final_normal += rnormals[norm_idx - 1];
        }

        rnormals[it->first - 1] = glm::normalize(final_normal);
    }

    rnormals.resize(rvertices.size(), glm::vec3(0.0));
    vec_count.resize(rvertices.size(), 0);

    vector<glm::vec4> vertices;
    vector<glm::vec2> uvs;
    vector<glm::vec3> normals;
    vector<GLushort> faces;

    for (face_t face : rfaces) {
        vertices.push_back(rvertices[face.v0 - 1]);
        vertices.push_back(rvertices[face.v1 - 1]);
        vertices.push_back(rvertices[face.v2 - 1]);

        faces.push_back(vertices.size() - 3);
        faces.push_back(vertices.size() - 2);
        faces.push_back(vertices.size() - 1);

        if (face.uv0 > 0)
            uvs.push_back(ruvs[face.uv0 - 1]);
        else
            uvs.push_back(glm::vec2(0.0, 1.0));

        if (face.uv1 > 0)
            uvs.push_back(ruvs[face.uv1 - 1]);
        else
            uvs.push_back(glm::vec2(1.0, 0.0));

        if (face.uv2 > 0)
            uvs.push_back(ruvs[face.uv2 - 1]);
        else
            uvs.push_back(glm::vec2(1.0, 1.0));

        if (preset_normals) {
            normals.push_back(rnormals[face.v0 - 1]);
            normals.push_back(rnormals[face.v1 - 1]);
            normals.push_back(rnormals[face.v2 - 1]);
        } else {
            glm::vec3 normal = glm::normalize(
                glm::cross(
                    glm::vec3(rvertices[face.v1 - 1]) - glm::vec3(rvertices[face.v0 - 1]),
                    glm::vec3(rvertices[face.v2 - 1]) - glm::vec3(rvertices[face.v0 - 1])
                )
            );

            normals.push_back(normal);
            normals.push_back(normal);
            normals.push_back(normal);

            continue; // TODO This is all temporary for meshes without preset normals

            for (GLushort i : { face.v0 - 1, face.v1 - 1, face.v2 - 1 }) {
                vec_count[i]++;

                if (vec_count[i] == 1) {
                    normals.at(i) = normal;
                } else {
                    auto old_normal = normals.at(i);

                    auto new_normal = glm::vec3(
                        old_normal.x * (1.0 - 1.0 / vec_count[i]) + normal.x / vec_count[i],
                        old_normal.y * (1.0 - 1.0 / vec_count[i]) + normal.y / vec_count[i],
                        old_normal.z * (1.0 - 1.0 / vec_count[i]) + normal.z / vec_count[i]
                    );

                    normals.at(i) = glm::normalize(new_normal);
                }
            }
        }
    }

    mesh.vertices = vertices;
    mesh.uvs = uvs;
    mesh.normals = normals;
    mesh.faces = faces;

    PLOGD << "Loaded mesh " << filename << " (" << vertices.size() << "v, " << (faces.size() / 3) << "f)";

    return mesh;
}
