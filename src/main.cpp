// Core libraries
#include <iostream>
#include <cmath>

// Third party libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Local headers
#include "GLSL.h"
#include "Program.h"
#include "WindowManager.h"
#include "Shape.h"
#include "Camera.h"

using namespace std;
using namespace glm;

char animationText[] = "A Hello, World! program is traditionally used to introduce novice programmers to a programming language. Hello, world! is also traditionally used in a sanity test to make sure that a computer language is correctly installed, and that the operator understands how to use it. The phrase is also used by computer hackers as a proof of concept that arbitrary code can be executed through an exploit where the system designers did not intend code to be executed. \0";
int animationTextIndex = 0;

double get_last_elapsed_time() {
	static double lasttime = glfwGetTime();
	double actualtime = glfwGetTime();
	double difference = actualtime - lasttime;
	lasttime = actualtime;
	return difference;
}

class Application : public EventCallbacks {
public:
	WindowManager *windowManager = nullptr;
    Camera *camera = nullptr;

    std::shared_ptr<Shape> face_closed, face_generic_consonant, face_AEI, face_BMP, face_Ch, face_FV, face_O, face_R, face_Th, face_U;
	std::shared_ptr<Program> phongShader;
	GLuint VAOid, face_closed_VBOid, face_generic_consonant_VBOid, face_AEI_VBOid, face_BMP_VBOid, face_Ch_VBOid, face_FV_VBOid, face_O_VBOid, face_R_VBOid, face_Th_VBOid, face_U_VBOid;

    double gametime = 0;
    bool wireframeEnabled = false;
    bool mousePressed = false;
    bool mouseCaptured = false;
    glm::vec2 mouseMoveOrigin = glm::vec2(0);
    glm::vec3 mouseMoveInitialCameraRot;

    Application() {
        camera = new Camera();
    }
    
