#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "./headers/stb_image_imp.h"
#include "./headers/shader.h"
#include "./headers/camera.h"

#include <iostream>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

// settings
const GLuint SCREEN_WIDTH = 1280;
const GLuint SCREEN_HEIGHT = 960;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
bool flashlight = true;

int main() {

    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw: window creation
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // confiure global OpenGL state
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    Shader cubeShader("./shaders/cube/cube-vs.glsl", "./shaders/cube/cube-fs.glsl");
    Shader lampShader("./shaders/lamp/lightCube-vs.glsl", "./shaders/lamp/lightCube-fs.glsl");
    Shader pyramidShader("./shaders/pyramid/pyramid-vs.glsl", "./shaders/pyramid/pyramid-fs.glsl");

    // set up vertex data
    GLfloat verticesCube[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    GLfloat verticesPyramid[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f,  0.0f,    //
         0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,  -1.0f,  0.0f,    // back
         0.0f,  0.5f,  0.0f,   0.0f, 0.0f, -1.0f,  -0.5f, -1.0f,    //
         
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  1.0f,   0.0f,  0.0f,    //
         0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  1.0f,  -1.0f,  0.0f,    // front
         0.0f,  0.5f,  0.0f,   0.0f, 0.0f,  1.0f,  -0.5f, -1.0f,    //

        -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f,  0.0f,   0.0f,  0.0f,    //
        -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  -1.0f,  0.0f,    // left
         0.0f,  0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  -0.5f, -1.0f,    //

         0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  0.0f,   0.0f,  0.0f,    //
         0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  0.0f,  -1.0f,  0.0f,    // right
         0.0f,  0.5f,  0.0f,   1.0f, 0.0f,  0.0f,  -0.5f, -1.0f,    //

        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  1.0f,   0.0f,  1.0f,    //
         0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  1.0f,   1.0f,  1.0f,    // bottom
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  1.0f,   0.0f,  0.0f,    //
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  1.0f,   1.0f,  0.0f,    //
         0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  1.0f,   0.0f,  0.0f,    // bottom
         0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  1.0f,   0.0f,  1.0f,    //
    };

    // positions of the first set of cubes
    glm::vec3 cubePositions1[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -10.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -8.3f),
        glm::vec3(2.4f, -0.4f, -3.5f)
    };

    // positions of the second set of cubes
    glm::vec3 cubePositions2[] = {
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -5.0f),
        glm::vec3(1.5f,  2.0f, -5.0f),
        glm::vec3(1.5f,  0.2f, -0.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    glm::vec3 pyramidPositions[] = {
        glm::vec3(-2.0f, 0.0f, -6.0f),
        glm::vec3(4.0f, 2.0f, -4.0f),
        glm::vec3(3.0f, -5.0f, -7.0f)
    };

    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -8.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };

    GLuint VBO;

    // configuring the cube's VAO
    // ---------------------------------------------------------------------------------------------
    GLuint cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCube), verticesCube, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // ---------------------------------------------------------------------------------------------

    // configuring the light cube's VAO
    // ---------------------------------------------------------------------------------------------
    GLuint lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // ---------------------------------------------------------------------------------------------

    // configuring the pyramid's VAO and VBO
    // ---------------------------------------------------------------------------------------------
    GLuint pyramidVAO, pyramidVBO;
    glGenVertexArrays(1, &pyramidVAO);
    glGenBuffers(1, &pyramidVBO);

    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPyramid), verticesPyramid, GL_STATIC_DRAW);

    glBindVertexArray(pyramidVAO);
    
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // ---------------------------------------------------------------------------------------------

    // load the textures
    unsigned int diffuseMap = loadTexture("./assets/textures/container.png");
    unsigned int specularMap = loadTexture("./assets/textures/container_specular.png");
    unsigned int diffuseMap2 = loadTexture("./assets/textures/wooden_box.png");
    unsigned int pyramidMap = loadTexture("./assets/textures/pyramid.png");

    cubeShader.use();
    cubeShader.setInt("material.diffuse", 0);
    cubeShader.setInt("material.specular", 1);

    lampShader.use();

    pyramidShader.use();
    pyramidShader.setInt("material.diffuse", 0);
    pyramidShader.setInt("material.specular", 1);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // cube lighting
        // -----------------------------------------------------------------------------------
        // activate shader
        cubeShader.use();
        cubeShader.setVec3("viewPos", camera.Position);
        cubeShader.setFloat("material.shininess", 32.0f);

        // directional light
        cubeShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        cubeShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        cubeShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        cubeShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        // point light 1
        cubeShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        cubeShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        cubeShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        cubeShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        cubeShader.setFloat("pointLights[0].constant", 1.0f);
        cubeShader.setFloat("pointLights[0].linear", 0.09f);
        cubeShader.setFloat("pointLights[0].quadratic", 0.032f);

        // point light 2
        cubeShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        cubeShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        cubeShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        cubeShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        cubeShader.setFloat("pointLights[1].constant", 1.0f);
        cubeShader.setFloat("pointLights[1].linear", 0.09f);
        cubeShader.setFloat("pointLights[1].quadratic", 0.032f);

        // point light 3
        cubeShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        cubeShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        cubeShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        cubeShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        cubeShader.setFloat("pointLights[2].constant", 1.0f);
        cubeShader.setFloat("pointLights[2].linear", 0.09f);
        cubeShader.setFloat("pointLights[2].quadratic", 0.032f);

        // point light 4
        cubeShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        cubeShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        cubeShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        cubeShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        cubeShader.setFloat("pointLights[3].constant", 1.0f);
        cubeShader.setFloat("pointLights[3].linear", 0.09f);
        cubeShader.setFloat("pointLights[3].quadratic", 0.032f);

        // spotLight
        if (flashlight) {
            cubeShader.setVec3("spotLight.position", camera.Position);
            cubeShader.setVec3("spotLight.direction", camera.Front);
            cubeShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            cubeShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
            cubeShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
            cubeShader.setFloat("spotLight.constant", 1.0f);
            cubeShader.setFloat("spotLight.linear", 0.09f);
            cubeShader.setFloat("spotLight.quadratic", 0.032f);
            cubeShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
            cubeShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        }
        else {
            cubeShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            cubeShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
            cubeShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
        }
        // -----------------------------------------------------------------------------------

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();
        cubeShader.setMat4("projection", projection);
        cubeShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        cubeShader.setMat4("model", model);

        // bind the diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        // bind the specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // setup for the moving cubes
        glBindVertexArray(cubeVAO);
        float moveAmount = static_cast<float>(sin(glfwGetTime()) * 1.0f);

        // render the x-moving cube
        // ------------------------------------------------------------------------------
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(moveAmount - 5.0f, 0.0f, -4.0f));
        model = glm::rotate(model, (float)(glfwGetTime() * sin(10.0f)), glm::vec3(1.0f));
        cubeShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // ------------------------------------------------------------------------------

        // render the y-moving cube
        // ------------------------------------------------------------------------------
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.5f, moveAmount + 3.0f, -5.0f));
        model = glm::rotate(model, (float)(glfwGetTime() * sin(5.0f)), glm::vec3(1.0f));
        cubeShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // ------------------------------------------------------------------------------

        // render the z-moving cube
        // ------------------------------------------------------------------------------
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(5.5f, 0.0f, moveAmount - 2.5f));
        model = glm::rotate(model, (float)(glfwGetTime() * sin(2.5f)), glm::vec3(1.0f));
        cubeShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // ------------------------------------------------------------------------------

        // render the first set of cubes
        for (unsigned int i = 0; i < 5; i++) {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model, (float)(glfwGetTime() * sin(i + 10.0f)), cubePositions1[i]);
            model = glm::translate(model, cubePositions1[i]);
            model = glm::scale(model, glm::vec3(i * 0.5f));
            cubeShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // bind the diffuse map to the second set of cubes
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap2);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);

        // render the second set of cubes
        for (unsigned int i = 0; i < 5; i++) {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            
            model = glm::rotate(model, (float)(glfwGetTime() * sin(i + 2.0f)), cubePositions2[i]);
            model = glm::translate(model, cubePositions2[i]);
            model = glm::scale(model, glm::vec3(i * 0.3f));
            
            cubeShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        // draw the lamp object
        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
       
        glBindVertexArray(lightCubeVAO);
       
        for (unsigned int i = 0; i < 4; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
           
            lampShader.setMat4("model", model);
            
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // bind the diffuse map to the pyramid
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pyramidMap);

        // pyramid lighting
        // -----------------------------------------------------------------------------------
        // activate shader
        pyramidShader.use();
        pyramidShader.setMat4("projection", projection);
        pyramidShader.setMat4("view", view);
        pyramidShader.setMat4("model", model);

        // directional light
        pyramidShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        pyramidShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        pyramidShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        pyramidShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        // point light 1
        pyramidShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        pyramidShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        pyramidShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        pyramidShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        pyramidShader.setFloat("pointLights[0].constant", 1.0f);
        pyramidShader.setFloat("pointLights[0].linear", 0.09f);
        pyramidShader.setFloat("pointLights[0].quadratic", 0.032f);

        // point light 2
        pyramidShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        pyramidShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        pyramidShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        pyramidShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        pyramidShader.setFloat("pointLights[1].constant", 1.0f);
        pyramidShader.setFloat("pointLights[1].linear", 0.09f);
        pyramidShader.setFloat("pointLights[1].quadratic", 0.032f);

        // point light 3
        pyramidShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        pyramidShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        pyramidShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        pyramidShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        pyramidShader.setFloat("pointLights[2].constant", 1.0f);
        pyramidShader.setFloat("pointLights[2].linear", 0.09f);
        pyramidShader.setFloat("pointLights[2].quadratic", 0.032f);

        // point light 4
        pyramidShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        pyramidShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        pyramidShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        pyramidShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        pyramidShader.setFloat("pointLights[3].constant", 1.0f);
        pyramidShader.setFloat("pointLights[3].linear", 0.09f);
        pyramidShader.setFloat("pointLights[3].quadratic", 0.032f);

        // spotLight
        if (flashlight) {
            pyramidShader.setVec3("spotLight.position", camera.Position);
            pyramidShader.setVec3("spotLight.direction", camera.Front);
            pyramidShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            pyramidShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
            pyramidShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
            pyramidShader.setFloat("spotLight.constant", 1.0f);
            pyramidShader.setFloat("spotLight.linear", 0.09f);
            pyramidShader.setFloat("spotLight.quadratic", 0.032f);
            pyramidShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
            pyramidShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        }
        else {
            pyramidShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            pyramidShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
            pyramidShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
        }
        // -----------------------------------------------------------------------------------

        // pyramid material properties
        pyramidShader.setFloat("material.shininess", 32.0f);

        glBindVertexArray(pyramidVAO);

        // pyramid
        // ---------------------------------------------------------------------------------
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
        model = glm::rotate(model, (float)(glfwGetTime() * sin(10.0f) * 2), glm::vec3(0.0f, 1.0f, 0.0f));
        pyramidShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 18);
        // ---------------------------------------------------------------------------------

        for (unsigned int i = 0; i < 3; i++) {
            model = glm::mat4(1.0f);

            model = glm::rotate(model, (float)(glfwGetTime() * sin(2.0f + i)), pyramidPositions[i]);
            model = glm::translate(model, pyramidPositions[i]);

            pyramidShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 18);
        }

        // glfw: swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // de-allocate all resources once they have outlived their purpose
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteVertexArrays(1, &pyramidVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated glfw resources
    glfwTerminate();
    return 0;
}

// handles the camera speed and camera controls
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.processKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.processKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.processKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.processKeyboard(RIGHT, deltaTime);
    }
}

// glfw: whenever the window is resized, this function is called
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// handles the flashlight controls
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        if (flashlight) {
            flashlight = false;
        }
        else {
            flashlight = true;
        }
    }
}

// glfw: whenever the mouse moves, this function is called
void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn) {
    float xPos = static_cast<float>(xPosIn);
    float yPos = static_cast<float>(yPosIn);

    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;
    
    camera.processMouseMovement(xOffset, yOffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this function is called
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
    camera.processMouseScroll(static_cast<float>(yOffset));
}

// utility function for loading a 2D texture from a file
unsigned int loadTexture(char const* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// code modified from https://learnopengl.com/