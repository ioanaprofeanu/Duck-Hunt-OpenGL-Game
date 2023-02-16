#pragma once

#include "components/simple_scene.h"
#include "components/text_renderer.h"


namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
     public:
         Tema1();
        ~Tema1();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void displayDuck(float deltaTimeSeconds);
        void displayGameElements(float deltaTimeSeconds);
        void displaySceneryElements();
        void createDuckMeshes();
        float generateAngle();

     protected:
        glm::mat3 modelMatrix;
        glm::mat3 modelMatrixForAll;
        float translateX, translateY;
        float scaleX, scaleY;
        clock_t timeElapsed;
        int ducksGenerated;

        // other variables
        bool decreaseX;
        bool decreaseY;
        float moveWingsAngle;
        bool fullyMovedWings;
        bool escaped;
        bool doneEscaping;
        float translateXEscaping;
        int lives;
        int bullets;
        float dimX;
        float dimY;
        float duckCentre;
        float squareSide;
        float OXstartSquare;
        float OYstartSquare;
        float OXendSquare;
        float OYendSquare;
        float OXSquareMiddleInit;
        float OYSquareMiddleInit;
        float centreRotationAngle;
        float centreRotationRadius;
        float currentRotationAngle;
        bool mouseOnDuck;
        int score;
        float trajectoryAngle;
        float timeStartRound;
        bool shoot;
        bool escapedShooting;
        float speed;
        float initialSpeed;
        bool startedGame;
        bool showedBullet;
        float bulletX;
        float bulletY;
        int framesBullet = 0;
        int currentScore = 0;
        int highScore = 0;
        int framesFeathers;
        float mouseCoordX;
        float mouseCoordY;

        gfxc::TextRenderer* textRenderer;
        const glm::vec3 kTextColor = NormalizedRGB(165, 204, 196);
        gfxc::TextRenderer* textRenderer2;
    };
}   // namespace m1
