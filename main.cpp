#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "Table.hpp"
#include "Column.hpp"
#include <imgui_node_editor.h>
namespace ed = ax::NodeEditor;

int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "VisualSQL - Start", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // V-Sync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    Table usersTable("Uzytkownicy");

    usersTable.addColumn(Column("id", "INT"));
    usersTable.addColumn(Column("email", "VARCHAR(255)"));
    usersTable.addColumn(Column("created_at", "TIMESTAMP"));

    ed::Config config;
    config.SettingsFile = "VisualSQL_NodeEditor.json";
    ed::EditorContext* m_Context = ed::CreateEditor(&config);
    ed::SetCurrentEditor(m_Context);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                ImGui::MenuItem("Zapisz", "Ctrl+S");
                ImGui::MenuItem("Zakoncz");
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                ImGui::MenuItem("Cofnij");
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Prybliz");
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        float screenWidth = viewport->WorkSize.x;
        float screenHeight = viewport->WorkSize.y;

        float leftPanelWidth = 60.0f;
        float rightPanelWidth = 250.0f;

        ImGuiWindowFlags panelFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, screenHeight));

        ImGui::Begin("LeftPanel", nullptr, panelFlags);
        ImGui::Text("Narzedzia");
        ImGui::Separator();
        if (ImGui::Button("+", ImVec2(40, 40))) {
            // W przyszłości będzie tu dodana nowa tabela
        }
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + screenWidth - rightPanelWidth, viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(rightPanelWidth, screenHeight));

        ImGui::Begin("RightPanel", nullptr, panelFlags);
        ImGui::Text("Wlasciwosci");
        ImGui::Separator();
        ImGui::Text("Wybierz tabele..."); // Tu pojawią się detale klikniętej tabeli
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + leftPanelWidth, viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(screenWidth - leftPanelWidth - rightPanelWidth, screenHeight));

        ImGui::Begin("Workspace", nullptr, panelFlags);

        ed::Begin("My Editor", ImVec2(0.0, 0.0f));

        ed::BeginNode(1);
        ImGui::Text("Tabela: %s", usersTable.getName().c_str());
        ImGui::Separator();
        for (size_t i = 0; i < usersTable.getColumns().size(); i++) {
            Column col = usersTable.getColumns()[i];
            ImGui::Text("%s : %s", col.getName().c_str(), col.getType().c_str());
        }
        ed::EndNode();

        ed::End();
        ImGui::End();

        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ed::DestroyEditor(m_Context);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}