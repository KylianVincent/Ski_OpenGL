#include "./../../include/dynamics/DampingForceField.hpp"

DampingForceField::DampingForceField(const std::vector<ParticlePtr> particles, const float damping, const float perpendicularDamping)
{
    m_particles = particles;
    m_damping = damping;
    m_perpendicularDamping = perpendicularDamping;
}

DampingForceField::~DampingForceField()
{
}

void DampingForceField::do_addForce()
{
  float dotProduct = 1;
  glm::vec3 planarVelocity;
  for (ParticlePtr p : m_particles) {
    planarVelocity = glm::vec3(p->getVelocity()[0], p->getVelocity()[1], 0);
    if (glm::length(planarVelocity) < 0.5) {
      dotProduct = 1;
    } else {
      dotProduct = std::abs(glm::dot(glm::normalize(glm::vec3(p->getVelocity()[0], p->getVelocity()[1], 0)), glm::normalize(glm::vec3(cos(p->getAngle()), sin(p->getAngle()), 0))));
    }
    //std::cout << dotProduct << std::endl;
    //std::cout << "glm::vec3(" << p->getVelocity()[0] << ", " << p->getVelocity()[1] << ", 0)), glm::normalize(glm::vec3(" << cos(p->getAngle()) << ", " << sin(p->getAngle()) << ", 0))))" << std::endl;
    p->incrForce(-m_damping*p->getVelocity() -m_perpendicularDamping*(1-dotProduct)*p->getVelocity());
  }
}

const std::vector<ParticlePtr> DampingForceField::getParticles()
{
    return m_particles;
}

void DampingForceField::setParticles(const std::vector<ParticlePtr>& particles)
{
    m_particles = particles;
}

const float& DampingForceField::getDamping()
{
    return m_damping;
}

void DampingForceField::setDamping(const float& damping)
{
    m_damping = damping;
}
