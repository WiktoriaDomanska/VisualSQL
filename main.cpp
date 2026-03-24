#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "Table.hpp"
#include "Column.hpp"
#include <imgui_node_editor.h>
#include <string>
#include "Schema.hpp"
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
    io.FontGlobalScale = 1.3f;
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

    Schema mySchema("Mój projekt VisualSQL");
    int tableCounter = 1;
    ed::NodeId selectedNodeId = 0;

    Table startTable("Uzytkownicy");
    startTable.addColumn(Column("id", "INT"));
    mySchema.addTable(startTable);

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
                if (ImGui::MenuItem("Dopasuj widok (Zoom to Fit)")) {
                    ed::NavigateToContent();
                }
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
            std::string nowaNazwa = "Tabela_" + std::to_string(tableCounter);
            tableCounter++;

            Table newTable(nowaNazwa);
            newTable.addColumn(Column("id", "INT"));

            mySchema.addTable(newTable);
        }

        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + screenWidth - rightPanelWidth, viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(rightPanelWidth, screenHeight));

        ImGui::Begin("RightPanel", nullptr, panelFlags);
        ImGui::Text("Wlasciwosci");
        ImGui::Separator();

        if (selectedNodeId.Get() != 0) {
            int tableIndex = selectedNodeId.Get() - 1;

            if (tableIndex >= 0 && tableIndex < mySchema.getTables().size()) {
                const Table& selectedTable = mySchema.getTables()[tableIndex];

                ImGui::Text("Wybrana tabela:");
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", selectedTable.getName().c_str());

                ImGui::Separator();
                ImGui::Text("Lista kolumn:");
                for (size_t i = 0; i < selectedTable.getColumns().size(); i++) {
                    Column col = selectedTable.getColumns()[i];
                    ImGui::BulletText("%s (%s)", col.getName().c_str(), col.getType().c_str());
                }
            }
        }
        else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Kliknij w wezel na plotnie.");
        }

        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + leftPanelWidth, viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(screenWidth - leftPanelWidth - rightPanelWidth, screenHeight));

        ImGui::Begin("Workspace", nullptr, panelFlags);

        ed::Begin("My Editor", ImVec2(0.0, 0.0f));

        const std::vector<Table>& wszystkieTabele = mySchema.getTables();

        for (size_t i = 0; i < wszystkieTabele.size(); i++) {
            ed::BeginNode(i + 1);

            ImGui::Text("Tabela: %s", wszystkieTabele[i].getName().c_str());
            ImGui::Separator();

            for (size_t j = 0; j < wszystkieTabele[i].getColumns().size(); j++) {
                Column col = wszystkieTabele[i].getColumns()[j];
                ImGui::Text("%s : %s", col.getName().c_str(), col.getType().c_str());
            }

            ed::EndNode();
        }

        ed::NodeId selectedNodes[1];
        int selectedCount = ed::GetSelectedNodes(selectedNodes, 1);

        if (selectedCount > 0) {
            selectedNodeId = selectedNodes[0];
        } else {
            selectedNodeId = 0;
        }

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