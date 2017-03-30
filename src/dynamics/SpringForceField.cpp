#include "./../../include/dynamics/SpringForceField.hpp"

SpringForceField::SpringForceField(const ParticlePtr p1, const ParticlePtr p2,
                                   float stiffness, float equilibriumLength,
                                   float damping) :
    m_p1(p1),
    m_p2(p2),
    m_stiffness(stiffness),
    m_equilibriumLength(equilibriumLength),
    m_damping(damping)
{}

void SpringForceField::do_addForce()
{
    //TODO: Implement a damped spring
    //Functions to use:
    //glm::length( vec3 ): Return the length of a vector
    //glm::normalize( vec3 ): Return the normalization of a vector
    //Particle::getVelocity(), Particle::getForce(), Particle::setForce()
    //
    //Nb:   Compute force ONLY IF the displacement length is above std::numeric_limits<float>::epsilon()
    //      Otherwise the computation is useless
    glm::vec3 f12;
    glm::vec3 f12damping;
    glm::vec3 distance12 = m_p2->getPosition() - m_p1->getPosition();

    if (length(distance12) < std::numeric_limits<float>::epsilon()) {
        return;
    }

    f12 = - m_stiffness * (length(distance12) - m_equilibriumLength) * normalize(distance12);
    f12damping = - m_damping * (m_p2->getVelocity() - m_p1->getVelocity()) * normalize(distance12) * normalize(distance12);
    //f12damping = - glm::vec3(1, 1, 1) * (m_p2->getVelocity() - m_p1->getVelocity()) * normalize(distance12) * normalize(distance12);

    glm::vec3 globalDamping = glm::vec3(1, 1, 1) * 1.0f;

    m_p2->setForce(m_p2->getForce() + (f12 + f12damping) - m_p2->getVelocity() * globalDamping);
    m_p1->setForce(m_p1->getForce() - (f12 + f12damping) - m_p1->getVelocity() * globalDamping);
}

ParticlePtr SpringForceField::getParticle1() const
{
    return m_p1;
}

ParticlePtr SpringForceField::getParticle2() const
{
    return m_p2;
}
