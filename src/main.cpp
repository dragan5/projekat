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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_BACK);

    // build and compile our shader program
    // ------------------------------------
    Shader shader("resources/shaders/1.vs", "resources/shaders/1.fs");
    Shader s1("resources/shaders/2.vs", "resources/shaders/2.fs");
    Shader izvor("resources/shaders/izvor.vs", "resources/shaders/izvor.fs");
    Shader mshader("resources/shaders/model.vs", "resources/shaders/model.fs");
    Shader windowShader("resources/shaders/window.vs", "resources/shaders/window.fs");
    Shader aaShader("resources/shaders/aa.vs", "resources/shaders/aa.fs");
    Shader hdrShader("resources/shaders/hdr.vs", "resources/shaders/hdr.fs");

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

    }; //temena za indeksno crtanje izvora svetlosti

    float transparentVertices[] = {
            // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
            0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
            2.0f, -1.0f,  0.0f,  1.0f,  1.0f,

            0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            2.0f, -1.0f,  0.0f,  1.0f,  1.0f,
            2.0f,  1.0f,  0.0f,  1.0f,  0.0f
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

    float quadVertices[] = {   // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
            1.0f, -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f,  1.0f, 1.0f
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

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    unsigned int VAO2;  //zute kocke
    glGenVertexArrays(1,&VAO2);
    glBindVertexArray(VAO2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int VAO3;
    glGenVertexArrays(1,&VAO3);
    glBindVertexArray(VAO3);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a multisampled color attachment texture
    unsigned int textureColorBufferMultiSampled;
    glGenTextures(1, &textureColorBufferMultiSampled);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);
    // create a (also multisampled) renderbuffer object for depth and stencil attachments
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    // create floating point color buffer
    unsigned int colorBuffer;
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // create depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    // attach buffers
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    unsigned int t0 = load_texture("resources/textures/container2.png");
    unsigned int t1 = load_texture("resources/textures/container2_specular_colored.png");
    unsigned int t2 = load_texture("resources/textures/window.png");
    shader.use();
    shader.setInt("material.diffuse",0);
    shader.setInt("material.specular",1);
    aaShader.setInt("screenTexture",0);
    windowShader.setInt("texture1",0);

    Model ourModel(FileSystem::getPath("resources/objects/box/box.obj"));


    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    while (!glfwWindowShouldClose(window)){
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 proj = glm::mat4(1.0f);

        proj = glm::perspective(glm::radians(fov),(float)SCR_WIDTH/SCR_HEIGHT,0.1f,100.0f);

        shader.use();
        shader.setMat4("model",model);
        shader.setMat4("view",view);
        shader.setMat4("proj",proj);

        shader.setVec3("plight.position",cubePositions[9]);
        shader.setVec3("viewPos",cameraPos);
        shader.setFloat("material.shininess",16.0f);

        shader.setVec3("plight.ambient",glm::vec3(0.3f));
        shader.setVec3("plight.diffuse",glm::vec3(0.5f));
        shader.setVec3("plight.specular",glm::vec3(1.0f));
        shader.setFloat("plight.constant",1.0f);
        shader.setFloat("plight.linear",0.09f);
        shader.setFloat("plight.qudratic",0.032f);

        shader.setVec3("bluelight.position",glm::vec3(0.0f,-5.0f,-20.0f));
        shader.setVec3("bluelight.ambient",glm::vec3(0.1f,0.3f,200.0f));
        shader.setVec3("bluelight.diffuse",glm::vec3(0.1f,0.3f,200.0f));
        shader.setVec3("bluelight.specular",glm::vec3(0.9f));
        shader.setFloat("bluelight.constant",1.0f);
        shader.setFloat("bluelight.linear",0.09f);
        shader.setFloat("bluelight.qudratic",0.032f);

        shader.setVec3("dlight.direction",glm::vec3(0,0,-1));
        shader.setVec3("dlight.ambient",glm::vec3(0.2f));
        shader.setVec3("dlight.diffuse",glm::vec3(0.2f));
        shader.setVec3("dlight.specular",glm::vec3(0.5f));

        view = glm::lookAt(cameraPos,cameraFront + cameraPos,cameraUp);
        shader.setMat4("view",view);

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


        mshader.use();

        mshader.setVec3("plight.position",cubePositions[9]);
        mshader.setVec3("plight.ambient",glm::vec3(0.3f));
        mshader.setVec3("plight.diffuse",glm::vec3(0.5f));
        mshader.setVec3("plight.specular",glm::vec3(1.0f));
        mshader.setFloat("plight.constant",1.0f);
        mshader.setFloat("plight.linear",0.09f);
        mshader.setFloat("plight.qudratic",0.032f);

        mshader.setVec3("dlight.direction",glm::vec3(0,0,-1));
        mshader.setVec3("dlight.ambient",glm::vec3(0.2f));
        mshader.setVec3("dlight.diffuse",glm::vec3(0.2f));
        mshader.setVec3("dlight.specular",glm::vec3(0.5f));

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.0f,0,-4+sin(glfwGetTime())));
        model = glm::scale(model, glm::vec3(0.9f));
        mshader.setMat4("model",model);
        mshader.setMat4("view",view);
        mshader.setMat4("proj",proj);
        glBindVertexArray(VAO3);
        ourModel.Draw(mshader);

        s1.use(); //bele kocke
        s1.setMat4("model",model);
        s1.setMat4("view",view);
        s1.setMat4("proj",proj);
        s1.setVec3("viewPos",cameraPos);

        s1.setVec3("material.ambient",glm::vec3(0.9f,0.9f,0.9f));
        s1.setVec3("material.diffuse",glm::vec3(0.9f,0.9f,0.9f));
        s1.setVec3("material.specular",glm::vec3(0.7f));
        s1.setFloat("material.shininess",32.0f);

        s1.setVec3("plight.position",cubePositions[9]);
        s1.setVec3("plight.ambient",glm::vec3(0.3f));
        s1.setVec3("plight.diffuse",glm::vec3(0.5f));
        s1.setVec3("plight.specular",glm::vec3(1.0f));
        s1.setFloat("plight.constant",1.0f);
        s1.setFloat("plight.linear",0.09f);
        s1.setFloat("plight.qudratic",0.032f);

        s1.setVec3("bluelight.position",glm::vec3(0.0f,-5.0f,-20.0f));
        s1.setVec3("bluelight.ambient",glm::vec3(0.1f,0.3f,200.0f));
        s1.setVec3("bluelight.diffuse",glm::vec3(0.1f,0.3f,200.0f));
        s1.setVec3("bluelight.specular",glm::vec3(0.9f));
        s1.setFloat("bluelight.constant",1.0f);
        s1.setFloat("bluelight.linear",0.09f);
        s1.setFloat("bluelight.qudratic",0.032f);

        s1.setVec3("dlight.direction",glm::vec3(0,0,-1));
        s1.setVec3("dlight.ambient",glm::vec3(0.2f));
        s1.setVec3("dlight.diffuse",glm::vec3(0.2f));
        s1.setVec3("dlight.specular",glm::vec3(0.5f));

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
        glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);
        izvor.setVec3("lcol",glm::vec3(0.1f,0.3f,0.8f));
        model = glm::translate(model, glm::vec3(0.0f,-5.0f,-20.0f));
        izvor.setMat4("model",model);
        //shader.setVec3("plight.position",glm::vec3(0.0,-10.0,-20.0));
        glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);

        windowShader.use();
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glBindVertexArray(transparentVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, t2);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f,1.0f,1.0f));
        windowShader.setMat4("model", model);
        windowShader.setMat4("view", view);
        windowShader.setMat4("proj", proj);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisable(GL_CULL_FACE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        hdrShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        hdrShader.setFloat("exposure", 1.0f);

        aaShader.use();
        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled); // use the now resolved color attachment as the quad's texture
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
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