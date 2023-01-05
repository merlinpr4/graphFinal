/*
This project was created by Merlin Prasad (Student number 19333557)
It creates a winter wonderland scene of a forest in the early morning
It implements all 3 required basic features (crowd,textures and lighting) and the following 6 advanced features : 
1. Music using irrklang audio library
2. Skybox 
3. Handmade models
4. Fog (implementation can be seen in manyLights.fs and shad.fs)
5. Advanced lighting (Blinn-Phong and Gamma correction)
6. Hierachal animated crowd

I worked on this project by myself and used learnopengl https://learnopengl.com/Getting-started/OpenGL for help implementing my features
Code references have been listed in the source files and report
*/
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

//audio library
#include <irrklang/irrKlang.h>
using namespace irrklang;

//function declarations
void framebufferSize(GLFWwindow* window, int width, int height);
void mouse(GLFWwindow* window, double xpos, double ypos);
void scroll(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadCubemap(vector<std::string> faces);

// window settings
const unsigned int SCR_WIDTH = 990;
const unsigned int SCR_HEIGHT = 556;

//variables to control fog
bool fog = false;
bool fogKey = false;

// lighting
//directional light Position
glm::vec3 lightPos(1.2f, 3.0f, 2.0f);
float ambient = 0.05f;
float specular = 1.0f;
float diffuse = .8f;

// camera
Camera camera(glm::vec3(0.0f, 2.0f, 3.0f)); //starting position for camera
float lastXPos = SCR_WIDTH / 2.0f;
float lastYPos = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// time
float dTime = 0.0f;
float last = 0.0f;

//creating the sound engine to play music
ISoundEngine* musicEngine = createIrrKlangDevice();

int main()
{
    // initialisation
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "WinterWonderland", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create window :(" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSize);
    glfwSetCursorPosCallback(window, mouse);
    glfwSetScrollCallback(window, scroll);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD :(" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);


    // build and compile shaders
    // ---------------------------------------------------
    //shaders that work for material properties
    Shader matShader("shad.vs", "shad.fs");
    Shader skyboxShader("skybox.vs","skybox.fs");
    //multiple light source shaders
    Shader lightingShader("manyLights.vs", "manyLights.fs");
    
    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(-24.21f,  5.19f,  .90f),
        glm::vec3(2.3f, 3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(10.0f,  4.0f, -3.0f)
    };

    //Skybox setup------------------------------------------------------------------------------------------------
    //Skybox code reference https://learnopengl.com/Advanced-OpenGL/Cubemaps
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
        "sky/right.jpg",
        "sky/left.jpg",
        "sky/top.jpg",
        "sky/bottom.jpg",
        "sky/front.jpg",
        "sky/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // load models

    //floor and presents
    Model floor("floorModel/ground.obj");
    Model prez("floorModel/prez.obj");
    Model armRight("snowManMatt/rightArm.obj");
    Model armLeft("snowManMatt/leftArm.obj");
   
    //snowmanHierachy model
    Model snowManBasic("snowManMatt/snowmanBasic.obj"); //snowmanBasic is the model with no arms
    Model basicLeft("snowManMatt/snowmanBasicLeft.obj");
    Model basicRight("snowManMatt/snowmanBasicRight.obj");

   
    if (!musicEngine)
    {
        printf("Could not startup engine\n");
        return 0; // error starting up the engine
    }

   
    ISoundSource* backgroundMusic = musicEngine->addSoundSourceFromFile("music/morning.mp3");

    backgroundMusic->setDefaultVolume(0.08f);
    musicEngine->play2D(backgroundMusic,true);
    
 
    //some positional sound
    vec3df crowdPosition(0, 0, 0);
   
    //crowd moving through snow sound
    ISound* snowSound = musicEngine->play3D("music/snow.mp3", crowdPosition,true);
    if (snowSound)
    {
        snowSound->setVolume(.006f);
        snowSound->setMinDistance(0.05f);
        
        snowSound->setIsPaused(false);
    }

    //birds in the forest sound
    vec3df birdPosition(-5.0, 10, -30);
    ISound* birdSound = musicEngine->play3D("music/birds.mp3", birdPosition, true);
    if (birdSound)
    {
        birdSound->setVolume(.08f);
        birdSound->setMinDistance(0.2f);
    }
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float current = static_cast<float>(glfwGetTime());
        dTime = current - last;
        last = current;

        processInput(window);

        //listener position set to camera
        musicEngine->setListenerPosition(vec3df(camera.Position.x, camera.Position.y, camera.Position.z), vec3df(0, 0, 1));


        // render-------------------------------------------------------------------------------------------
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.use();
        lightingShader.setFloat("material.shininess", 32.0f);
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setInt("fog", fog);

        // point light 1
        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        lightingShader.setVec3("pointLights[0].ambient", ambient, ambient, ambient);
        lightingShader.setVec3("pointLights[0].diffuse", diffuse, diffuse, diffuse);
        lightingShader.setVec3("pointLights[0].specular", specular, specular, specular);
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", 0.09f);
        lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        lightingShader.setVec3("pointLights[1].ambient", ambient, ambient, ambient);
        lightingShader.setVec3("pointLights[1].diffuse", diffuse, diffuse, diffuse);
        lightingShader.setVec3("pointLights[1].specular", specular, specular, specular);
        lightingShader.setFloat("pointLights[1].constant", 1.0f);
        lightingShader.setFloat("pointLights[1].linear", 0.09f);
        lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        lightingShader.setVec3("pointLights[2].ambient", ambient, ambient, ambient);
        lightingShader.setVec3("pointLights[2].diffuse", diffuse, diffuse, diffuse);
        lightingShader.setVec3("pointLights[2].specular", specular, specular, specular);
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear", 0.09f);
        lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        lightingShader.setVec3("pointLights[3].ambient", ambient, ambient, ambient);
        lightingShader.setVec3("pointLights[3].diffuse", diffuse, diffuse, diffuse);
        lightingShader.setVec3("pointLights[3].specular", specular, specular, specular);
        lightingShader.setFloat("pointLights[3].constant", 1.0f);
        lightingShader.setFloat("pointLights[3].linear", 0.09f);
        lightingShader.setFloat("pointLights[3].quadratic", 0.032f);

        // spotLight
        /**/
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", ambient, ambient, ambient);
        lightingShader.setVec3("spotLight.diffuse", diffuse, diffuse, diffuse);
        lightingShader.setVec3("spotLight.specular", specular, specular, specular);
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.08f);
        lightingShader.setFloat("spotLight.quadratic", 0.040f);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.0f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(14.0f)));
        
        // directional light

        // light properties
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); 
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); 

        lightingShader.setVec3("dirLight.direction", lightPos);
        lightingShader.setVec3("dirLight.ambient", ambientColor);
        lightingShader.setVec3("dirLight.diffuse", diffuseColor);
        lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        matShader.use();
        matShader.setVec3("light.position", lightPos);
        matShader.setVec3("viewPos", camera.Position);
        matShader.setMat4("projection", projection);
        matShader.setMat4("view", view);
        matShader.setInt("fog", fog);

        matShader.setVec3("light.ambient", ambientColor);
        matShader.setVec3("light.diffuse", diffuseColor);
        matShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
  
        //render the floor
        glm::mat4 modelFloor = glm::mat4(1.0f);
        modelFloor = glm::translate(modelFloor, glm::vec3(0.0f, 0.0f, 0.0f));
        modelFloor = glm::scale(modelFloor, glm::vec3(0.25f, 0.25f, 0.25f));

        //presents model 5 different presents to show material properties
        glm::mat4 modelPrez = glm::mat4(1.0f);
        modelPrez = glm::translate(modelPrez, glm::vec3(0.0f, 0.f, 0.0f));
        modelPrez = glm::scale(modelPrez, glm::vec3(0.25f, 0.25f, 0.25f));

        glm::mat4 modelPrez2 = glm::mat4(1.0f);
        modelPrez2 = glm::translate(modelPrez2, glm::vec3(0.0f, 0.02f, 0.3f));  //yzx
        modelPrez2 = glm::scale(modelPrez2, glm::vec3(0.25f, 0.25f, 0.25f));

        glm::mat4 modelPrez3 = glm::mat4(1.0f);
        modelPrez3 = glm::translate(modelPrez3, glm::vec3(-0.1f, 0.0f, 0.30f));
        modelPrez3 = glm::scale(modelPrez3, glm::vec3(0.3f, 0.25f, 0.3f));

        glm::mat4 modelPrez4 = glm::mat4(1.0f);
        modelPrez4 = glm::translate(modelPrez4, glm::vec3(0.05f, 0.08f, 0.20f));
        modelPrez4 = glm::scale(modelPrez4, glm::vec3(0.25f, 0.25f, 0.3f));

        glm::mat4 modelPrez5 = glm::mat4(1.0f);
        modelPrez5 = glm::translate(modelPrez5, glm::vec3(0.1f, 0.0f, 0.0f));
        modelPrez5 = glm::scale(modelPrez5, glm::vec3(0.3f, 0.25f, 0.3f));

        // render the loaded snowman model base
        glm::mat4 modelBody = glm::mat4(1.0f);
        modelBody = glm::translate(modelBody, glm::vec3(0.0f, -0.3f, cos((float)glfwGetTime()) * 2)); // making it move around the scene
        modelBody = glm::scale(modelBody, glm::vec3(0.10f, 0.10f, 0.10f));	
        modelBody = glm::rotate(modelBody, cos((float)glfwGetTime()) / 8, glm::vec3(1.0f, 0.0f, 1.0f));  //rotating body

        //render the rightArm 
        glm::mat4 rightArm = glm::mat4(1.0f);
        rightArm = glm::translate(rightArm, glm::vec3(0.2f, 0.0f, 0.0f));
        rightArm = glm::scale(rightArm, glm::vec3(1.0f, 1.0f, 1.0f));
        rightArm = glm::rotate(rightArm, cos((float)glfwGetTime()) / 3, glm::vec3(2.0f, 0.0f, 0.0f)); //moving arm

        //render the leftArm 
        glm::mat4 leftArm = glm::mat4(1.0f);
        leftArm = glm::translate(leftArm, glm::vec3(-0.2f, 0.0f, .5f));
        leftArm = glm::scale(leftArm, glm::vec3(1.0f, 1.0f, 1.0f));
        leftArm = glm::rotate(leftArm, sin((float)glfwGetTime()) / 3, glm::vec3(2.0f, 0.0f, 0.0f)); 

        //snowman Crowd connected to modelBody -----------------------------------------------------------------------------------
        glm::mat4 modelSnowman2 = glm::mat4(1.0f);
        modelSnowman2 = glm::translate(modelSnowman2, glm::vec3(10.0f, 0.0f, 1.0f));
        glm::mat4 leftArm2 = glm::mat4(1.0f);
        leftArm2 = glm::translate(leftArm2, glm::vec3(-0.2f, 0.0f, .5f));
        glm::mat4 rightArm2 = glm::mat4(1.0f);
        rightArm2 = glm::translate(rightArm2, glm::vec3(0.2f, 0.0f, 0.0f));

        glm::mat4 modelSnowman3 = glm::mat4(1.0f);
        modelSnowman3 = glm::translate(modelSnowman3, glm::vec3(20.0f, 0.0f, 0.0f));
        glm::mat4 leftArm3 = glm::mat4(1.0f);
        leftArm3 = glm::translate(leftArm3, glm::vec3(-0.2f, 0.0f, .5f));
        glm::mat4 rightArm3 = glm::mat4(1.0f);
        rightArm3 = glm::translate(rightArm3, glm::vec3(0.2f, 0.0f, 0.0f));
       
        glm::mat4 modelSnowman4 = glm::mat4(1.0f);
        modelSnowman4 = glm::translate(modelSnowman4, glm::vec3(15.0f, -.6f, -10));
        glm::mat4 leftArm4 = glm::mat4(1.0f);
        leftArm4 = glm::translate(leftArm4, glm::vec3(-0.2f, 0.0f, .5f));
        glm::mat4 rightArm4 = glm::mat4(1.0f);
        rightArm4 = glm::translate(rightArm4, glm::vec3(0.2f, 0.0f, 0.0f));
 
        glm::mat4 modelSnowman5 = glm::mat4(1.0f);
        modelSnowman5 = glm::translate(modelSnowman5, glm::vec3(5.0f, -.7f, -10));
        glm::mat4 leftArm5 = glm::mat4(1.0f);
        leftArm5 = glm::translate(leftArm5, glm::vec3(-0.2f, 0.0f, .5f));
        glm::mat4 rightArm5 = glm::mat4(1.0f);
        rightArm5 = glm::translate(rightArm5, glm::vec3(0.2f, 0.0f, 0.0f));

        //Drawing Models ############################################################################################################################

        lightingShader.use();
        lightingShader.setMat4("model", modelFloor);
        floor.Draw(lightingShader);

        //presents to show different materials --------------------------------------------------------------------------------
        // 
        matShader.use();
        //gold
        matShader.setVec3("material.ambient", 0.24725f, 0.1995f, 0.0745f);
        matShader.setVec3("material.diffuse", 0.75164f, 0.60648f, 0.22648f);
        matShader.setVec3("material.specular", 0.628281f, 0.555802f, 0.366065f);
        matShader.setFloat("material.shininess", 0.4f);
        matShader.setMat4("model", modelPrez);
        prez.Draw(matShader);

        //ruby
        matShader.setVec3("material.ambient", 0.1745f, 0.01175f, 0.01175f);
        matShader.setVec3("material.diffuse", 0.61424f, 0.04136f, 0.04136f);
        matShader.setVec3("material.specular", 0.727811f, 0.626959f, 0.626959f);
        matShader.setFloat("material.shininess", 0.6f);
        matShader.setMat4("model", modelPrez2);
        prez.Draw(matShader);

        //emerald
        matShader.setVec3("material.ambient", 0.0215f, 0.1745f, 0.0215f);
        matShader.setVec3("material.diffuse", 0.07568, 0.61424, 0.07568);
        matShader.setVec3("material.specular", 0.633, 0.727811, 0.633);
        matShader.setFloat("material.shininess", 0.6f);
        matShader.setMat4("model", modelPrez3);
        prez.Draw(matShader);

        //jade
        matShader.setVec3("material.ambient", 0.19225, 0.19225, 0.19225);
        matShader.setVec3("material.diffuse", 0.50754, 0.50754, 0.50754);
        matShader.setVec3("material.specular", 0.508273, 0.508273, 0.508273);
        matShader.setFloat("material.shininess", 0.4f);
        matShader.setMat4("model", modelPrez4);
        prez.Draw(matShader);

        //bronze
        matShader.setVec3("material.ambient", 0.2125, 0.1275, 0.054);
        matShader.setVec3("material.diffuse", .714, 0.4284, 0.18144);
        matShader.setVec3("material.specular", 0.393548, 0.271906, 0.166721);
        matShader.setFloat("material.shininess", 0.2f);
        matShader.setMat4("model", modelPrez5);
        prez.Draw(matShader);

        //crowd of snowman  hierachy connected to modelBody  ------------------------------------------------------------------------------------
        lightingShader.use();
        lightingShader.setMat4("model", modelBody);
        snowManBasic.Draw(lightingShader);
            //right arm moves along with the body
            lightingShader.setMat4("model", modelBody* rightArm);
            armRight.Draw(lightingShader);
            //left arn moves along with the body
            lightingShader.setMat4("model", modelBody* leftArm);
            armLeft.Draw(lightingShader);
     
            lightingShader.setMat4("model", modelBody * modelSnowman2);
            snowManBasic.Draw(lightingShader);
            lightingShader.setMat4("model", modelBody * modelSnowman2* leftArm2 * rightArm);
            armLeft.Draw(lightingShader);
            lightingShader.setMat4("model", modelBody* modelSnowman2* rightArm2 *rightArm);
            armRight.Draw(lightingShader);

        lightingShader.setMat4("model", modelBody* modelSnowman3);
        snowManBasic.Draw(lightingShader);
        lightingShader.setMat4("model", modelBody* modelSnowman3* leftArm3* rightArm);
        armLeft.Draw(lightingShader);
        lightingShader.setMat4("model", modelBody* modelSnowman3* rightArm3* rightArm);
        armRight.Draw(lightingShader);

        lightingShader.setMat4("model", modelBody * modelSnowman4);
        snowManBasic.Draw(lightingShader);
        lightingShader.setMat4("model", modelBody* modelSnowman4 * leftArm * leftArm4);
        basicLeft.Draw(lightingShader);
        lightingShader.setMat4("model", modelBody* modelSnowman4 * leftArm * rightArm4);
        basicRight.Draw(lightingShader);
     
        lightingShader.setMat4("model", modelBody* modelSnowman5);
        snowManBasic.Draw(lightingShader);
        lightingShader.setMat4("model", modelBody* modelSnowman5 * leftArm* leftArm5);
        basicLeft.Draw(lightingShader);
        lightingShader.setMat4("model", modelBody* modelSnowman5 * leftArm* rightArm5);
        basicRight.Draw(lightingShader);

 
         // draw skybox ---------------------------------------------------------------------------------------------------------------------
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

        //enable gamma correction disable for a darker scene------------------------------------------------------------------------------------------------
        if (!fog)
        {
            glEnable(GL_FRAMEBUFFER_SRGB);
        }
        else {
         
            glDisable(GL_FRAMEBUFFER_SRGB);
        }
       

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //delete resources
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);

    if (musicEngine)
    {
        musicEngine->drop();
    }
    glfwTerminate();
  
    return 0;  
}

//keyboard controls function
void processInput(GLFWwindow* window)
{
    //camera and escape controls ---------------------------------------------------------------------------------------------------------------------------------------
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, dTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, dTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, dTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, dTime);

    //lighting controls ------------------------------------------------------------------------------------------------------------------------------------------------
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        ambient = ambient + .02f;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        ambient = ambient - .02f;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        diffuse = diffuse + .02f;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        diffuse = diffuse - .02f;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        specular = specular + .02f;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        specular = specular - .02f;  
       
    //fog ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !fogKey)
    {
        fog = !fog;
        fogKey = true;
        //play beep sound
        musicEngine->play2D("music/beep.mp3");
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
    {
        fogKey = false;
    }
}

//window Size changes
void framebufferSize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Mouse movement
void mouse(GLFWwindow* window, double x, double y)
{
    float xPos = static_cast<float>(x);
    float yPos = static_cast<float>(y);
    if (firstMouse)
    {
        lastXPos = xPos;
        lastYPos = yPos;
        firstMouse = false;
    }
    float xoffset = xPos - lastXPos;
    float yoffset = lastYPos - yPos; 
    lastXPos = xPos;
    lastYPos = yPos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll(GLFWwindow* window, double xoffset, double yoffset)
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