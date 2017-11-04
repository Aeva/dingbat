
#define GLFW_INCLUDE_ES31
#include <GLFW/glfw3.h>

int main()
{
    if (glfwInit())
    {
    }
    else
    {
        return 1;
    }
    glfwTerminate();
    return 0;
}
