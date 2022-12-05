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
unsigned int loadCubemap(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 2.0f, 3.0f)); //starting position for camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
//glm::vec3 lightPos(11.0f, 2.0f, 2.0f);
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
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
    Shader hatShader("shad.vs", "shad.fs");


    Shader skyboxShader("skybox.vs","skybox.fs");

   // Shader lightingShader("light.vs", "light.fs");
    Shader lightingShader("manyLights.vs", "manyLights.fs");
    Shader lightCubeShader("lightBox.vs","lightBox.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    
    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  4.2f,  2.0f),
        glm::vec3(2.3f, 3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(10.0f,  4.0f, -3.0f)
    };

    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    vector<std::string> faces
    {
        "nightSky/right.jpg",
        "nightSky/left.jpg",
        "nightSky/top.jpg",
        "nightSky/bottom.jpg",
        "nightSky/front.jpg",
        "nightSky/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

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

   // Model mountain("floorModel/mountain.obj");

   // Model robin("models/robin2.obj");

    //snowmanNoHierachy model
    Model snowManBasic("snowManMatt/snowmanScary.obj");


    //tree model if crash reduce poly count
    //Model tree("floorModel/tree.obj");


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

       
        lightingShader.use();
        lightingShader.setFloat("material.shininess", 32.0f);
        lightingShader.setVec3("viewPos", camera.Position);


         // light properties
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence

        glm::vec3 lightPosD(11.0f, 2.0f, 2.0f);
        //move light around
        lightPosD.x = 8.0f + sin(glfwGetTime()) * 2.0f;

        // point light 1
        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", 0.09f);
        lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[1].constant", 1.0f);
        lightingShader.setFloat("pointLights[1].linear", 0.09f);
        lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear", 0.09f);
        lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[3].constant", 1.0f);
        lightingShader.setFloat("pointLights[3].linear", 0.09f);
        lightingShader.setFloat("pointLights[3].quadratic", 0.032f);

        // spotLight
        /**/
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", 0.5f, 0.5f, 0.5f);
        lightingShader.setVec3("spotLight.diffuse", 1.0f, .9f, .9f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.08f);
        lightingShader.setFloat("spotLight.quadratic", 0.040f);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.0f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(14.0f)));
        
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);


        // render the cube
        glBindVertexArray(cubeVAO);
                
        // lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
       

        // we now draw as many light bulbs as we have point lights.
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            lightCubeShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

     //uses old material shader that only has directional lights
      
        ourShader.use();
        ourShader.setVec3("light.position", lightPos);
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        
        ourShader.setVec3("light.ambient", ambientColor);
        ourShader.setVec3("light.diffuse", diffuseColor);
        ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("material.ambient", 1.0f, 1.0f, 1.00f);
        ourShader.setVec3("material.diffuse", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("material.specular", 1.0f, 1.0f, 1.0f); // figure out colour later for snow
        ourShader.setFloat("material.shininess", 12.0f);
      
        hatShader.use();
        hatShader.setVec3("light.position", lightPos);
        hatShader.setVec3("viewPos", camera.Position);
        hatShader.setMat4("projection", projection);
        hatShader.setMat4("view", view);

        hatShader.setVec3("light.ambient", ambientColor);
        hatShader.setVec3("light.diffuse", diffuseColor);
        hatShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        // material properties
        hatShader.setVec3("material.ambient", 0.1745f, 0.01175f, 0.01175f);
        hatShader.setVec3("material.diffuse", 0.61424f, 0.04136f, 0.04136f);
        hatShader.setVec3("material.specular", 0.727811f, 0.626959f, 0.626959f);
        hatShader.setFloat("material.shininess", 0.6f);
       

        //render the floor
        glm::mat4 modelFloor = glm::mat4(1.0f);
        modelFloor = glm::translate(modelFloor, glm::vec3(0.0f, 0.0f, 0.0f));
        modelFloor = glm::scale(modelFloor, glm::vec3(0.25f, 0.25f, 0.25f));

        // render the loaded model base
        glm::mat4 modelBody = glm::mat4(1.0f);
        modelBody = glm::translate(modelBody, glm::vec3(10.0f, 0.0f, cos((float)glfwGetTime()) * 2)); // translate it down so it's at the center of the scene
        modelBody = glm::scale(modelBody, glm::vec3(0.10f, 0.10f, 0.10f));	// it's a bit too big for our scene, so scale it down
        //modelBody = glm::rotate(modelBody, cos((float)glfwGetTime()) / 6, glm::vec3(1.0f, 0.0f, 0.0f));


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

        glm::mat4 modelMountain = glm::mat4(1.0f);
        modelMountain = glm::translate(modelMountain, glm::vec3(0.0f, -5.0f, 0.0f));
        modelMountain = glm::scale(modelMountain, glm::vec3(0.25f, 0.25f, 0.25f));


        glm::mat4 modelTree = glm::mat4(1.0f);
        modelTree = glm::translate(modelTree, glm::vec3(0.0f, 0.0f, 0.0f));
        modelTree = glm::scale(modelTree, glm::vec3(0.25f, 0.25f, 0.25f));

       
        //snowman Crowd connected to modelBody

        glm::mat4 modelSnowman2 = glm::mat4(1.0f);
        modelSnowman2 = glm::translate(modelSnowman2, glm::vec3(10.0f, 0.0f, (cos((float)glfwGetTime()) * 5))); // translate it down so it's at the center of the scene

        glm::mat4 modelSnowman3 = glm::mat4(1.0f);
        modelSnowman3 = glm::translate(modelSnowman3, glm::vec3(20.0f, 0.0f, (cos((float)glfwGetTime()) * 5)));
       
        glm::mat4 modelSnowman4 = glm::mat4(1.0f);
        modelSnowman4 = glm::translate(modelSnowman4, glm::vec3(15.0f, -.6f, -10+ (cos((float)glfwGetTime()) * 5)));

        glm::mat4 modelSnowman5 = glm::mat4(1.0f);
        modelSnowman5 = glm::translate(modelSnowman5, glm::vec3(5.0f, -.7f, -10 + (cos((float)glfwGetTime()) * 5)));

      //  ourShader.use();
        lightingShader.use();
        lightingShader.setMat4("model", modelFloor);
        floor.Draw(ourShader);

       
        lightingShader.setMat4("model", modelBody);
        ourModel.Draw(lightingShader);

        //right arm moves along with the body
        //ourShader.use();
        lightingShader.setMat4("model", modelBody * rightArm);
        armRight.Draw(lightingShader);

        //left arn moves along with the body
        lightingShader.setMat4("model", modelBody * leftArm);
        armLeft.Draw(lightingShader);

        hatShader.use();
        hatShader.setMat4("model", modelBody* modelHat);
        hat.Draw(hatShader);
   
        hatShader.setMat4("model", modelBody* modelSnowman3);
        snowManBasic.Draw(hatShader);

        //crowd of snowman 
        ourShader.use(); //testing other shader
        ourShader.setMat4("model", modelBody * modelSnowman2);
        snowManBasic.Draw(ourShader);

        lightingShader.use();
        ourShader.setMat4("model", modelBody * modelSnowman4);
     //   snowManBasic.Draw(ourShader);


        ourShader.setMat4("model", modelMountain);
      //  mountain.Draw(ourShader);


        ourShader.setMat4("model", modelTree);
      //  tree.Draw(ourShader);

      
        //even though it says our shader the last shader colour combo is used???
        ourShader.setMat4("model", modelBody* modelSnowman5);
        snowManBasic.Draw(ourShader);

     //   ourShader.setMat4("model", modelRobin);
    //    robin.Draw(ourShader);


         // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);



    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);

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

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}