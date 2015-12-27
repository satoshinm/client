#include "matrix.h"
#include "hud.h"
#include "cube.h"
#include "hud_shader.h"
#include <string>

namespace konstructs {
    using matrix::projection_2d;
    using std::vector;


    float* make_stacks(const std::unordered_map<Vector2i, ItemStack, matrix_hash<Vector2i>> &stacks,
                       const int blocks[256][6]);

    float* make_stack_amounts(const std::unordered_map<Vector2i, ItemStack, matrix_hash<Vector2i>> &stacks, int total);

    vector<float> make_square(const std::unordered_map<Vector2i, int, matrix_hash<Vector2i>> &background);


    ItemStackModel::ItemStackModel(const GLuint position_attr, const GLuint uv_attr,
                                   const std::unordered_map<Vector2i, ItemStack, matrix_hash<Vector2i>> &stacks,
                                   const int blocks[256][6]) :
        position_attr(position_attr),
        uv_attr(uv_attr) {

        auto data = make_stacks(stacks, blocks);

        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, stacks.size() * 10 * 6 * 6 * sizeof(GLfloat),
                     data, GL_STATIC_DRAW);
        verts = stacks.size() * 6 * 6;
        delete[] data;
    }

    int ItemStackModel::vertices() {
        return verts;
    }

    void ItemStackModel::bind() {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glEnableVertexAttribArray(position_attr);
        glEnableVertexAttribArray(uv_attr);
        glVertexAttribPointer(position_attr, 3, GL_FLOAT, GL_FALSE,
                              sizeof(GLfloat) * 10, 0);
        glVertexAttribPointer(uv_attr, 4, GL_FLOAT, GL_FALSE,
                              sizeof(GLfloat) * 10, (GLvoid *)(sizeof(GLfloat) * 6));
    }

    AmountModel::AmountModel(const GLuint position_attr, const GLuint uv_attr,
                             const std::unordered_map<Vector2i, ItemStack, matrix_hash<Vector2i>> &stacks):
        position_attr(position_attr),
        uv_attr(uv_attr) {
        int total_text_length = 0;
        for (const auto &pair: stacks) {
            if(pair.second.amount == 0) {
                continue;
            }
            if(pair.second.amount > 9) {
                total_text_length += 2;
            } else {
                total_text_length ++;
            }
        }

        auto data = make_stack_amounts(stacks, total_text_length);

        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, total_text_length * 7 * 6 * sizeof(GLfloat),
                     data, GL_STATIC_DRAW);
        verts = total_text_length * 6;
        delete[] data;
    }

    int AmountModel::vertices() {
        return verts;
    }

    void AmountModel::bind() {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glEnableVertexAttribArray(position_attr);
        glEnableVertexAttribArray(uv_attr);
        glVertexAttribPointer(position_attr, 3, GL_FLOAT, GL_FALSE,
                              sizeof(GLfloat) * 7, 0);
        glVertexAttribPointer(uv_attr, 4, GL_FLOAT, GL_FALSE,
                              sizeof(GLfloat) * 7, (GLvoid *)(sizeof(GLfloat) * 3));
    }

    HudModel::HudModel(const std::unordered_map<Vector2i, int, matrix_hash<Vector2i>> &background,
                       const GLuint position_attr, const GLuint uv_attr) :
        position_attr(position_attr),
        uv_attr(uv_attr) {

        auto data = make_square(background);

        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat),
                     data.data(), GL_STATIC_DRAW);
        verts = data.size() / 7;
    }

    int HudModel::vertices() {
        return verts;
    }

    void HudModel::bind() {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glEnableVertexAttribArray(position_attr);
        glEnableVertexAttribArray(uv_attr);
        glVertexAttribPointer(position_attr, 3, GL_FLOAT, GL_FALSE,
                              sizeof(GLfloat) * 7, 0);
        glVertexAttribPointer(uv_attr, 4, GL_FLOAT, GL_FALSE,
                              sizeof(GLfloat) * 7, (GLvoid*)(sizeof(GLfloat)*3));
    }

    HudShader::HudShader(const int columns, const int rows, const int texture,
                         const int block_texture, const int font_texture) :
        ShaderProgram(
            "hud",

            "#version 330\n"
            "uniform vec4 offset;\n"
            "uniform mat4 matrix;\n"
            "in vec3 position;\n"
            "in vec4 uv;\n"
            "out vec2 fragment_uv;\n"
            "void main() {\n"
            "    fragment_uv = uv.xy;\n"
            "    gl_Position = vec4(position, 1.0) * matrix + offset;\n"
            "}\n",

            "#version 330\n"
            "uniform sampler2D sampler;\n"
            "in vec2 fragment_uv;\n"
            "out vec4 fragColor;\n"
            "void main() {\n"
            "    vec3 color = vec3(texture(sampler, fragment_uv));\n"
            "    if (color == vec3(1.0, 0.0, 1.0)) {\n"
            "        discard;\n"
            "    }\n"
            "    fragColor = vec4(color, 1.0);\n"
            "}\n"),
        position(attributeId("position")),
        uv(attributeId("uv")),
        offset(uniformId("offset")),
        matrix(uniformId("matrix")),
        sampler(uniformId("sampler")),
        texture(texture),
        block_texture(block_texture),
        font_texture(font_texture),
        columns(columns),
        rows(rows) {}

    optional<Vector2i> HudShader::clicked_at(const double x, const double y,
                                             const int width, const int height) {
        // Convert to Open GL coordinates (-1 to 1) and inverted y
        double glx = (x / (double)width) * 2.0 - 1.0;
        double gly = (((double)height - y) / (double)height) * 2.0 - 1.0;

        double scale = 4.0/(double)columns;
        double xscale = (double)height / (double)width;

        // Convert to inventory positions
        double ix = (glx + 2.0*xscale*0.6) / (scale*xscale*0.6);
        double iy = (gly + 1.0) / (scale * 0.6);

        // Return position if it is within bounds
        if(ix >= 0.0 && ix < (double)columns && iy >= 0.0 && iy < (double)rows) {
            return optional<Vector2i>({(int)ix, (int)iy});
        } else {
            return nullopt;
        }
    }

    void HudShader::render(const int width, const int height,
                           const Hud &hud,
                           const int blocks[256][6]) {
        bind([&](Context c) {
                Matrix4f m;
                float scale = 4.0f/(float)columns;
                float xscale = (float)height / (float)width;
                m.col(0) << scale * 0.6f * xscale, 0.0f, 0.0f, 0.0f;
                m.col(1) << 0.0f, scale * 0.6, 0.0f, 0.0f;
                m.col(2) << 0.0f, 0.0f, 1.0f, 0.0f;
                m.col(3) << 0.0f, 0.0f, 0.0f, 1.0f;
                c.set(matrix, m);
                c.set(offset, Vector4f(-2*xscale*0.6f, -1.0f, 0.0f, 0.0f));
                c.set(sampler, texture);
                HudModel hm(hud.backgrounds(), position, uv);
                c.draw(hm);
                c.set(sampler, block_texture);
                ItemStackModel ism(position, uv, hud.stacks(), blocks);
                c.draw(ism);
                c.set(sampler, font_texture);
                AmountModel am(position, uv, hud.stacks());
                c.draw(am);
            });
    }

    vector<float> make_square(const std::unordered_map<Vector2i, int, matrix_hash<Vector2i>> &background) {
        vector<float> m;
        float ts = 0.25;
        for(auto pair: background) {
            int i = pair.first[0];
            int j = pair.first[1];
            int t = pair.second;
            if(t >= 0) {
                m.push_back(-0.0f+i); m.push_back(1.0f+j); m.push_back(0.0f);
                m.push_back(t*ts); m.push_back(1.0f);
                m.push_back(0.0f); m.push_back(0.0f);
                m.push_back(1.0f+i);  m.push_back(1.0f+j);  m.push_back(0.0f);
                m.push_back(t*ts + ts); m.push_back(1.0f);
                m.push_back(0.0f); m.push_back(0.0f);
                m.push_back(-0.0f+i); m.push_back(-0.0f+j);  m.push_back(0.0f);
                m.push_back(t*ts); m.push_back(0.0f);
                m.push_back(0.0f); m.push_back(0.0f);
                m.push_back(-0.0f+i); m.push_back(-0.0f+j);  m.push_back(0.0f);
                m.push_back(t*ts); m.push_back(0.0f);
                m.push_back(0.0f); m.push_back(0.0f);
                m.push_back(1.0f+i);  m.push_back(1.0f+j);  m.push_back(0.0f);
                m.push_back(t*ts + ts); m.push_back(1.0f);
                m.push_back(0.0f); m.push_back(0.0f);
                m.push_back(1.0f+i); m.push_back(-0.0f+j);  m.push_back(0.0f);
                m.push_back(t*ts + ts); m.push_back(0.0f);
                m.push_back(0.0f); m.push_back(0.0f);
            }
        }
        return m;
    }

    float* make_stacks(const std::unordered_map<Vector2i, ItemStack, matrix_hash<Vector2i>> &stacks,
                       const int blocks[256][6]) {
        float ao[6][4] = {0};
        float light[6][4] = {
            {0.5, 0.5, 0.5, 0.5},
            {0.5, 0.5, 0.5, 0.5},
            {0.5, 0.5, 0.5, 0.5},
            {0.5, 0.5, 0.5, 0.5},
            {0.5, 0.5, 0.5, 0.5},
            {0.5, 0.5, 0.5, 0.5}
        };

        int i = 0;
        float *d = new float[stacks.size() * 10 * 6 * 6];
        for (const auto &pair: stacks) {
            make_cube(d + i * 10 * 6 * 6, ao, light,
                      1, 1, 1, 1, 1, 1,
                      pair.first[0] + 0.5, pair.first[1] + 0.5, 0, 0.35,
                      pair.second.type, blocks);
            i++;
        }
        return d;
    }

    float* make_stack_amounts(const std::unordered_map<Vector2i, ItemStack, matrix_hash<Vector2i>> &stacks,
                              int total) {
        int i = 0;

        float *d = new float[total * 7 * 6];

        for (const auto &pair: stacks) {
            if(pair.second.amount == 0) {
                continue;
            }
            std::string text = std::to_string(pair.second.amount);
            for (int index = 0; index < text.length(); index++) {
                int offset = text.length() - index - 1;
                make_character(d + i * 7 * 6, pair.first[0] - (float)offset*0.3 + 0.75f, pair.first[1] + 0.25f, 0.15, 0.2, text[index], 0.0);
                i++;
            }
        }
        return d;
    }

};
