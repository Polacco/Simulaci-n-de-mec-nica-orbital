#include "raylib.h"
#include "Body.hpp"
#include <vector>
#include <cmath>

int main() {
    const int screenWidth = 1200;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Simulador de Mecanica Orbital - Curvatura Suave");

    // configuracion de camara
    Camera3D camera = { 0 };
    camera.position = { 0.0f, 15.0f, 50.0f }; 
    camera.target = { 0.0f, 0.0f, 0.0f };      
    camera.up = { 0.0f, 1.0f, 0.0f };          
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    double scale = 1e-6; 
    double dt = 100.0; 

    // tierra y satelite
    Phys::Body earth("Tierra", 5.972e24, Phys::Vector3(0, 0, 0), Phys::Vector3(0, 0, 0));
    Phys::Body sat("Sputnik", 1000.0, Phys::Vector3(8000000, 0, 0), Phys::Vector3(0, 0, 7500));

    std::vector<Vector3> trail;
    const int maxTrailPoints = 1500; 

    SetTargetFPS(60);
    DisableCursor(); // cursor oculto asi no moleste

    while (!WindowShouldClose()) {
        
        // rotar camara
        UpdateCamera(&camera, CAMERA_THIRD_PERSON);

        // fisica
        Phys::Vector3 force = sat.calculateGravitationalForce(earth);
        sat.applyForce(force);
        sat.update(dt);

        // logica de la estela de rastro
        Vector3 currentSatPos = { 
            static_cast<float>(sat.position.x * scale), 
            static_cast<float>(sat.position.y * scale), 
            static_cast<float>(sat.position.z * scale) 
        };

        trail.push_back(currentSatPos);
        if (trail.size() > maxTrailPoints) {
            trail.erase(trail.begin());
        }

        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode3D(camera);
                
                // grilla distorsionada | espacio-tiempo
                const int gridCount = 41; 
                const float gridSpacing = 2.5f; 
                const float visualDepthExaggeration = 8.0f; 

                Vector3 gridPoints[gridCount][gridCount];

                // calculo del hundimiento por cada punto
                for (int i = 0; i < gridCount; i++) {
                    for (int j = 0; j < gridCount; j++) {
                        float x = (i - gridCount/2) * gridSpacing;
                        float z = (j - gridCount/2) * gridSpacing;
                        float y = 0.0f;

                        float distEarth2 = x*x + z*z + 8.0f; 
                        float visualEarthDepth = (10.0f * visualDepthExaggeration) / std::sqrt(distEarth2);
                        y -= visualEarthDepth;

                        float dxSat = x - currentSatPos.x;
                        float dzSat = z - currentSatPos.z;
                        float distSat2 = dxSat*dxSat + dzSat*dzSat + 0.5f;
                        float visualSatDepth = (5.0f * visualDepthExaggeration) / std::sqrt(distSat2);
                        y -= visualSatDepth;

                        gridPoints[i][j] = { x, y, z };
                    }
                }

                for (int i = 0; i < gridCount; i++) {
                    for (int j = 0; j < gridCount; j++) {
                        if (i < gridCount - 1) DrawLine3D(gridPoints[i][j], gridPoints[i+1][j], Fade(DARKGRAY, 0.4f));
                        if (j < gridCount - 1) DrawLine3D(gridPoints[i][j], gridPoints[i][j+1], Fade(DARKGRAY, 0.4f));
                    }
                }

                DrawSphere({0, -1.5f, 0}, 2.0f, BLUE); // tierra
                DrawSphereWires({0, -1.5f, 0}, 2.1f, 16, 16, Fade(DARKBLUE, 0.3f));

                if (trail.size() >= 2) {
                    for (size_t i = 0; i < trail.size() - 1; i++) {
                        DrawLine3D(trail[i], trail[i + 1], Fade(RED, 0.7f));
                    }
                }

                DrawSphere(currentSatPos, 0.4f, WHITE); // satelite
                
            EndMode3D();

            DrawRectangle(10, 10, 300, 100, Fade(SKYBLUE, 0.1f));
            DrawText("GRAVITY WELL VISUALIZER", 20, 20, 15, Fade(YELLOW, 0.8f));
            DrawText(TextFormat("Velocidad: %.0f km/h", (sat.velocity.magnitude() * 3.6)), 20, 50, 20, Fade(GREEN, 0.8f));
            
            DrawFPS(screenWidth - 100, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}