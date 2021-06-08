#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#include <rg/Shader.h>
#include <learnopengl/shader.h>
//#include <rg/mesh.h>
//#include <rg/model.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/model.h>
#include <learnopengl/mesh.h>
#include <learnopengl/filesystem.h>

#include <iostream>
#include <stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int load_texture(char const * path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

glm::vec3 cameraPos = glm::vec3(0,0,7);
glm::vec3 cameraFront = glm::vec3(0,0,-1);
glm::vec3 cameraUp = glm::vec3(0,1,0);

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct Pointlight{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    // ------------------------------------
    Shader shader("resources/shaders/1.vs", "resources/shaders/1.fs");
    Shader s1("resources/shaders/2.vs", "resources/shaders/2.fs");
    Shader izvor("resources/shaders/izvor.vs", "resources/shaders/izvor.fs");
    Shader mshader("resources/shaders/3.vs", "resources/shaders/3.fs");

    float vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,0.0,0.0,-1.0,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,0.0,0.0,-1.0,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,0.0,0.0,-1.0,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,0.0,0.0,-1.0,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,0.0,0.0,-1.0,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,0.0,0.0,-1.0,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,0.0f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,0.0f,  0.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,0.0f,  0.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,0.0f,  0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,0.0f,  0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,1.0f,  0.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,1.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,0.0f, -1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,0.0f, -1.0f,  0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,0.0f,  1.0f,  0.0f
    };

    float vertices2[] = {
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,

            -0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,

            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,

            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,

            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,


            -0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,

    };

    glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 3.5f,  0.2f, -1.5f),
            glm::vec3(5.3f,  1.0f, -1.5f)
    };

    unsigned int indices[] = {
            0,1,2,
            0,2,3,

            4,5,6,
            4,6,7,

            8,9,10,
            8,10,11,

            12,13,14,
            12,14,15,

            16,17,18,
            16,18,19,

            20,21,22,
            20,22,23
    };


    unsigned int VBO,VAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float )));
    glEnableVertexAttribArray(2);


    unsigned int izvorVAO,EBO,VBO2;
    glGenVertexArrays(1,&izvorVAO);
    glBindVertexArray(izvorVAO);

    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    glGenBuffers(1,&EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    unsigned int VAO2;
    glGenVertexArrays(1,&VAO2);
    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float )));
    glEnableVertexAttribArray(2);

    unsigned int VAO3;
    glGenVertexArrays(1,&VAO3);
    glBindVertexArray(VAO3);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    unsigned int t0 = load_texture("resources/textures/container2.png");
    unsigned int t1 = load_texture("resources/textures/container2_specular_colored.png");
    shader.use();
    shader.setInt("material.diffuse",0);
    shader.setInt("material.specular",1);

    //Model ourModel(FileSystem::getPath("resources/objects/pumpkin.obj"));
    Model ourModel(FileSystem::getPath("resources/objects/box/box.obj"));

//    shader.use();
//    shader.setUniform1i("t0",0);


    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    while (!glfwWindowShouldClose(window)){
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 proj = glm::mat4(1.0f);

        proj = glm::perspective(glm::radians(fov),(float)SCR_WIDTH/SCR_HEIGHT,0.1f,100.0f);

        shader.use();

        shader.setMat4("model",model);
        shader.setMat4("view",view);
        shader.setMat4("proj",proj);

        shader.setVec3("light.lightPos",cubePositions[9]);
        shader.setVec3("light.direction",glm::vec3(-0.2,-1.1,-0.3));
        shader.setVec3("viewPos",cameraPos);
        shader.setFloat("material.shininess",64.0f);

        shader.setVec3("light.ambient",glm::vec3(0.3));
        shader.setVec3("light.diffuse",glm::vec3(0.5));
        shader.setVec3("light.specular",glm::vec3(1.0));
        shader.setFloat("light.constant",1.0f);
        shader.setFloat("light.linear",0.09f);
        shader.setFloat("light.qudratic",0.032f);

        view = glm::lookAt(cameraPos,cameraFront + cameraPos,cameraUp);
        shader.setMat4("view",view);


        mshader.use();

        mshader.setVec3("plight.position",cubePositions[9]);
        mshader.setVec3("plight.ambient",glm::vec3(0.2));
        mshader.setVec3("plight.diffuse",glm::vec3(1.0));
        mshader.setVec3("plight.specular",glm::vec3(1.0));
        mshader.setFloat("plight.constant",1.0f);
        mshader.setFloat("plight.linear",0.09f);
        mshader.setFloat("plight.qudratic",0.032f);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.0f,0,-4+sin(glfwGetTime())));
        model = glm::scale(model, glm::vec3(0.9f));
        mshader.setMat4("model",model);
        mshader.setMat4("view",view);
        mshader.setMat4("proj",proj);
        glBindVertexArray(VAO3);
        ourModel.Draw(mshader);

        shader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, t0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, t1);
        glBindVertexArray(VAO);
        for(unsigned int i = 1; i < 9; i+=2)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, angle+i, glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model",model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        s1.use();
        s1.setMat4("model",model);
        s1.setMat4("view",view);
        s1.setMat4("proj",proj);
        s1.setVec3("viewPos",cameraPos);

        s1.setVec3("material.ambient",glm::vec3(1.0,1.0,0));
        s1.setVec3("material.diffuse",glm::vec3(1.0,1.0,0));
        s1.setVec3("material.specular",glm::vec3(1.0));
        s1.setFloat("material.shininess",32);

        s1.setVec3("plight.position",cubePositions[9]);
        s1.setVec3("plight.ambient",glm::vec3(0.2));
        s1.setVec3("plight.diffuse",glm::vec3(1.0));
        s1.setVec3("plight.specular",glm::vec3(1.0));
        s1.setFloat("plight.constant",1.0f);
        s1.setFloat("plight.linear",0.09f);
        s1.setFloat("plight.qudratic",0.032f);

        s1.setVec3("dight.direction",glm::vec3(0,0,1));
        s1.setVec3("dlight.ambient",glm::vec3(0.2));
        s1.setVec3("dlight.diffuse",glm::vec3(1.0));
        s1.setVec3("dlight.specular",glm::vec3(1.0));

        glBindVertexArray(VAO2);

        for(unsigned int i = 0; i < 9; i+=2)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, (float)glfwGetTime()+i, glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model",model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        izvor.use();
        model = glm::mat4(1.0f);
        izvor.setMat4("model",model);
        izvor.setMat4("view",view);
        izvor.setMat4("proj",proj);
        izvor.setVec3("lcol",glm::vec3(1.0));

        //model = glm::translate(model, glm::vec3(cubePositions[9].x-3*(sin(glfwGetTime())+1),cubePositions[9].y,cubePositions[9].z));
        model = glm::translate(model, cubePositions[9]);
        float angle = 20.0f * 8;
        model = glm::scale(model,glm::vec3(0.2f));
        model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
        izvor.setMat4("model",model);

        glBindVertexArray(izvorVAO);
        //glDrawArrays(GL_TRIANGLES, 0, 36);
        glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }



    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
//    glDeleteVertexArrays(1, &VAO);
//    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float speed = 0.3;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraFront * speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraFront * speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront,cameraUp)) * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront,cameraUp)) * speed;
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        cameraPos += cameraUp * speed;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        cameraPos -= cameraUp * speed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

unsigned int load_texture(char const * path){

    unsigned int textureId;

    glGenTextures(1, &textureId);


    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if(nrChannels == 1)
            format = GL_RED;
        else if(nrChannels == 3)
            format = GL_RGB;
        else if(nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        stbi_image_free(data);
    }
    return textureId;
}