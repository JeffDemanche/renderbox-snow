#ifndef SNOW_RENDERER_H
#define SNOW_RENDERER_H

#ifndef USE_RENDERBOX
#error "RenderBox is required for viz"
#endif //USE_RENDERBOX


#define RENDERBOX_USE_OPENGL
#define RENDERBOX_USE_GLFW

#include "renderbox.h"

#include "common.h"


static std::unique_ptr<renderbox::OpenGLRenderer> renderer;
static std::unique_ptr<renderbox::GLFWOpenGLRenderTarget> renderTarget;

static std::shared_ptr<renderbox::Scene> scene;

static std::shared_ptr<renderbox::PerspectiveCamera> camera;
static std::shared_ptr<renderbox::Object> cameraRig;

static std::shared_ptr<renderbox::Object> colliders;
static std::shared_ptr<renderbox::Material> colliderMaterial;

static std::shared_ptr<renderbox::Object> particles;

static std::shared_ptr<renderbox::Geometry> snowParticleGeometry;
static std::shared_ptr<renderbox::Material> snowParticleMaterial;

static GLFWwindow *window;

static int keyMods = 0;

static double cameraDistance = 2;
static double cameraAngle[] = {0.0, 90.0};


static void windowSizeCallback(GLFWwindow *window, int width, int height) {
    camera->setPerspective(glm::radians(45.0f),
                           (float) renderTarget->getWindowWidth() / (float) renderTarget->getWindowHeight());
}

static void keyCallback(GLFWwindow *window, int key, int scanCode, int action, int mods) {
    keyMods = mods;
}

static void scrollCallback(GLFWwindow *window, double deltaX, double deltaY) {
    auto cameraDirection = camera->getRay(renderbox::vec2()).getDirection();
    auto forward = glm::normalize(glm::dvec3(cameraDirection.x, cameraDirection.y, 0));
    auto right = glm::normalize(glm::dvec3(cameraDirection.y, -cameraDirection.x, 0));
    if (keyMods & GLFW_MOD_ALT) {
        cameraAngle[1] += -deltaY;
        return;
    }
    cameraRig->translate((forward * deltaY - right * deltaX) * cameraDistance * 0.01);
}

static void zoomCallback(GLFWwindow *window, double magnification) {
    cameraDistance /= (1 + magnification);
    camera->setTranslation(glm::vec3(0, 0, cameraDistance));
}

static void rotateCallback(GLFWwindow *window, double rotation) {
    cameraAngle[0] += -rotation;
}

static void updateVizParticlePositions() {

    auto numParticles = solver->particleNodes.size();
    for (auto i = 0; i < numParticles; i++) {
        particles->children[i]->setTranslation(solver->particleNodes[i].position);
    }

}

static void initRenderer() {

    auto simulationSize = solver->h * glm::dvec3(solver->size);
    double particleSize = 0.0072;

    // Renderer

    renderer.reset(new renderbox::OpenGLRenderer());
    renderTarget.reset(new renderbox::GLFWOpenGLRenderTarget());

    window = renderTarget->getWindow();

    // Callbacks

    glfwSetWindowSizeCallback(window, windowSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetZoomCallback(window, zoomCallback);
    glfwSetRotateCallback(window, rotateCallback);

    // Scene

    scene = std::make_shared<renderbox::Scene>();

    // Light

    auto light = std::make_shared<renderbox::PointLight>(glm::dvec3(100));
    light->setTranslation({simulationSize.x / 2, simulationSize.y / 2, 20});
    scene->addChild(light);

    // Camera

    cameraRig = std::make_shared<renderbox::Object>();
    cameraRig->setTranslation({simulationSize.x / 2, simulationSize.y / 2, 0.1});

    camera = std::make_shared<renderbox::PerspectiveCamera>(
            glm::radians(45.0), (double) renderTarget->getWindowWidth() / (double) renderTarget->getWindowHeight());
    cameraRig->addChild(camera);
    camera->setTranslation(glm::dvec3(0, 0, cameraDistance));

    // Colliders

    colliders = std::make_shared<renderbox::Object>();
    scene->addChild(colliders);

    colliderMaterial = std::make_shared<renderbox::MeshLambertMaterial>(glm::dvec3(0.2));

    // Particles

    particles = std::make_shared<renderbox::Object>();
    scene->addChild(particles);

    snowParticleGeometry = std::make_shared<renderbox::BoxGeometry>(particleSize, particleSize, particleSize);
    snowParticleMaterial = std::make_shared<renderbox::MeshLambertMaterial>(renderbox::vec3(1, 1, 1));

    auto numParticles = solver->particleNodes.size();
    for (auto i = 0; i < numParticles; i++) {
        particles->addChild(std::make_shared<renderbox::Object>(snowParticleGeometry, snowParticleMaterial));
    }

    updateVizParticlePositions();

}

static void startRenderLoop(void (*update)(unsigned int)) {

    unsigned int frame = 0;
    auto timeLast = std::chrono::system_clock::now();
    while (!glfwWindowShouldClose(window)) {

        // Manually cap frame rate
        auto timeNow = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::nanoseconds>(timeNow - timeLast);
        if (ms.count() < 16666666.67) {
            continue;
        }
        timeLast = timeNow;

        if (cameraAngle[1] < 10) cameraAngle[1] = 10;
        else if (cameraAngle[1] > 85) cameraAngle[1] = 85;
        cameraRig->clearRotation();
        cameraRig->rotate({1, 0, 0}, glm::radians(cameraAngle[1]));
        cameraRig->rotate({0, 0, 1}, glm::radians(cameraAngle[0]));

        renderer->render(scene.get(), camera.get(), renderTarget.get());

        glfwSwapBuffers(window);

        glfwPollEvents();

        // Update

        update(++frame);

        updateVizParticlePositions();

    }

    glfwTerminate();

}

#endif //SNOW_RENDERER_H
