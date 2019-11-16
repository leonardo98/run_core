#include <functional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

enum class ShaderType
{
    Flat,
    Textured
};

class Engine
{
private:
    static Engine *_instance;

    GLuint _texturedProgramID;
    GLuint _texturedMatrixID;
    GLuint _textureID;

    GLuint _flatProgramID;
    GLuint _flatMatrixID;

    GLFWwindow* _window;
public:
    virtual ~Engine();
    Engine();
    static Engine *Instance();
    bool CreateWindow(const char *name);
    int Run(const std::function<int()> &frameFunc);
    void SetShader(ShaderType shaderType);
};
