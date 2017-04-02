#ifndef DYNAMIC_MESH_HPP
#define DYNAMIC_MESH_HPP

#include "Particle.hpp"

#include <iostream>
#include <memory>
#include <glm/glm.hpp>

/**@brief Represent a DynamicMesh as a moving ball.
 *
 * This class is used to model DynamicMeshs in a dynamic system.
 * Such a DynamicMesh is seen as a ball with a mass, a velocity and
 * a position. This ball is affected by forces that will change
 * both its position and its velocity. This ball can be fixed,
 * making its position constant and its velocity null.
 */
class DynamicMesh : public Particle
{
    public :
    DynamicMesh(const glm::vec3& position, const glm::vec3& velocity,
            const float& mass, const float& radius, const float & angle);
    virtual ~DynamicMesh();
};

typedef std::shared_ptr<DynamicMesh> DynamicMeshPtr;

/**
 * \brief output stream operator, as non-member
 */
std::ostream& operator<<(std::ostream& os, const DynamicMeshPtr& p);

#endif
