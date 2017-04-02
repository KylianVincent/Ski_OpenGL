#include "./../../include/dynamics/DynamicSnowman.hpp"

    DynamicSnowman::DynamicSnowman(const glm::vec3& position, const glm::vec3& velocity,
            const float& mass, const float& radius, const float& angle)
            : Particle(position, velocity, mass, radius, angle) {

            }
    DynamicSnowman::~DynamicSnowman()
    {}

std::ostream& operator<<(std::ostream& os, const DynamicSnowmanPtr& p)
{
    const glm::vec3& x = p->getPosition();
    const glm::vec3& v = p->getVelocity();

    os << "pos (" << x[0] << ", " << x[1] << ", " << x[2] << ")";
    os << " ; ";
    os << "vel (" << v[0] << ", " << v[1] << ", " << v[2] << ")";

    return os;
}
