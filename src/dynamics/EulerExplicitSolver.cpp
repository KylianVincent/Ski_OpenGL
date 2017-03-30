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
            //Functions to use:
            //Particle::getPosition(), Particle::getVelocity(), Particle::getMass(), Particle::getForce()
            //Particle::setPosition(), Particle::setVelocity()
            newVelocity = p->getVelocity() + (dt/(p->getMass()) * p->getForce());
            newPosition = p->getPosition() + dt*newVelocity;
            p->setVelocity(newVelocity);
            p->setPosition(newPosition);
        }
    }
}
