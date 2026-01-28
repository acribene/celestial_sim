
#ifndef SIDEBAR_H
#define SIDEBAR_H
#include "raylib.h"
#include "raygui.h"
#include "Body.h"
#include "TimeManager.h"

// Enum for sidebar tabs
enum SidebarTab {
    INSPECTOR,
    SETTINGS,
    INFO
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

public:
    Sidebar();
    
    // Core loop
    void update(float dt);
    void render(TimeManager& timeManager);

    // Selection logic
    void selectBody(Body* body);
    void deselect();
    bool isMouseOver(); // Helper to prevent clicking through the UI
    bool hasSelection() const { return selectedBody_ != nullptr; }
};

#endif // SIDEBAR_H