#include <memory>
#include <sstream>

#include "utils/common.h"
#include "snow/slab.h"


void launchSimGenSlab(int argc, char const **argv) {

    // Simulation consts

    double density = 400; // kg/m3
    double particleSize = .0072;
    double gridSize = particleSize * 2;
    auto simulationSize = glm::dvec3(1);

    // Init simulation

    solver.reset(new SnowSolver(gridSize, simulationSize * (1 / gridSize)));

    if (argc > 2) solver->delta_t = atof(argv[2]);
    if (argc > 3) solver->beta = atof(argv[3]);

    // Particles

    genSnowSlab(glm::dvec3(0.2, 0.45, 0.7), glm::dvec3(0.8, 0.55, 0.9), density, particleSize);

    std::cout << "#particles=" << solver->particleNodes.size() << std::endl;

    // Output

    std::ostringstream filename;
    filename << "frame-0" SOLVER_STATE_EXT;
    solver->saveState(filename.str());

    std::cout << "Frame 0 written to: " << filename.str() << std::endl;

}
