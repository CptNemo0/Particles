#include "Shader.h"
#include "pch.h"

Shader::Shader(const string& vertex_shader_path, const string& fragment_shader_path)
    : v_path(vertex_shader_path), f_path(fragment_shader_path)
{
    // Zmienne z kodem zrodlowym shaderow
    string v;
    string f;

    std::ifstream vertexReader;
    std::ifstream fragmtReader;

    try
    {
        std::stringstream vertexStream;
        std::stringstream fragmtStream;

        // Otworzenie plikow
        vertexReader.open(vertex_shader_path);
        fragmtReader.open(fragment_shader_path);

        // Odczyt
        vertexStream << vertexReader.rdbuf();
        fragmtStream << fragmtReader.rdbuf();

        vertexReader.close();
        fragmtReader.close();

        v = vertexStream.str();
        f = fragmtStream.str();
    }
    catch (std::fstream::failure e)
    {
        printf("You have f'ed up!");
    }

    const char* vertexSource = v.c_str();
    const char* fragmtSource = f.c_str();

    unsigned int vertex;
    unsigned int fragmt;

    int success;
    char* infoLog = new char[512];

    // Kompilacja shaderow
    // Vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        printf("Vertex shader compilation failed\n");
        printf(infoLog);
        exit(1);
    }
    else
    {
        printf("Vertex shader compiled\n");
    }

    // Fragment shader
    fragmt = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmt, 1, &fragmtSource, NULL);
    glCompileShader(fragmt);
    // check for shader compile errors
    glGetShaderiv(fragmt, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmt, 512, NULL, infoLog);
        printf("Fragment shader compilation failed\n");
        printf(infoLog);
        exit(1);
    }
    else
    {
        printf("Fragment shader compiled\n");
    }

    // Stworzenie programu wykonujacego shadery
    id_ = glCreateProgram();     // Tworzenie programy shaderow
    glAttachShader(id_, vertex); // Przyczepienie vertex shaderu do dzielonego programu
    glAttachShader(id_, fragmt); // Przyczepienie fragment shaderu do dzielonego programu
    glLinkProgram(id_);          // Utworzenie ostatecznego obiektu programu polaczonego z shaderami

    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(id_, 512, NULL, infoLog);
        printf("Shader linking failed !!! \n");
        printf(infoLog);
        exit(1);
    }
    else
    {
        printf("Shader Linked\n");
    }

    glDeleteShader(vertex);
    glDeleteShader(fragmt);
}

Shader::Shader(const string &vertex_shader_path, const string &geometry_shader_path, const string &fragment_shader_path)
    : v_path(vertex_shader_path), g_path(geometry_shader_path), f_path(fragment_shader_path)
{
        // Zmienne z kodem zrodlowym shaderow
    string v;
    string g;
    string f;

    std::ifstream vertexReader;
    std::ifstream geomReader;
    std::ifstream fragmtReader;

    try
    {
        std::stringstream vertexStream;
        std::stringstream fragmtStream;
        std::stringstream geomStream;

        // Otworzenie plikow
        vertexReader.open(vertex_shader_path);
        fragmtReader.open(fragment_shader_path);
        geomReader.open(geometry_shader_path);

        // Odczyt
        vertexStream << vertexReader.rdbuf();
        fragmtStream << fragmtReader.rdbuf();
        geomStream << geomReader.rdbuf();

        vertexReader.close();
        fragmtReader.close();
        geomReader.close();

        v = vertexStream.str();
        f = fragmtStream.str();
        g = geomStream.str();
    }
    catch (std::fstream::failure e)
    {
        printf("You have f'ed up!");
    }

    const char* vertexSource = v.c_str();
    const char* fragmtSource = f.c_str();
    const char* geomSource = g.c_str();

    unsigned int vertex;
    unsigned int fragmt;
    unsigned int geom;

    int success;
    char* infoLog = new char[512];

    // Kompilacja shaderow
    // Vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        printf("Vertex shader compilation failed\n");
        printf(infoLog);
        exit(1);
    }
    else
    {
        printf("Vertex shader compiled\n");
    }

    // Geometry shader
    geom = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geom, 1, &geomSource, NULL);
    glCompileShader(geom);

    glGetShaderiv(geom, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(geom, 512, NULL, infoLog);
        printf("Geometry shader compilation failed\n");
        printf(infoLog);
        exit(1);
    }
    else
    {
        printf("Vertex shader compiled\n");
    }

    // Fragment shader
    fragmt = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmt, 1, &fragmtSource, NULL);
    glCompileShader(fragmt);
    // check for shader compile errors
    glGetShaderiv(fragmt, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmt, 512, NULL, infoLog);
        printf("Fragment shader compilation failed\n");
        printf(infoLog);
        exit(1);
    }
    else
    {
        printf("Fragment shader compiled\n");
    }

    // Stworzenie programu wykonujacego shadery
    id_ = glCreateProgram();     // Tworzenie programy shaderow
    glAttachShader(id_, vertex); // Przyczepienie vertex shaderu do dzielonego programu
    glAttachShader(id_, geom);   // Przyczepienie geometry shaderu do dzielonego programu
    glAttachShader(id_, fragmt); // Przyczepienie fragment shaderu do dzielonego programu
    glLinkProgram(id_);          // Utworzenie ostatecznego obiektu programu polaczonego z shaderami

    glDeleteShader(vertex);
    glDeleteShader(fragmt);
    glDeleteShader(geom);
}

