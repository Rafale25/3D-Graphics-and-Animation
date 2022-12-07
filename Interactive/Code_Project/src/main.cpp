#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Context.hpp"

#include "TestView.hpp"
#include "AppView.hpp"

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
const char *TITLE = "F21GA";
const int VSYNC = 0;

int main()
{
    Context ctx(SCR_WIDTH, SCR_HEIGHT, TITLE);

    // View* app_view = new AppView(ctx); // can't do View* or the destructor will not get called
    View* app_view = new AppView(ctx); // can't do View* or the destructor will not get called

    ctx.setVsync(VSYNC);
    ctx.show_view(app_view);
    ctx.run();

    delete app_view;

    return 0;
}