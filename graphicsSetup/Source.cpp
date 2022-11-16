#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "Camera.h"
#include "Model.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "WinterWonderland", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("shad.vs", "shad.fs");

    // load models
    // -----------
    //Model ourModel("backpack/backpack.obj");
   // Model ourModel("C:/Users/thoma/Desktop/GraphicsSetup/graphicsSetup/graphicsSetup/backpack/backpack.obj");
   // Model ourModel("backpack/backpack.obj");
    //Model ourModel("C:/Users/thoma/Documents/Merlin college - 4th yr/graphics/snowManMatt/snowmanScary.obj");

    Model ourModel("snowManMatt/noArmsNoHat.obj");

    Model armRight("snowManMatt/rightArm.obj");
    Model armLeft("snowManMatt/leftArm.obj");

    Model hat("snowManMatt/hat.obj");
    Model floor("floorModel/snowFloor.obj");

    Model mountain("floorModel/mountain.obj");

    Model robin("models/robin2.obj");

    //snowmanNoHierachy model
    Model snowManBasic("snowManMatt/snowmanScary.obj");


    Model cycl("models/cyclinder.obj");


   // Model ourModel("snowManMatt/snowmanScary.obj");
    //Model ourModel("C:/Users/thoma/Desktop/snowManMatt/snowmanScary.obj");

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);


        //render the floor
        glm::mat4 modelFloor = glm::mat4(1.0f);
        modelFloor = glm::translate(modelFloor, glm::vec3(0.0f, 0.0f, 0.0f));
        modelFloor = glm::scale(modelFloor, glm::vec3(1.0f, 1.0f, 1.0f));


        // render the loaded model base
        glm::mat4 modelBody = glm::mat4(1.0f);
        modelBody = glm::translate(modelBody, glm::vec3(10.0f, 0.0f, cos((float)glfwGetTime()) * 5)); // translate it down so it's at the center of the scene
        modelBody = glm::scale(modelBody, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        modelBody = glm::rotate(modelBody, cos((float)glfwGetTime()) / 6, glm::vec3(1.0f, 0.0f, 0.0f));


        //render the rightArm 
        glm::mat4 rightArm = glm::mat4(1.0f);
        rightArm = glm::translate(rightArm, glm::vec3(0.2f, 0.0f, 0.0f));
        rightArm = glm::scale(rightArm, glm::vec3(1.0f, 1.0f, 1.0f));
        rightArm = glm::rotate(rightArm, cos((float)glfwGetTime()) / 3, glm::vec3(2.0f, 0.0f, 0.0f));


        //render the leftArm 
        glm::mat4 leftArm = glm::mat4(1.0f);
        leftArm = glm::translate(leftArm, glm::vec3(-0.2f, 0.0f, .5f));
        leftArm = glm::scale(leftArm, glm::vec3(1.0f, 1.0f, 1.0f));
        leftArm = glm::rotate(leftArm, sin((float)glfwGetTime()) / 3, glm::vec3(2.0f, 0.0f, 0.0f));

        //render the hat 
        glm::mat4 modelHat = glm::mat4(1.0f);
        modelHat = glm::translate(modelHat, glm::vec3(0.0f, -0.1f, 0.0f));
        modelHat = glm::scale(modelHat, glm::vec3(1.0f, 1.0f, 1.0f));
        modelHat = glm::rotate(modelHat, cos((float)glfwGetTime()) / 15, glm::vec3(0.0f, 0.0f, 0.1f));


        //load cyclinder
        //glm::mat4 modelCyc = glm::mat4(1.0f);
        //modelCyc = glm::translate(modelCyc, glm::vec3(20.0f, -0.1f, 0.0f));
        //modelCyc = glm::scale(modelCyc, glm::vec3(1.0f, 1.0f, 1.0f));
      

        glm::mat4 modelMountain = glm::mat4(1.0f);
        modelMountain = glm::translate(modelMountain, glm::vec3(0.0f, 10.0f, 0.0f));
        modelMountain = glm::scale(modelMountain, glm::vec3(1.0f, 1.0f, 1.0f));




     
        //robin
        //glm::mat4 modelRobin = glm::mat4(1.0f);
        //modelRobin = glm::translate(modelRobin, glm::vec3(50.0f, 50.0f, 0.0f));
        //modelRobin = glm::scale(modelRobin, glm::vec3(1.0f, 1.0f, 1.0f));


        //snowman Crowd connected to modelBody
        glm::mat4 modelSnowman2 = glm::mat4(1.0f);
        modelSnowman2 = glm::translate(modelSnowman2, glm::vec3(10.0f, 0.0f, (cos((float)glfwGetTime()) * 5))); // translate it down so it's at the center of the scene

        glm::mat4 modelSnowman3 = glm::mat4(1.0f);
        modelSnowman3 = glm::translate(modelSnowman3, glm::vec3(20.0f, 0.0f, (cos((float)glfwGetTime()) * 5)));
       
        glm::mat4 modelSnowman4 = glm::mat4(1.0f);
        modelSnowman4 = glm::translate(modelSnowman4, glm::vec3(15.0f, -.6f, -10+ (cos((float)glfwGetTime()) * 5)));

        glm::mat4 modelSnowman5 = glm::mat4(1.0f);
        modelSnowman5 = glm::translate(modelSnowman5, glm::vec3(5.0f, -.7f, -10 + (cos((float)glfwGetTime()) * 5)));


        ourShader.setMat4("model", modelFloor);
        floor.Draw(ourShader);

        ourShader.setMat4("model", modelBody);
        ourModel.Draw(ourShader);

        //right arm moves along with the body
        ourShader.setMat4("model", modelBody * rightArm);
        armRight.Draw(ourShader);

        //left arn moves along with the body
        ourShader.setMat4("model", modelBody * leftArm);
        armLeft.Draw(ourShader);

        ourShader.setMat4("model", modelBody * modelHat);
        hat.Draw(ourShader);

        //crowd of snowman
        ourShader.setMat4("model", modelBody * modelSnowman2);
        snowManBasic.Draw(ourShader);

        ourShader.setMat4("model", modelBody * modelSnowman3);
        snowManBasic.Draw(ourShader);

        ourShader.setMat4("model", modelBody * modelSnowman4);
        snowManBasic.Draw(ourShader);

        ourShader.setMat4("model", modelBody* modelSnowman5);
        snowManBasic.Draw(ourShader);


        ourShader.setMat4("model", modelMountain);
        mountain.Draw(ourShader);

     //   ourShader.setMat4("model", modelRobin);
    //    robin.Draw(ourShader);

       


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}