    ~Application() {
        delete camera;
    }

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
		// Movement
        if (key == GLFW_KEY_W && action != GLFW_REPEAT) camera->vel.z = (action == GLFW_PRESS) * -0.2f;
        if (key == GLFW_KEY_S && action != GLFW_REPEAT) camera->vel.z = (action == GLFW_PRESS) * 0.2f;
        if (key == GLFW_KEY_A && action != GLFW_REPEAT) camera->vel.x = (action == GLFW_PRESS) * -0.2f;
        if (key == GLFW_KEY_D && action != GLFW_REPEAT) camera->vel.x = (action == GLFW_PRESS) * 0.2f;
        // Rotation
        if (key == GLFW_KEY_I && action != GLFW_REPEAT) camera->rotVel.x = (action == GLFW_PRESS) * 0.02f;
        if (key == GLFW_KEY_K && action != GLFW_REPEAT) camera->rotVel.x = (action == GLFW_PRESS) * -0.02f;
        if (key == GLFW_KEY_J && action != GLFW_REPEAT) camera->rotVel.y = (action == GLFW_PRESS) * 0.02f;
        if (key == GLFW_KEY_L && action != GLFW_REPEAT) camera->rotVel.y = (action == GLFW_PRESS) * -0.02f;
        if (key == GLFW_KEY_U && action != GLFW_REPEAT) camera->rotVel.z = (action == GLFW_PRESS) * 0.02f;
        if (key == GLFW_KEY_O && action != GLFW_REPEAT) camera->rotVel.z = (action == GLFW_PRESS) * -0.02f;
        // Polygon mode (wireframe vs solid)
        if (key == GLFW_KEY_P && action == GLFW_PRESS) {
            wireframeEnabled = !wireframeEnabled;
            glPolygonMode(GL_FRONT_AND_BACK, wireframeEnabled ? GL_LINE : GL_FILL);
        }
        // Hide cursor (allows unlimited scrolling)
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
            mouseCaptured = !mouseCaptured;
            glfwSetInputMode(window, GLFW_CURSOR, mouseCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            resetMouseMoveInitialValues(window);
        }
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods) {
        mousePressed = (action != GLFW_RELEASE);
        if (action == GLFW_PRESS) {
            resetMouseMoveInitialValues(window);
        }
    }
    
    void mouseMoveCallback(GLFWwindow *window, double xpos, double ypos) {
        if (mousePressed || mouseCaptured) {
            float yAngle = (xpos - mouseMoveOrigin.x) / windowManager->getWidth() * 3.14159f;
            float xAngle = (ypos - mouseMoveOrigin.y) / windowManager->getHeight() * 3.14159f;
            camera->setRotation(mouseMoveInitialCameraRot + glm::vec3(-xAngle, -yAngle, 0));
        }
    }

	void resizeCallback(GLFWwindow *window, int in_width, int in_height) { }
    
    // Reset mouse move initial position and rotation
    void resetMouseMoveInitialValues(GLFWwindow *window) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        mouseMoveOrigin = glm::vec2(mouseX, mouseY);
        mouseMoveInitialCameraRot = camera->rot;
    }

	void initGeom(const std::string& resourceDirectory) {

		face_closed = make_shared<Shape>();
		face_closed->loadMesh(resourceDirectory + "/face closed.obj");
		face_closed->resize();
		face_closed->init();

		face_generic_consonant = make_shared<Shape>();
		face_generic_consonant->loadMesh(resourceDirectory + "/face Generic.obj");
		face_generic_consonant->resize();
		face_generic_consonant->init();

		face_AEI = make_shared<Shape>();
		face_AEI->loadMesh(resourceDirectory + "/face AEI.obj");
		face_AEI->resize();
		face_AEI->init();

		face_BMP = make_shared<Shape>();
		face_BMP->loadMesh(resourceDirectory + "/face BMP.obj");
		face_BMP->resize();
		face_BMP->init();

		face_Ch = make_shared<Shape>();
		face_Ch->loadMesh(resourceDirectory + "/face Ch.obj");
		face_Ch->resize();
		face_Ch->init();

		face_O = make_shared<Shape>();
		face_O->loadMesh(resourceDirectory + "/face O.obj");
		face_O->resize();
		face_O->init();

		face_Th = make_shared<Shape>();
		face_Th->loadMesh(resourceDirectory + "/face Th.obj");
		face_Th->resize();
		face_Th->init();

		face_FV = make_shared<Shape>();
		face_FV->loadMesh(resourceDirectory + "/face FV.obj");
		face_FV->resize();
		face_FV->init();

		face_R = make_shared<Shape>();
		face_R->loadMesh(resourceDirectory + "/face R.obj");
		face_R->resize();
		face_R->init();

		face_U = make_shared<Shape>();
		face_U->loadMesh(resourceDirectory + "/face U.obj");
		face_U->resize();
		face_U->init();

		glGenVertexArrays(1, &VAOid);
		glBindVertexArray(VAOid);

		glGenBuffers(1, &face_closed_VBOid);
		glBindBuffer(GL_ARRAY_BUFFER, face_closed_VBOid);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * face_closed->posBuf->size(), face_closed->posBuf->data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &face_generic_consonant_VBOid);
		glBindBuffer(GL_ARRAY_BUFFER, face_generic_consonant_VBOid);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * face_generic_consonant->posBuf->size(), face_generic_consonant->posBuf->data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &face_AEI_VBOid);
		glBindBuffer(GL_ARRAY_BUFFER, face_AEI_VBOid);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * face_AEI->posBuf->size(), face_AEI->posBuf->data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &face_BMP_VBOid);
		glBindBuffer(GL_ARRAY_BUFFER, face_BMP_VBOid);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * face_BMP->posBuf->size(), face_BMP->posBuf->data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &face_Ch_VBOid);
		glBindBuffer(GL_ARRAY_BUFFER, face_Ch_VBOid);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * face_Ch->posBuf->size(), face_Ch->posBuf->data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &face_O_VBOid);
		glBindBuffer(GL_ARRAY_BUFFER, face_O_VBOid);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * face_O->posBuf->size(), face_O->posBuf->data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &face_Th_VBOid);
		glBindBuffer(GL_ARRAY_BUFFER, face_Th_VBOid);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * face_Th->posBuf->size(), face_Th->posBuf->data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &face_FV_VBOid);
		glBindBuffer(GL_ARRAY_BUFFER, face_FV_VBOid);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * face_FV->posBuf->size(), face_FV->posBuf->data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &face_R_VBOid);
		glBindBuffer(GL_ARRAY_BUFFER, face_R_VBOid);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * face_R->posBuf->size(), face_R->posBuf->data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &face_U_VBOid);
		glBindBuffer(GL_ARRAY_BUFFER, face_U_VBOid);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * face_U->posBuf->size(), face_U->posBuf->data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(9);
		glVertexAttribPointer(9, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindVertexArray(0);
	}
	
	void init(const std::string& resourceDirectory) {
		GLSL::checkVersion();

		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL programs
        phongShader = std::make_shared<Program>();
        phongShader->setShaderNames(resourceDirectory + "/phong.vert", resourceDirectory + "/phong.frag");
        phongShader->init();
		if (!phongShader->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			//exit(1);
		}

		phongShader->addUniform("current");
		phongShader->addUniform("next");
		phongShader->addUniform("t");
	}
    
    glm::mat4 getPerspectiveMatrix() {
        float fov = 3.14159f / 4.0f;
        float aspect = windowManager->getAspect();
        return glm::perspective(fov, aspect, 0.01f, 10000.0f);
    }

	void render() {
		double frametime = get_last_elapsed_time();
		gametime += frametime;

		// Clear framebuffer.
		glClearColor(0.3f, 0.7f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks.
		glm::mat4 V, M, P;
        P = getPerspectiveMatrix();
        V = camera->getViewMatrix();
        M = glm::mat4(1);

		float currentFaceChoice;
		float nextFaceChoice;

		char currentChar = animationText[animationTextIndex];
		char nextChar = animationText[animationTextIndex + 1];

		float t = (gametime / 0.125);

		if (gametime > 0.125)
		{
			animationTextIndex++;
			gametime = 0.0;
		}

		if (animationTextIndex + 2 == (sizeof(animationText) / sizeof(animationText[0])))
		{
			animationTextIndex = 0;
		}

		switch (currentChar)
		{
			case 'A':
				currentFaceChoice = 2.0f;
				break; //optional
			case 'E':
				currentFaceChoice = 2.0f;
				break; //optional
			case 'I':
				currentFaceChoice = 2.0f;
				break; //optional
			case 'a':
				currentFaceChoice = 2.0f;
				break; //optional
			case 'e':
				currentFaceChoice = 2.0f;
				break; //optional
			case 'i':
				currentFaceChoice = 2.0f;
				break; //optional

			case 'B':
				currentFaceChoice = 3.0f;
				break; //optional
			case 'M':
				currentFaceChoice = 3.0f;
				break; //optional
			case 'P':
				currentFaceChoice = 3.0f;
				break; //optional
			case 'b':
				currentFaceChoice = 3.0f;
				break; //optional
			case 'm':
				currentFaceChoice = 3.0f;
				break; //optional
			case 'p':
				currentFaceChoice = 3.0f;
				break; //optional

			case 'C':
				if (nextChar == 'H' || nextChar == 'h')
				{
					currentFaceChoice = 4.0f;
				}
				else
				{
					currentFaceChoice = 1.0f;
				}
				break; //optional
			case 'c':
				if (nextChar == 'H' || nextChar == 'h')
				{
					currentFaceChoice = 4.0f;
				}
				else
				{
					currentFaceChoice = 1.0f;
				}
				break; //optional

			case 'O':
				currentFaceChoice = 5.0f;
				break; //optional
			case 'o':
				currentFaceChoice = 5.0f;
				break; //optional

			case 'T':
				if (nextChar == 'H' || nextChar == 'h')
				{
					currentFaceChoice = 6.0f;
				}
				else
				{
					currentFaceChoice = 1.0f;
				}
				break; //optional
			case 't':
				if (nextChar == 'H' || nextChar == 'h')
				{
					currentFaceChoice = 6.0f;
				}
				else
				{
					currentFaceChoice = 1.0f;
				}
				break; //optional

			case 'F':
				currentFaceChoice = 7.0f;
				break; //optional
			case 'V':
				currentFaceChoice = 7.0f;
				break; //optional
			case 'f':
				currentFaceChoice = 7.0f;
				break; //optional
			case 'v':
				currentFaceChoice = 7.0f;
				break; //optional

			case 'R':
				currentFaceChoice = 8.0f;
				break; //optional
			case 'r':
				currentFaceChoice = 8.0f;
				break; //optional

			case 'U':
				currentFaceChoice = 9.0f;
				break; //optional
			case 'u':
				currentFaceChoice = 9.0f;
				break; //optional

			case ' ':
				currentFaceChoice = 0.0f;
				break; //optional
			case '.':
				currentFaceChoice = 0.0f;
				break; //optional
			case '?':
				currentFaceChoice = 0.0f;
				break; //optional
			case '!':
				currentFaceChoice = 0.0f;
				break; //optional
		
			default: //Optional
				currentFaceChoice = 1.0f;
				break; //optional
		}

		switch (nextChar)
		{
			case 'A':
				nextFaceChoice = 2.0f;
				break; //optional
			case 'E':
				nextFaceChoice = 2.0f;
				break; //optional
			case 'I':
				nextFaceChoice = 2.0f;
				break; //optional
			case 'a':
				nextFaceChoice = 2.0f;
				break; //optional
			case 'e':
				nextFaceChoice = 2.0f;
				break; //optional
			case 'i':
				nextFaceChoice = 2.0f;
				break; //optional

			case 'B':
				nextFaceChoice = 3.0f;
				break; //optional
			case 'M':
				nextFaceChoice = 3.0f;
				break; //optional
			case 'P':
				nextFaceChoice = 3.0f;
				break; //optional
			case 'b':
				nextFaceChoice = 3.0f;
				break; //optional
			case 'm':
				nextFaceChoice = 3.0f;
				break; //optional
			case 'p':
				nextFaceChoice = 3.0f;
				break; //optional

			case 'C':
				if (animationText[animationTextIndex + 1] == 'H' || animationText[animationTextIndex + 1] == 'h')
				{
					nextFaceChoice = 4.0f;
				}
				else
				{
					nextFaceChoice = 1.0f;
				}
				break; //optional
			case 'c':
				if (animationText[animationTextIndex + 1] == 'H' || animationText[animationTextIndex + 1] == 'h')
				{
					nextFaceChoice = 4.0f;
				}
				else
				{
					nextFaceChoice = 1.0f;
				}
				break; //optional

			case 'O':
				nextFaceChoice = 5.0f;
				break; //optional
			case 'o':
				nextFaceChoice = 5.0f;
				break; //optional

			case 'T':
				if (animationText[animationTextIndex + 1] == 'H' || animationText[animationTextIndex + 1] == 'h')
				{
					nextFaceChoice = 6.0f;
				}
				else
				{
					nextFaceChoice = 1.0f;
				}
				break; //optional
			case 't':
				if (animationText[animationTextIndex + 1] == 'H' || animationText[animationTextIndex + 1] == 'h')
				{
					nextFaceChoice = 6.0f;
				}
				else
				{
					nextFaceChoice = 1.0f;
				}
				break; //optional

			case 'F':
				nextFaceChoice = 7.0f;
				break; //optional
			case 'V':
				nextFaceChoice = 7.0f;
				break; //optional
			case 'f':
				nextFaceChoice = 7.0f;
				break; //optional
			case 'v':
				nextFaceChoice = 7.0f;
				break; //optional

			case 'R':
				nextFaceChoice = 8.0f;
				break; //optional
			case 'r':
				nextFaceChoice = 8.0f;
				break; //optional

			case 'U':
				nextFaceChoice = 9.0f;
				break; //optional
			case 'u':
				nextFaceChoice = 9.0f;
				break; //optional

			case ' ':
				nextFaceChoice = 0.0f;
				break; //optional
			case '.':
				nextFaceChoice = 0.0f;
				break; //optional
			case '?':
				nextFaceChoice = 0.0f;
				break; //optional
			case '!':
				nextFaceChoice = 0.0f;
				break; //optional

			default: //Optional
				nextFaceChoice = 1.0f;
				break; //optional
		}
        
        /**************/
        /* DRAW SHAPE */
        /**************/

		glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(0.0f, 0.0f, -3.0f));
		glm::mat4 yRotation = glm::rotate(glm::mat4(1), (3.14159f / 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		M = translation * yRotation;

        phongShader->bind();
        phongShader->setMVP(&M[0][0], &V[0][0], &P[0][0]);

		
		glUniform1f(phongShader->getUniform("current"), currentFaceChoice);
		glUniform1f(phongShader->getUniform("next"), nextFaceChoice);
		glUniform1f(phongShader->getUniform("t"), t);


		std::cout << currentFaceChoice << ", " << nextFaceChoice << ", " << t << std::endl;

		glBindVertexArray(VAOid);
		glDrawArrays(GL_TRIANGLES, 0, 2274);

        phongShader->unbind();
	}
};

int main(int argc, char **argv) {
	std::string resourceDir = "../resources";
	if (argc >= 2) {
		resourceDir = argv[1];
	}

	Application *application = new Application();

    // Initialize window.
	WindowManager * windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// Initialize scene.
	application->init(resourceDir);
	application->initGeom(resourceDir);
    
	// Loop until the user closes the window.
	while (!glfwWindowShouldClose(windowManager->getHandle())) {
        // Update camera position.
        application->camera->update();
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
