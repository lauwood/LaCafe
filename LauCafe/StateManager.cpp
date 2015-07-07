////////////////////////////////////////
// StateManager.cpp
////////////////////////////////////////

#include "StateManager.h"
#include "TimeManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

static StateManager* StateMan;
bool ShowDebugger = true;

////////////////////////////////////////////////////////////////////////////////

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)				{ glViewport(0, 0, width, height); }
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)		{ StateMan->KeyCallback(window, key, scancode, action, mods); }
static void mouse_callback(GLFWwindow* window, int button, int action, int mods)				{ StateMan->MouseButtonCallback(window, button, action, mods); }

////////////////////////////////////////////////////////////////////////////////

void StateManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_F1:
			if (b_EditMode) {
				// delete state edit mode
				PopState();
				PeekState()->Resume();
				b_EditMode = false;
			} else {
				PeekState()->Pause();
				EditState *State_EditMode= new EditState(window, a);
				State_EditMode->SetSquarePath(((PlayState*)PeekState())->GetSquarePath());
				PushState(State_EditMode);
				b_EditMode = true;
			}
			fprintf(stdout, "F1 pressed");
			break;
		case GLFW_KEY_F2:
			fprintf(stdout, "F2 pressed");
			ShowDebugger = !ShowDebugger;
			break;
		}
	}
}

void StateManager::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	int b;
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		b = MOUSE_LEFT;
		break;

	case GLFW_MOUSE_BUTTON_MIDDLE:
		b = MOUSE_MIDDLE;
		break;

	case GLFW_MOUSE_BUTTON_RIGHT:
		b = MOUSE_RIGHT;
		break;

	default:
		b = 0;
	}

	if (action == GLFW_PRESS) {
		MouseActiveButton |= b;
	}
	else {
		MouseActiveButton &= ~b;
	}

	PeekState()->SetMouseActiveButton(MouseActiveButton);
}

StateManager::StateManager(GLFWwindow* window) {
	a = new Area(10, 10, 0, 0);
	b_EditMode = false;

	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);

	SetWindow(window);
	PlayState *state = new PlayState(GetWindow(), a);
	PushState(state);
	StateMan = this;
}


////////////////////////////////////////////////////////////////////////////////

void StateManager::GameLoop() {
	ImGui_ImplGlfwGL3_Init(window, false);

	ImVec4 clear_color = ImColor(114, 144, 154);

	while (!glfwWindowShouldClose(GetWindow())) {
		TimeManager::Instance().CalculateFrameRate(false);
		// timers
		UpdateFPSCounter(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		ImGuiIO& io = ImGui::GetIO();
		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();

		if (ShowDebugger) {
			ImGui::Text("Debugger (hide with F2)");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}

		Input();
		Update();
		Draw();

		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		ImGui::Render();
		glfwSwapBuffers(window);
	}
}

////////////////////////////////////////////////////////////////////////////////

void StateManager::Quit() {
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////