Shader::Shader(const string& vertex_shader_path, const string& tcs_shader_path, const string& tes_shader_path, const string& fragment_shader_path)
    : v_path(vertex_shader_path), tcs_path(tcs_shader_path), tes_path(tes_shader_path), f_path(fragment_shader_path)
{
    // Zmienne z kodem zrodlowym shaderow
    string v;
    string f;
    string tcs;
    string tes;

    std::ifstream vertexReader;
    std::ifstream fragmtReader;
    std::ifstream tcsReader;
    std::ifstream tesReader;

    try
    {
        std::stringstream vertexStream;
        std::stringstream fragmtStream;
        std::stringstream tcsStream;
        std::stringstream tesStream;

        // Otworzenie plikow
        vertexReader.open(vertex_shader_path);
        fragmtReader.open(fragment_shader_path);
        tcsReader.open(tcs_shader_path);
        tesReader.open(tes_shader_path);

        // Odczyt
        vertexStream << vertexReader.rdbuf();
        fragmtStream << fragmtReader.rdbuf();
        tcsStream << tcsReader.rdbuf();
        tesStream << tesReader.rdbuf();

        vertexReader.close();
        fragmtReader.close();
        tcsReader.close();
        tesReader.close();

        v = vertexStream.str();
        f = fragmtStream.str();
        tcs = tcsStream.str();
        tes = tesStream.str();
    }
    catch (std::fstream::failure e)
    {
        printf("You have f'ed up!");
    }

    const char* vertexSource = v.c_str();
    const char* fragmtSource = f.c_str();
    const char* tcsSource = tcs.c_str();
    const char* tesSource = tes.c_str();

    unsigned int vertex;
    unsigned int fragmt;
    unsigned int tcs_id;
    unsigned int tes_id;

    int success;
    char* infoLog = new char[512];

    // Kompilacja shaderow
    // Vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        printf("Vertex shader compilation failed\n");
        printf(infoLog);
        exit(1);
    }
    else
    {
        printf("Vertex shader compiled\n");
    }

    // Fragment shader
    fragmt = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmt, 1, &fragmtSource, NULL);
    glCompileShader(fragmt);
    // check for shader compile errors
    glGetShaderiv(fragmt, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmt, 512, NULL, infoLog);
        printf("Fragment shader compilation failed\n");
        printf(infoLog);
        exit(1);
    }
    else
    {
        printf("Fragment shader compiled\n");
    }

    tcs_id = glCreateShader(GL_TESS_CONTROL_SHADER);
    glShaderSource(tcs_id, 1, &tcsSource, NULL);
    glCompileShader(tcs_id);
    // check for shader compile errors
    glGetShaderiv(tcs_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(tcs_id, 512, NULL, infoLog);
        printf("Tesselation controll shader compilation failed\n");
        printf(infoLog);
        exit(1);
    }
    else
    {
        printf("Tesselation controll shader compiled\n");
    }

    tes_id = glCreateShader(GL_TESS_EVALUATION_SHADER);
    glShaderSource(tes_id, 1, &tesSource, NULL);
    glCompileShader(tes_id);
    // check for shader compile errors
    glGetShaderiv(tes_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(tes_id, 512, NULL, infoLog);
        printf("Tesselation evaluation shader compilation failed\n");
        printf(infoLog);
        exit(1);
    }
    else
    {
        printf("Tesselation evaluation shader compiled\n");
    }

    // Stworzenie programu wykonujacego shadery
    id_ = glCreateProgram();     // Tworzenie programy shaderow
    glAttachShader(id_, vertex); // Przyczepienie vertex shaderu do dzielonego programu
    glAttachShader(id_, fragmt); // Przyczepienie fragment shaderu do dzielonego programu
    glAttachShader(id_, tcs_id);   // Przyczepienie tc shaderu do dzielonego programu
    glAttachShader(id_, tes_id);
    glLinkProgram(id_);          // Utworzenie ostatecznego obiektu programu polaczonego z shaderami

    glDeleteShader(vertex);
    glDeleteShader(fragmt);
    glDeleteShader(tcs_id);
    glDeleteShader(tes_id);
}

Shader::~Shader()
{
    End();
}

void Shader::Use()
{
    glUseProgram(id_);
}

void Shader::End()
{
    glDeleteProgram(id_);
}

unsigned int Shader::get_id() const
{
    return id_;
}

void Shader::SetBool(const string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(id_, name.c_str()), (int)value);
}
void Shader::SetInt(const string& name, int value) const
{
    glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
}
void Shader::SetFloat(const string& name, float value) const
{
    glUniform1f(glGetUniformLocation(id_, name.c_str()), value);
}
void Shader::SetMatrix4(const string& name, glm::mat4 value) const
{
    glUniformMatrix4fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetVec3(const string& name, glm::vec3 value) const
{
    glUniform3f(glGetUniformLocation(id_, name.c_str()), value.x, value.y, value.z);
}

void Shader::SetVec2(const string& name, glm::vec2 value) const
{
    glUniform2f(glGetUniformLocation(id_, name.c_str()), value.x, value.y);
}

void Shader::SetVec4(const string &name, glm::vec4 value) const
{
    glUniform4f(glGetUniformLocation(id_, name.c_str()), value.x, value.y, value.z, value.w);
}
