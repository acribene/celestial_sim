
#ifndef SIDEBAR_H
#define SIDEBAR_H
#include "raylib.h"
#include "raygui.h"
#include "Body.h"
#include "TimeManager.h"
#include "simulation.h"

// Enum for sidebar tabs
enum class SidebarTab {
    INSPECTOR,
    SETTINGS,
    INFO,
    CREATOR
};

class Sidebar {
private:
    // Layout
    Rectangle bounds_;
    float expandedWidth_ = 300.0f;
    float currentWidth_ = 0.0f;
    bool isOpen_ = false;
    
    // State
    Body* selectedBody_ = nullptr; // TODO: May change because of possible invalid pointers when bodies are removed
    SidebarTab currentTab_ = SidebarTab::INSPECTOR;
    Simulation& simulation_;
    TimeManager& timeManager_;

    Body tempBody_ = Body(); // Temporary body for creation tab

public:
    Sidebar(Simulation& sim, TimeManager& timeMgr);
    
    // Core loop
    void update(float dt);
    void render();
    void openCreationMenu(Vec2 worldPos);

    // Selection logic
    void selectBody(Body* body);
    void deselect();
    bool isMouseOver(); // Helper to prevent clicking through the UI
    bool hasSelection() const { return selectedBody_ != nullptr; }
};

#endif // SIDEBAR_H