#define RAYGUI_IMPLEMENTATION
#include "../headers/Sidebar.h"
#include <iostream>

Sidebar::Sidebar( Simulation& sim, TimeManager& timeMgr ) : simulation_(sim), timeManager_(timeMgr) {
    bounds_ = { 0, 0, 0, (float)GetScreenHeight() }; // Left side, full height
}

void Sidebar::update(float dt) {
    float targetWidth = isOpen_ ? expandedWidth_ : 0.0f;
    
    // Smooth slide animation (Linear Interpolation)
    currentWidth_ += (targetWidth - currentWidth_) * 10.0f * dt;
    
    bounds_.width = currentWidth_;
    bounds_.height = (float)GetScreenHeight(); // Handle window resizing
}

void Sidebar::render() {
    // Draw Background
    if (currentWidth_ < 1.0f) return; // Don't draw if closed

    DrawRectangleRec(bounds_, Fade(DARKGRAY, 0.95f));
    DrawRectangleLinesEx(bounds_, 2, LIGHTGRAY);

    // Draw Content only if expanded enough
    if (currentWidth_ > 200.0f) {
        
        // --- TABS (Dynamic Layout) ---
        // Calculate width so they fit perfectly within the sidebar
        float padding = 10.0f;
        float availableWidth = bounds_.width - (padding * 2);
        float tabWidth = availableWidth / 4.0f;
        float btnHeight = 30.0f;
        
        // Render Tabs
        if (GuiButton((Rectangle){ padding, 10, tabWidth, btnHeight }, "Inspect")) currentTab_ = SidebarTab::INSPECTOR;
        if (GuiButton((Rectangle){ padding + tabWidth, 10, tabWidth, btnHeight }, "Create")) currentTab_ = SidebarTab::CREATOR;
        if (GuiButton((Rectangle){ padding + tabWidth*2, 10, tabWidth, btnHeight }, "Settings")) currentTab_ = SidebarTab::SETTINGS;
        if (GuiButton((Rectangle){ padding + tabWidth*3, 10, tabWidth, btnHeight }, "Info")) currentTab_ = SidebarTab::INFO;

        // --- SEPARATOR ---
        DrawLine(10, 45, bounds_.width - 10, 45, LIGHTGRAY);

        if (currentTab_ == SidebarTab::INSPECTOR) {
            if (selectedBody_ != nullptr) {
                GuiLabel((Rectangle){ 10, 50, 200, 20 }, "Body Properties");
                
                // Position Readout
                GuiLabel((Rectangle){ 10, 70, 200, 20 }, TextFormat("Pos: %.2f, %.2f AU", selectedBody_->getPos().getX(), selectedBody_->getPos().getY()));

                // --- MASS (Log Scale) ---
                GuiLabel((Rectangle){ 10, 100, 200, 20 }, "Mass (Log Scale)");
                
                double currentMass = selectedBody_->getMass();
                float oldLogMass = (float)log10(currentMass); 
                float logMass = oldLogMass;
                
                // Allow full range -8 to 1.0 (Planetesimal to Star)
                GuiSlider((Rectangle){ 60, 120, 150, 20 }, "Mass", TextFormat("%.2e", currentMass), &logMass, -8.0f, 1.0f);
                
                if (logMass != oldLogMass) {
                    selectedBody_->setMass(pow(10.0, logMass));
                    
                    // Optional: Auto-scale radius in Inspector too?
                    // This keeps it consistent with the Creator tab behavior
                    double newRadius = 0.02 + 0.005 * (logMass + 8.0);
                    selectedBody_->setRadius(newRadius);
                }

                // --- RADIUS (Read Only / Auto) ---
                GuiLabel((Rectangle){ 10, 150, 200, 20 }, "Radius (Auto-Scaled)");
                GuiStatusBar((Rectangle){ 60, 170, 150, 20 }, TextFormat("%.4f AU", selectedBody_->getRadius()));

                // --- VELOCITY (Direct Control) ---
                // 1. Get local copy
                Vec2 currentVel = selectedBody_->getVel();

                GuiLabel((Rectangle){ 10, 200, 200, 20 }, "Velocity X (AU/yr)");
                float tempVelX = (float)currentVel.getX();
                GuiSlider((Rectangle){ 60, 220, 150, 20 }, "VX", TextFormat("%.2f", tempVelX), &tempVelX, -10.0f, 10.0f);
                currentVel.setX(tempVelX);

                GuiLabel((Rectangle){ 10, 250, 200, 20 }, "Velocity Y (AU/yr)");
                float tempVelY = (float)currentVel.getY();
                GuiSlider((Rectangle){ 60, 270, 150, 20 }, "VY", TextFormat("%.2f", tempVelY), &tempVelY, -10.0f, 10.0f);
                currentVel.setY(tempVelY);

                // 2. Write back to real body
                selectedBody_->setVel(currentVel);

                // --- DELETE BUTTON ---
                // Moved down slightly to make room for velocity controls
                if (GuiButton((Rectangle){ 10, 320, availableWidth, 40 }, "DELETE BODY")) {
                    simulation_.DeleteBodyAt( selectedBody_->getPos() );
                    deselect();
                }
            } else {
                GuiLabel((Rectangle){ 10, 50, 200, 20 }, "No body selected.");
                GuiLabel((Rectangle){ 10, 70, 200, 20 }, "Click a body to inspect.");
            }
        }
        else if(currentTab_ == SidebarTab::SETTINGS) {
            // --- BARNES-HUT THETA ---

            GuiLabel((Rectangle){ 10, 80, 200, 20 }, "Barnes-Hut Theta (Accuracy)");
            
            // Convert double to float for RayGui
            float currentTheta = (float)simulation_.getQuadtree().getTheta();
            float oldTheta = currentTheta;

            // Slider Range: 0.0 (More Accurate/Slower) to 2.0 (Less Accurate/Faster)
            // 0.5 is a common default for Barnes-Hut
            GuiSlider((Rectangle){ 60, 100, 150, 20 }, "Theta", TextFormat("%.2f", currentTheta), &currentTheta, 0.0f, 2.0f);
            
            // Only update if changed to avoid unnecessary writes
            if (currentTheta != oldTheta) {
                simulation_.getQuadtree().setTheta((double)currentTheta);
            }

            // Explanatory Text
            GuiLabel((Rectangle){ 10, 130, 230, 20 }, "0.0 = Brute Force (Slowest)");
            GuiLabel((Rectangle){ 10, 150, 230, 20 }, "0.5 = Balanced");
            GuiLabel((Rectangle){ 10, 170, 230, 20 }, ">1.0 = Fast Approximation");
        }
        else if(currentTab_ == SidebarTab::INFO) {            
            GuiLabel((Rectangle){ 10, 50, 200, 20 }, "2D Physics Simulator");
            
            GuiLabel((Rectangle){ 10, 80, 200, 20 }, TextFormat("FPS: %d", GetFPS()));
            GuiLabel((Rectangle){ 10, 100, 200, 20 }, TextFormat("Time Scale: %.2f", timeManager_.getTimeScale()));

            if (timeManager_.getPauseState()) { 
                DrawText("PAUSED", 10, 130, 20, RED);
                GuiLabel((Rectangle){ 10, 150, 200, 20 }, "(Press ENTER to Step)");
            }

            int startY = 200;
            GuiLabel((Rectangle){ 10.0f, (float)startY, 250, 20 }, "CONTROLS:");
            GuiLabel((Rectangle){ 10.0f, (float)startY + 25, 250, 20 }, "- Arrow keys: Pan Camera");
            GuiLabel((Rectangle){ 10.0f, (float)startY + 45, 250, 20 }, "- Scroll / A / Z: Zoom");
            GuiLabel((Rectangle){ 10.0f, (float)startY + 65, 250, 20 }, "- +/- : Change Time Scale");
            GuiLabel((Rectangle){ 10.0f, (float)startY + 85, 250, 20 }, "- Space : Pause/Resume");
        }
        else if(currentTab_ == SidebarTab::CREATOR) {
           GuiLabel((Rectangle){ 10, 50, 200, 20 }, "Create New Body");
            GuiLabel((Rectangle){ 10, 70, 200, 20 }, TextFormat("Pos: %.2f, %.2f AU", tempBody_.getPos().getX(), tempBody_.getPos().getY()));

            // --- MASS (Log Scale) ---
            GuiLabel((Rectangle){ 10, 100, 200, 20 }, "Mass (Log Scale)");
            
            // 1. Get current log value from the real mass
            float currentLogMass = (float)log10(tempBody_.getMass());
            
            // 2. Draw Slider using the log value
            GuiSlider((Rectangle){ 60, 120, 150, 20 }, "Mass", TextFormat("%.2e Msun", tempBody_.getMass()), &currentLogMass, -8.0f, -4.0f);
            
            // 3. Convert back to real mass and update body
            tempBody_.setMass(pow(10.0, currentLogMass));

            // --- RADIUS (Auto-calculated) ---
            // Formula: same as in rendering for consistency
            double calculatedRadius = 0.02 + 0.005 * (currentLogMass + 8.0);
            
            // Update the temp body immediately
            tempBody_.setRadius(calculatedRadius);

            // Display as Read-Only Label instead of Slider
            GuiLabel((Rectangle){ 10, 150, 200, 20 }, "Radius (Auto-Scaled)");
            // Draw a disabled slider or just a value box to visualize it
            GuiStatusBar((Rectangle){ 60, 170, 150, 20 }, TextFormat("%.4f AU", calculatedRadius));

            // --- VELOCITY ---
            Vec2 currentVel = tempBody_.getVel();
            GuiLabel((Rectangle){ 10, 200, 200, 20 }, "Velocity X (AU/yr)");
            float tempVelX = (float)currentVel.getX();
            GuiSlider((Rectangle){ 60, 220, 150, 20 }, "VX", TextFormat("%.2f", tempVelX), &tempVelX, -10.0f, 10.0f);
            currentVel.setX(tempVelX);

            GuiLabel((Rectangle){ 10, 250, 200, 20 }, "Velocity Y (AU/yr)");
            float tempVelY = (float)currentVel.getY();
            GuiSlider((Rectangle){ 60, 270, 150, 20 }, "VY", TextFormat("%.2f", tempVelY), &tempVelY, -10.0f, 10.0f);
            currentVel.setY(tempVelY);

            tempBody_.setVel(currentVel);

            // Create Button
            if (GuiButton((Rectangle){ 10, 320, 220, 40 }, "SPAWN BODY")) {
                Body newBody( tempBody_ );
                selectedBody_ = simulation_.addBody( newBody );                
                currentTab_ = SidebarTab::INSPECTOR;
                timeManager_.togglePause(); // Unpause on creation
            }
        }
    }
}

void Sidebar::openCreationMenu(Vec2 worldPos) {
    tempBody_ = Body(); // Reset temporary body
    tempBody_.setPos(worldPos);
    currentTab_ = SidebarTab::CREATOR;
    isOpen_ = true;
    
    // Store REAL mass (10^-6), not the log value (-6.0)
    tempBody_.setMass(pow(10.0, -6.0)); 
    
    // Auto-calculate initial radius based on the mass
    double logMass = -6.0;
    double autoRadius = 0.02 + 0.005 * (logMass + 8.0);
    tempBody_.setRadius(autoRadius);
    
    tempBody_.setVel(Vec2(0.0, 0.0));
}

void Sidebar::selectBody(Body* body) {
    selectedBody_ = body;
    isOpen_ = true; // Auto open on click
}

void Sidebar::deselect() {
    selectedBody_ = nullptr;
    isOpen_ = false;
}

bool Sidebar::isMouseOver() {
    Vector2 mouse = GetMousePosition();
    return CheckCollisionPointRec(mouse, bounds_);
}