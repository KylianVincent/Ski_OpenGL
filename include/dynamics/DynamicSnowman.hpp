#ifndef DYNAMIC_SNOWMAN_HPP
#define DYNAMIC_SNOWMAN_HPP

#include "Particle.hpp"

#include <iostream>
#include <memory>
#include <glm/glm.hpp>

/**@brief Represent a DynamicSnowman as a moving ball.
 *
 * This class is used to model DynamicSnowmans in a dynamic system.
 * Such a DynamicSnowman is seen as a ball with a mass, a velocity and
 * a position. This ball is affected by forces that will change
 * both its position and its velocity. This ball can be fixed,
 * making its position constant and its velocity null.
 */
class DynamicSnowman : public Particle
{
    public :
    DynamicSnowman(const glm::vec3& position, const glm::vec3& velocity,
            const float& mass, const float& radius);
    virtual ~DynamicSnowman();
};

typedef std::shared_ptr<DynamicSnowman> DynamicSnowmanPtr;

/**
 * \brief output stream operator, as non-member
 */
std::ostream& operator<<(std::ostream& os, const DynamicSnowmanPtr& p);

#endif