#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/Tema1/transform2D.h"
#include "triangle2D.h"
#include "rectangle2D.h"
#include "circle2D.h"

using namespace std;
using namespace m1;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}

// generate a random angle from [20-70] and
// [110-160] degrees
float Tema1::generateAngle() {
    float angles[110];
    int noAngles = 0;
    for (int i = 20; i <= 70; i++) {
        angles[noAngles] = i * M_PI / 180;
        noAngles++;
    }
    for (int i = 110; i <= 160; i++) {
        angles[noAngles] = i * M_PI / 180;
        noAngles++;
    }

    int anglePos = (int)rand() % noAngles;
    return angles[anglePos];
}

void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution(true);
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    // create the text render
    textRenderer = new gfxc::TextRenderer(window->props.selfDir, resolution.x, resolution.y);
    textRenderer->Load(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.ttf"), 30);

    textRenderer2 = new gfxc::TextRenderer(window->props.selfDir, resolution.x, resolution.y);
    textRenderer2->Load(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.ttf"), 20);

    glm::vec3 corner = glm::vec3(0, 0, 0);

    squareSide = 6;
    duckCentre = 5.3;

    scaleY = 0.6 * 40;
    scaleX = 0.6 * 40;

    // square - the coordinates as they are in the logic space
    OXstartSquare = 0 - (squareSide - 5.3) / 2 - 4.5 - 0.1;
    OYstartSquare = 0.5 - (squareSide - 4) / 2 - 2 - 0.1;
    OXendSquare = 5.3 + (squareSide - 5.3) / 2 - 4.5 - 0.1;
    OYendSquare = 4.5 + (squareSide - 4) / 2 - 2 - 0.1;

    // the middle of the square
    OXSquareMiddleInit = (OXstartSquare + OXendSquare) / 2;
    OYSquareMiddleInit = (OYstartSquare + OYendSquare) / 2;
    // polar coordinates of the square, with the beak in the middle (because the duck rotation is made
    // with the beak as the center)
    centreRotationAngle = atan2(OYSquareMiddleInit, OXSquareMiddleInit);
    centreRotationRadius = sqrt(OXSquareMiddleInit * OXSquareMiddleInit + OYSquareMiddleInit * OYSquareMiddleInit);

    currentRotationAngle = 0;
    escaped = false;
    doneEscaping = false;

    translateX = rand() % resolution.x;
    translateY = 0;
    trajectoryAngle = generateAngle();

    moveWingsAngle = 0;
    fullyMovedWings = false;
    ducksGenerated = 1;
    lives = 0;
    bullets = 0;
    mouseOnDuck = false;
    score = 0;
    timeStartRound = clock() / 1000;
    shoot = false;
    speed = 320;
    initialSpeed = 320;
    startedGame = false;
    showedBullet = false;
    framesFeathers = 60;
    mouseCoordX = 0;
    mouseCoordY = 0;

    // create the meshes for the design and gameplay
    Mesh* life = circle2D::CreateCircle("life", corner, resolution.x / 70, glm::vec3(0.6392f, 0.0196f, 0.0196f), true);
    AddMeshToList(life);

    Mesh* bullet = rectangle2D::CreateRectangle("bullet", corner, 32, 12, glm::vec3(0.0745f, 0.4f, 0.0627f), true);
    AddMeshToList(bullet);

    Mesh* scoreMargin = rectangle2D::CreateRectangle("scoreMargin", corner, 40, 415, glm::vec3(1, 1, 1), false);
    AddMeshToList(scoreMargin);

    Mesh* scoreFragment = rectangle2D::CreateRectangle("scoreFragment", corner, 30, 40, glm::vec3(0.1450f, 0.2274f, 0.5019f), true);
    AddMeshToList(scoreFragment);

    Mesh* square = rectangle2D::CreateRectangle("square", corner, squareSide, squareSide, glm::vec3(1, 1, 1), true);
    AddMeshToList(square);

    Mesh* mouseTarget1 = rectangle2D::CreateRectangle("mouseTarget1", corner, 45, 5, glm::vec3(0, 0, 0), true);
    AddMeshToList(mouseTarget1);

    Mesh* mouseTarget2 = rectangle2D::CreateRectangle("mouseTarget2", corner, 5, 45, glm::vec3(0, 0, 0), true);
    AddMeshToList(mouseTarget2);

    Mesh* sun = circle2D::CreateCircle("sun", corner, resolution.x / 23, glm::vec3(0.9921f, 0.7215f, 0.0745f), true);
    AddMeshToList(sun);

    Mesh* cloudDay = circle2D::CreateCircle("cloudDay", corner, resolution.x / 40, glm::vec3(0.9647f, 0.9647f, 0.9647f), true);
    AddMeshToList(cloudDay);

    Mesh* cloudNight = circle2D::CreateCircle("cloudNight", corner, resolution.x / 40, glm::vec3(0.7686f, 0.7882f, 0.8f), true);
    AddMeshToList(cloudNight);

    Mesh* moonVisible = circle2D::CreateCircle("moonVisible", corner, resolution.x / 25, glm::vec3(0.8705f, 0.8666f, 0.8196f), true);
    AddMeshToList(moonVisible);

    Mesh* moonInvisible = circle2D::CreateCircle("moonInvisible", corner, resolution.x / 20, glm::vec3(0, 0.0156f, 0.1098f), true);
    AddMeshToList(moonInvisible);
}

void Tema1::FrameStart()
{
    timeElapsed = clock() / 1000;
    // Clears the color buffer (using the previously set color) and depth buffer
    // the new color is the sky color, that changes according to the time
    if (timeElapsed % 30 < 10) {
        glClearColor(0.2588f, 0.6431f, 0.9607f, 1);
    }
    else if (timeElapsed % 30 >= 10 && timeElapsed % 30 < 20) {
        glClearColor(0.1960f, 0.3921f, 0.6588f, 1);
    }
    else {
        glClearColor(0, 0.0156f, 0.1098f, 1);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
}

void Tema1::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->GetResolution();
    timeElapsed = clock() / 1000;

    if (lives > 0) {
        // it it has been 5 second since the last duck has spawned, it has to escape
        if (timeElapsed - timeStartRound > 5 && escaped == false && shoot == false) {
            escaped = true;
            translateXEscaping = translateX;
        }

        // calculate translation coordinates if the duck is flying
        if (shoot == false && escaped == false) {
            translateX += deltaTimeSeconds * speed * cos(trajectoryAngle);
            translateY += deltaTimeSeconds * speed * sin(trajectoryAngle);
        }

        // if the duck is shot or has escaped, the X coordinates is the same
        // for all future frames
        if (shoot == true || escaped == true) {
            translateX = translateXEscaping;
        }

        // the Y coordinate for escaping
        if (escaped == true && shoot == false) {
            translateY += deltaTimeSeconds * speed;
        }
        // the Y coordinate for shooted
        else if (shoot == true) {
            translateY -= deltaTimeSeconds * speed;
        }

        // if the duck has reached the top or bottom side
        if (translateX >= resolution.x || translateX <= 0) {
            // the new trajectory angle
            trajectoryAngle = M_PI - trajectoryAngle;
            // if the duck is too far away from the screen, bring it back
            if (translateX > resolution.x) {
                translateX = resolution.x;
            }
            if (translateX < 0) {
                translateX = 0;
            }
        }

        // if the duck has reached the right side
        if (translateY >= resolution.y) {
            // the new trajectory angle
            trajectoryAngle = -trajectoryAngle;
            // if the duck has escaped and reaches the top, it has finished the move
            if (escaped) {
                doneEscaping = true;
                translateY = 0;
            }
            // if the duck is too far away from the screen, bring it back
            else if (translateY > resolution.y) {
                translateY = resolution.y;
            }
        }
        // if the duck has reached the left side
        if (translateY <= 0) {
            // the new trajectory angle
            trajectoryAngle = -trajectoryAngle;
            // if the duck is shot and reaches the bottom, it has finished the move
            if (shoot) {
                doneEscaping = true;
                translateY = 0;
            }
            // if the duck is too far away from the screen, bring it back
            else if (translateY < 0) {
                translateY = 0;
            }
        }

        // if the duck has finished the escaping/shooting move
        if (doneEscaping == true) {
            // if escaped and not shot
            if (escaped && !shoot) {
                lives--;
                if (currentScore > 0) {
                    currentScore -= 10;
                }
            }
            escaped = false;
            doneEscaping = false;
            shoot = false;
            bullets = 3;
            ducksGenerated++;
            // increase the speed after 5 ducks
            if (ducksGenerated % 5 == 0) {
                speed = initialSpeed + (int)(ducksGenerated / 5) * initialSpeed / 5;
            }
            translateX = rand() % resolution.x;
            trajectoryAngle = generateAngle();
            translateY = 0;
            timeStartRound = clock() / 1000;
        }

        modelMatrixForAll = glm::mat3(1);
        // if the duck is flying, rotate it
        if (escaped == false && shoot == false) {
            modelMatrixForAll *= transform2D::Translate(translateX, translateY);
            modelMatrixForAll *= transform2D::Rotate(trajectoryAngle);
            currentRotationAngle = trajectoryAngle;
        }
        // if the duck has escaped, rotate it upwards
        else if (escaped == true && shoot == false) {
            modelMatrixForAll *= transform2D::Translate(translateXEscaping, translateY);
            modelMatrixForAll *= transform2D::Rotate(M_PI / 2);
            currentRotationAngle = M_PI / 2;
        }
        // it the duck has been shot, rotate it downwards
        else if (shoot == true) {
            modelMatrixForAll *= transform2D::Translate(translateXEscaping, translateY);
            modelMatrixForAll *= transform2D::Rotate(-M_PI / 2);
            currentRotationAngle = -M_PI / 2;
        }

        scaleY = 0.6 * 40;
        scaleX = 0.6 * 40;

        displayGameElements(deltaTimeSeconds);
    }
    // display initial/game over screen
    else {
        modelMatrixForAll = glm::mat3(1);
        modelMatrixForAll *= transform2D::Translate(resolution.x / 2 + 90, resolution.y / 2);
        translateX = resolution.x / 2 + 90;
        translateY = resolution.y / 2;
        scaleX = 40;
        scaleY = 40;
        speed = initialSpeed;
    }

    // the grass
    float grassSide = resolution.x / static_cast<float>(12);
    Mesh* grass = rectangle2D::CreateRectangle("grass", glm::vec3(0, 0, 0), grassSide, 12 * grassSide, glm::vec3(0.1019f, 0.4313f, 0.1529f), true);
    AddMeshToList(grass);
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(0, 0);
    RenderMesh2D(meshes["grass"], shaders["VertexColor"], modelMatrix);

    // display the duck
    displayDuck(deltaTimeSeconds);

    // display the scenery elements
    displaySceneryElements();
}

// display the game elements
void Tema1::displayGameElements(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->GetResolution();
    // mouse target
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(mouseCoordX, mouseCoordY);
    modelMatrix *= transform2D::Translate(0, -20);
    RenderMesh2D(meshes["mouseTarget1"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(mouseCoordX, mouseCoordY);
    modelMatrix *= transform2D::Translate(-20, 0);
    RenderMesh2D(meshes["mouseTarget2"], shaders["VertexColor"], modelMatrix);

    // bullet animation
    if (showedBullet == true) {
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(bulletX, bulletY);
        RenderMesh2D(meshes["bullet"], shaders["VertexColor"], modelMatrix);
        framesBullet--;
        if (framesBullet == 0) {
            showedBullet = false;
        }
    }

    // display lives
    for (int i = 1; i <= lives; i++) {
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(50 * i, resolution.y - 30);
        RenderMesh2D(meshes["life"], shaders["VertexColor"], modelMatrix);
    }

    // display bullets
    for (int i = 1; i <= bullets; i++) {
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(47 * i, resolution.y - 100);
        RenderMesh2D(meshes["bullet"], shaders["VertexColor"], modelMatrix);
    }

    // display the score
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(resolution.x - 445, resolution.y - 55);
    RenderMesh2D(meshes["scoreMargin"], shaders["VertexColor"], modelMatrix);
    for (int i = 0; i < score; i++) {
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(resolution.x - 437.5 + i * 40, resolution.y - 50);
        RenderMesh2D(meshes["scoreFragment"], shaders["VertexColor"], modelMatrix);
    }
}

// display the scenery elements
void Tema1::displaySceneryElements()
{
    timeElapsed = clock() / 1000;
    glm::ivec2 resolution = window->GetResolution();

    // if the game is running
    if (lives > 0) {
        if (timeElapsed % 30 < 10) {
            // first cloud
            for (int i = 425; i <= 475; i += 50) {
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(i, resolution.y - 380);
                RenderMesh2D(meshes["cloudDay"], shaders["VertexColor"], modelMatrix);
            }

            for (int i = 400; i <= 500; i += 50) {
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(i, resolution.y - 400);
                RenderMesh2D(meshes["cloudDay"], shaders["VertexColor"], modelMatrix);
            }

            for (int i = 425; i <= 475; i += 50) {
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(i, resolution.y - 420);
                RenderMesh2D(meshes["cloudDay"], shaders["VertexColor"], modelMatrix);
            }

            // second cloud
            for (int i = 825; i <= 875; i += 50) {
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(i, resolution.y - 300);
                RenderMesh2D(meshes["cloudDay"], shaders["VertexColor"], modelMatrix);
            }

            for (int i = 800; i <= 900; i += 50) {
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(i, resolution.y - 320);
                RenderMesh2D(meshes["cloudDay"], shaders["VertexColor"], modelMatrix);
            }

            for (int i = 825; i <= 875; i += 50) {
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(i, resolution.y - 340);
                RenderMesh2D(meshes["cloudDay"], shaders["VertexColor"], modelMatrix);
            }

            // the sun
            modelMatrix = glm::mat3(1);
            modelMatrix *= transform2D::Translate(650, resolution.y - 250);
            RenderMesh2D(meshes["sun"], shaders["VertexColor"], modelMatrix);
        }
        else if (timeElapsed % 30 >= 10 && timeElapsed % 30 < 20) {
            // first cloud
            for (int i = 425; i <= 475; i += 50) {
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(i, resolution.y - 380);
                RenderMesh2D(meshes["cloudNight"], shaders["VertexColor"], modelMatrix);
            }

            for (int i = 400; i <= 500; i += 50) {
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(i, resolution.y - 400);
                RenderMesh2D(meshes["cloudNight"], shaders["VertexColor"], modelMatrix);
            }

            for (int i = 425; i <= 475; i += 50) {
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(i, resolution.y - 420);
                RenderMesh2D(meshes["cloudNight"], shaders["VertexColor"], modelMatrix);
            }

            // second cloud
            for (int i = 825; i <= 875; i += 50) {
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(i, resolution.y - 300);
                RenderMesh2D(meshes["cloudNight"], shaders["VertexColor"], modelMatrix);
            }

            for (int i = 800; i <= 900; i += 50) {
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(i, resolution.y - 320);
                RenderMesh2D(meshes["cloudNight"], shaders["VertexColor"], modelMatrix);
            }

            for (int i = 825; i <= 875; i += 50) {
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(i, resolution.y - 340);
                RenderMesh2D(meshes["cloudNight"], shaders["VertexColor"], modelMatrix);
            }
        }
        else {
            // the moon
            modelMatrix = glm::mat3(1);
            modelMatrix *= transform2D::Translate(710, resolution.y - 250);
            RenderMesh2D(meshes["moonInvisible"], shaders["VertexColor"], modelMatrix);

            modelMatrix = glm::mat3(1);
            modelMatrix *= transform2D::Translate(650, resolution.y - 250);
            RenderMesh2D(meshes["moonVisible"], shaders["VertexColor"], modelMatrix);
        }
    }
}

// display the duck
void Tema1::displayDuck(float deltaTimeSeconds)
{
    createDuckMeshes();

    // animation for duck death
    if (shoot == true && framesFeathers > 0) {
        modelMatrix = glm::mat3(1);
        modelMatrix *= modelMatrixForAll;
        modelMatrix *= transform2D::Scale(scaleX, scaleY);
        modelMatrix *= transform2D::Translate(2 - 4.5, 5 - 2);
        RenderMesh2D(meshes["duckFeather"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= modelMatrixForAll;
        modelMatrix *= transform2D::Scale(scaleX, scaleY);
        modelMatrix *= transform2D::Translate(2 - 4.5, 0 - 2);
        RenderMesh2D(meshes["duckFeather"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= modelMatrixForAll;
        modelMatrix *= transform2D::Scale(scaleX, scaleY);
        modelMatrix *= transform2D::Translate(1 - 4.5, 0.5 - 2);
        RenderMesh2D(meshes["duckFeather"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= modelMatrixForAll;
        modelMatrix *= transform2D::Scale(scaleX, scaleY);
        modelMatrix *= transform2D::Translate(1 - 4.5, 4.5 - 2);
        RenderMesh2D(meshes["duckFeather"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= modelMatrixForAll;
        modelMatrix *= transform2D::Scale(scaleX, scaleY);
        modelMatrix *= transform2D::Translate(-0.5 - 4.5, 4 - 2);
        RenderMesh2D(meshes["duckFeather"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= modelMatrixForAll;
        modelMatrix *= transform2D::Scale(scaleX, scaleY);
        modelMatrix *= transform2D::Translate(-0.5 - 4.5, 1 - 2);
        RenderMesh2D(meshes["duckFeather"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= modelMatrixForAll;
        modelMatrix *= transform2D::Scale(scaleX, scaleY);
        modelMatrix *= transform2D::Translate(-0.5 - 4.5, 2.5 - 2);
        RenderMesh2D(meshes["duckFeather"], shaders["VertexColor"], modelMatrix);

        framesFeathers--;
    }

    // duck eye black
    modelMatrix = glm::mat3(1);
    modelMatrix *= modelMatrixForAll;
    modelMatrix *= transform2D::Scale(scaleX, scaleY);
    modelMatrix *= transform2D::Translate(-0.17, 0.8);
    RenderMesh2D(meshes["duckEyeBlack"], shaders["VertexColor"], modelMatrix);

    // duck eye white
    modelMatrix = glm::mat3(1);
    modelMatrix *= modelMatrixForAll;
    modelMatrix *= transform2D::Scale(scaleX, scaleY);
    modelMatrix *= transform2D::Translate(-0.17, 0.8);
    RenderMesh2D(meshes["duckEyeWhite"], shaders["VertexColor"], modelMatrix);

    // duck head
    modelMatrix = glm::mat3(1);
    modelMatrix *= modelMatrixForAll;
    modelMatrix *= transform2D::Scale(scaleX, scaleY);
    modelMatrix *= transform2D::Translate(-0.7, 0.5);
    RenderMesh2D(meshes["duckHead"], shaders["VertexColor"], modelMatrix);

    // duck beak
    modelMatrix = glm::mat3(1);
    modelMatrix *= modelMatrixForAll;
    modelMatrix *= transform2D::Scale(scaleX, scaleY);
    modelMatrix *= transform2D::Translate(0, 0);
    RenderMesh2D(meshes["duckBeak"], shaders["VertexColor"], modelMatrix);

    // duck body
    modelMatrix = glm::mat3(1);
    modelMatrix *= modelMatrixForAll;
    modelMatrix *= transform2D::Scale(scaleX, scaleY);
    modelMatrix *= transform2D::Translate(-4.5, -1);
    RenderMesh2D(meshes["duckBody"], shaders["VertexColor"], modelMatrix);

    // calculate wings move
    if (!fullyMovedWings) {
        moveWingsAngle += deltaTimeSeconds * 0.8 * speed / initialSpeed;
    }
    if (fullyMovedWings) {
        moveWingsAngle -= deltaTimeSeconds * 0.8 * speed / initialSpeed;
    }
    if (moveWingsAngle > 0.785398) {
        fullyMovedWings = true;
    }

    if (moveWingsAngle < 0) {
        fullyMovedWings = false;
    }

    // duck upper wing
    modelMatrix = glm::mat3(1);
    modelMatrix *= modelMatrixForAll;
    modelMatrix *= transform2D::Scale(scaleX, scaleY);
    // rotate 90 degrees so it it perpendicular on the body
    modelMatrix *= transform2D::Translate(-2, 1);
    modelMatrix *= transform2D::Rotate(1.5708 + moveWingsAngle);
    modelMatrix *= transform2D::Translate(2, -1);
    modelMatrix *= transform2D::Translate(-2, 1);
    RenderMesh2D(meshes["duckUpperWing"], shaders["VertexColor"], modelMatrix);

    // rata down wing
    modelMatrix = glm::mat3(1);
    modelMatrix *= modelMatrixForAll;
    modelMatrix *= transform2D::Scale(scaleX, scaleY);
    // rotate 90 degrees so it it perpendicular on the body
    modelMatrix *= transform2D::Translate(-2, 0);
    modelMatrix *= transform2D::Rotate(-1.5708 - moveWingsAngle);
    modelMatrix *= transform2D::Translate(2, 0);
    modelMatrix *= transform2D::Translate(-2, -1.5);
    RenderMesh2D(meshes["duckUpperWing"], shaders["VertexColor"], modelMatrix);
}

// create the duck meshes
void Tema1::createDuckMeshes()
{
    glm::vec3 corner = glm::vec3(0, 0, 0);
    float duckBodySide = 3;
    float duckBodyWidth = 4;

    // meshes for dead duck
    if (shoot == true && framesFeathers > 0) {
        Mesh* duckBody = triangle2D::CreateTriangle("duckBody", corner, duckBodySide, duckBodyWidth, glm::vec3(0.7490f, 0.0431f, 0.0431f), true);
        AddMeshToList(duckBody);

        Mesh* duckUpperWing = triangle2D::CreateTriangle("duckUpperWing", corner, duckBodySide / 2, duckBodyWidth / 2, glm::vec3(0.7490f, 0.0431f, 0.0431f), true);
        AddMeshToList(duckUpperWing);

        Mesh* duckDownWing = triangle2D::CreateTriangle("duckDownWing", corner, duckBodySide / 2, duckBodyWidth / 2, glm::vec3(0.7490f, 0.0431f, 0.0431f), true);
        AddMeshToList(duckDownWing);

        Mesh* duckBeak = triangle2D::CreateTriangle("duckBeak", corner, duckBodySide / 3, duckBodyWidth / 3.6, glm::vec3(0.7490f, 0.0431f, 0.0431f), true);
        AddMeshToList(duckBeak);

        Mesh* duckHead = circle2D::CreateCircle("duckHead", corner, duckBodySide / 2.6, glm::vec3(0.7490f, 0.0431f, 0.0431f), true);
        AddMeshToList(duckHead);
    }
    // meshes for alive duck
    else {
        Mesh* duckBody = triangle2D::CreateTriangle("duckBody", corner, duckBodySide, duckBodyWidth, glm::vec3(0.2196f, 0.1529f, 0.0549f), true);
        AddMeshToList(duckBody);

        Mesh* duckUpperWing = triangle2D::CreateTriangle("duckUpperWing", corner, duckBodySide / 2, duckBodyWidth / 2, glm::vec3(0.2196f, 0.1529f, 0.0549f), true);
        AddMeshToList(duckUpperWing);

        Mesh* duckDownWing = triangle2D::CreateTriangle("duckDownWing", corner, duckBodySide / 2, duckBodyWidth / 2, glm::vec3(0.2196f, 0.1529f, 0.0549f), true);
        AddMeshToList(duckDownWing);

        Mesh* duckBeak = triangle2D::CreateTriangle("duckBeak", corner, duckBodySide / 3, duckBodyWidth / 3.6, glm::vec3(0.8901f, 0.8666f, 0.1686f), true);
        AddMeshToList(duckBeak);

        Mesh* duckHead = circle2D::CreateCircle("duckHead", corner, duckBodySide / 2.6, glm::vec3(0.0352f, 0.2196f, 0.0823f), true);
        AddMeshToList(duckHead);
    }

    Mesh* duckFeather = triangle2D::CreateTriangle("duckFeather", corner, duckBodySide / 16, duckBodySide / 16, glm::vec3(1, 1, 1), true);
    AddMeshToList(duckFeather);

    Mesh* duckEyeWhite = circle2D::CreateCircle("duckEyeWhite", corner, duckBodySide / 12, glm::vec3(1, 1, 1), true);
    AddMeshToList(duckEyeWhite);

    Mesh* duckEyeBlack = circle2D::CreateCircle("duckEyeBlack", corner, duckBodySide / 20, glm::vec3(0, 0, 0), true);
    AddMeshToList(duckEyeBlack);
}

void Tema1::FrameEnd()
{
    glm::ivec2 resolution = window->GetResolution();
    // show the text for the initial/game over screen
    if (lives == 0) {
        textRenderer->RenderText("PRESS SPACE TO START", 470, 450, 1.0f, kTextColor);
        if (startedGame == true) {
            textRenderer->RenderText("GAME OVER", 570, 200, 1.0f, kTextColor);
            std::stringstream str2;
            str2 << "HIGHSCORE : " << highScore;
            textRenderer2->RenderText(str2.str(), 1050, 70, 1.0f, kTextColor);
        }
    }
    // show the score during the game
    else {
        std::stringstream str1;
        str1 << "SCORE : " << currentScore;
        textRenderer2->RenderText(str1.str(), 880, 70, 1.0f, kTextColor);

        std::stringstream str2;
        str2 << "HIGHSCORE : " << highScore;
        textRenderer2->RenderText(str2.str(), 1050, 70, 1.0f, kTextColor);
    }
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */
void Tema1::OnInputUpdate(float deltaTime, int mods)
{
}

void Tema1::OnKeyPress(int key, int mods)
{
    glm::ivec2 resolution = window->GetResolution();
    // if we are in the initial/game over screen,
    // press space to start the game
    if (key == GLFW_KEY_SPACE && lives == 0) {
        lives = 3;
        bullets = 3;
        score = 0;
        ducksGenerated = 1;
        speed = initialSpeed;
        translateX = rand() % resolution.x;
        trajectoryAngle = generateAngle();
        translateY = 0;
        timeStartRound = clock() / 1000;
        startedGame = true;
    }
}

void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    glm::ivec2 resolution = window->GetResolution();

    float actualX = mouseX;
    float actualY = resolution.y - mouseY;

    // calculate the current centre of the square in the logic space,
    // according to the initial rotation angle relative to the beak and
    // the current rotation angle of the duck (and of the beak)
    float OXSquareMiddleCurrent = centreRotationRadius * cos(currentRotationAngle + centreRotationAngle);
    float OYSquareMiddleCurrent = centreRotationRadius * sin(currentRotationAngle + centreRotationAngle);

    // calculate the square's vertices (the actual coordinates)
    float x1 = (OXSquareMiddleCurrent - 3) * scaleX + translateX;
    float x2 = (OXSquareMiddleCurrent + 3) * scaleX + translateX;
    float y1 = (OYSquareMiddleCurrent - 3) * scaleY + translateY;
    float y2 = (OYSquareMiddleCurrent + 3) * scaleX + translateY;

    // check if the mouse is located inside the square
    if (x1 <= actualX && actualX <= x2 && y1 <= actualY && actualY <= y2) {
        mouseOnDuck = true;
    }
    else {
        mouseOnDuck = false;
    }

    mouseCoordX = actualX;
    mouseCoordY = actualY;
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    glm::ivec2 resolution = window->GetResolution();
    // if the left click is pressed
    if (button == GLFW_MOUSE_BUTTON_2) {
        if (lives > 0) {
            // initialise variables used for the bullet animation
            if (bullets > 0) {
                framesBullet = 10;
                showedBullet = true;
                bulletX = mouseX;
                bulletY = resolution.y - mouseY;
            }
            // if the duck isn't hit
            if (mouseOnDuck == false && shoot == false) {
                bullets--;
                // if out of bullets, the duck escapes
                if (bullets == 0) {
                    if (escaped == false) {
                        escaped = true;
                        translateXEscaping = translateX;
                    }
                }
            }
            // if the duck is shot
            else if (mouseOnDuck == true && bullets > 0 && shoot == false) {
                bullets--;
                currentScore += 10;
                highScore = max(currentScore, highScore);
                // increase score
                if (score < 10) {
                    score++;
                }
                shoot = true;
                framesFeathers = 60;
                translateXEscaping = translateX;
            }
        }
    }

}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema1::OnWindowResize(int width, int height)
{
}
