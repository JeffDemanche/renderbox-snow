#ifdef USE_RENDERBOX

#include <memory>

#define RENDERBOX_USE_OPENGL
#define RENDERBOX_USE_GLFW

#include "renderbox.h"

#include "scenes/scene0.h"
#include "snow/sphere.h"
#include "utils/renderer.h"


static unsigned ticksPerFrame = 1;


static void demoRenderLoopUpdate(unsigned int frame) {

    for (auto tick = 0; tick < ticksPerFrame; tick++) {
        solver->update();
    }

}

void launchDemoSnowball(int argc, char const **argv) {

    // Simulation

    double density = 400; // kg/m3
    double particleSize = .01;
    double gridSize = particleSize * 2;
    auto simulationSize = glm::dvec3(1);

    solver.reset(new SnowSolver(gridSize, simulationSize * (1 / gridSize)));
    solver->delta_t = 5e-4;

    genSnowSphere(glm::dvec3(0.5, 0.5, 0.5), 0.06, density, particleSize);

    solver->handleNodeCollisionVelocityUpdate = handleNodeCollisionVelocityUpdate;

    // Rendering

    initRenderer();

    renderColliders();

    startRenderLoop(demoRenderLoopUpdate);

}

#endif //USE_RENDERBOX
