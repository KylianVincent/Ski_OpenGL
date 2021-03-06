#include "./../../include/dynamics/Particle.hpp"

void Particle::setRadius(const float &radius)
{
    m_radius = radius;
}


bool Particle::isFixed() const
{
    return m_isFixed;
}

void Particle::setFixed(bool isFixed)
{
    m_isFixed = isFixed;
}

Particle::Particle(const glm::vec3 &position, const glm::vec3 &velocity,
                   const float &mass, const float &radius, const float & angle,
                  const float & xAngle, const float & yAngle)
    : m_initialPosition( position ), m_initialVelocity( velocity ),
    m_position(position),
    m_velocity(velocity),
    m_force(glm::vec3(0.0,0.0,0.0)),
    m_mass(mass),
    m_radius(radius), m_isFixed( false ),
    m_angle(angle),
    m_xAngle(xAngle),
    m_yAngle(yAngle)
{}

Particle::~Particle()
{}


const glm::vec3 & Particle::getPosition() const
{
    return m_position;
}

const float &Particle::getAngle() const {
    return m_angle;
}

const float &Particle::getXAngle() const {
    return m_xAngle;
}

const float &Particle::getYAngle() const {
    return m_yAngle;
}

const glm::vec3 & Particle::getVelocity() const
{
    return m_velocity;
}

const glm::vec3 & Particle::getForce() const
{
    return m_force;
}

float Particle::getMass() const
{
    return m_mass;
}

float Particle::getRadius() const
{
    return m_radius;
}

void Particle::setPosition(const glm::vec3 &pos)
{
    m_position = pos;
}

void Particle::setAngle(const float &ang) {
    m_angle = ang;
}

void Particle::setXAngle(const float &ang) {
    m_xAngle = ang;
}

void Particle::setYAngle(const float &ang) {
    m_yAngle = ang;
}

void Particle::setVelocity(const glm::vec3 &vel)
{
    m_velocity = vel;
}

void Particle::setForce(const glm::vec3 &force)
{
    m_force = force;
}

void Particle::incrPosition(const glm::vec3 &pos)
{
    m_position += pos;
}

void Particle::incrVelocity(const glm::vec3 &vel)
{
    m_velocity += vel;
}

void Particle::incrForce(const glm::vec3& force)
{
    m_force +=force;
}

void Particle::restart()
{
    m_position = m_initialPosition;
    m_velocity = m_initialVelocity;
}

void Particle::doRightAction(float &dt) {}

void Particle::doLeftAction(float &dt) {}

void Particle::undoLeftAction(float &dt) {}

void Particle::undoRightAction(float &dt) {}


std::ostream& operator<<(std::ostream& os, const ParticlePtr& p)
{
    const glm::vec3& x = p->getPosition();
    const glm::vec3& v = p->getVelocity();

    os << "pos (" << x[0] << ", " << x[1] << ", " << x[2] << ")";
    os << " ; ";
    os << "vel (" << v[0] << ", " << v[1] << ", " << v[2] << ")";
    os << " ; ";
    os << "angle :" << p->getAngle();

    return os;
}
