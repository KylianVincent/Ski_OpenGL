#include "./../../include/dynamics/EulerExplicitSolver.hpp"

EulerExplicitSolver::EulerExplicitSolver()
{

}

EulerExplicitSolver::~EulerExplicitSolver()
{

}

void EulerExplicitSolver::do_solve(const float& dt, std::vector<ParticlePtr>& particles)
{
    glm::vec3 newPosition;
    glm::vec3 newVelocity;
    for (ParticlePtr p : particles) {
        if (!p->isFixed()) {
            newVelocity = p->getVelocity() + (dt/(p->getMass()) * p->getForce());
            newPosition = p->getPosition() + dt*newVelocity;
            p->setVelocity(newVelocity);
            p->setPosition(newPosition);
//            p->setAngle((float) acos(glm::dot(glm::normalize(newPosition[1]-newPosition[0]), glm::normalize(newPosition[2]-newPosition[0]))));
        }
    }
}
