#include <cmath>
#include <iostream>
#include <map>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include "../../include/gl_helper.hpp"
#include "../../include/dynamics_rendering/DynamicSystemRenderable.hpp"
#include "../../include/Viewer.hpp"

DynamicSystemRenderable::DynamicSystemRenderable(DynamicSystemPtr system) :
    HierarchicalRenderable(nullptr), m_lastUpdateTime(0)
{
    m_system = system;
}

DynamicSystemRenderable::~DynamicSystemRenderable()
{}

void DynamicSystemRenderable::do_draw()
{}

void DynamicSystemRenderable::do_animate(float time)
{
    if (time - m_lastUpdateTime >= m_system->getDt() || time <= m_lastUpdateTime) {
        m_system->computeSimulationStep();
        //Dynamic system step
        m_lastUpdateTime = time;
    }
}

void DynamicSystemRenderable::setDynamicSystem(const DynamicSystemPtr &system)
{
    m_system = system;
}

void DynamicSystemRenderable::do_keyPressedEvent(sf::Event &e)
{
    if (e.key.code == sf::Keyboard::A) {    //Toggle collision detection
        m_system->setCollisionsDetection(!m_system->getCollisionDetection());
    } else if (e.key.code == sf::Keyboard::T) {   //Tilt particles
        srand(time(0));
        // "tilt" all particles randomly around their position
        for (const ParticlePtr& p : m_system->getParticles()) {
            glm::vec3 pos = p->getPosition();
            // tilt all positions
            pos += glm::ballRand(1.0f);
            p->setPosition(pos);
        }
    } else if (e.key.code == sf::Keyboard::F5) {    //Reset the simulation
        for (HierarchicalRenderablePtr c : getChildren()) {
          c->keyPressedEvent(e);
        }
        for (const ParticlePtr& p : m_system->getParticles()) {
            p->restart();
        }
        m_lastUpdateTime = 0;
    } else {    //Propagate events to the children
        for (HierarchicalRenderablePtr c : getChildren()) {
            c->keyPressedEvent(e);
        }
    }
}

void DynamicSystemRenderable::do_keyReleasedEvent(sf::Event& e)
{
    //Propagate events to the children
    for (HierarchicalRenderablePtr c : getChildren()) {
        c->keyReleasedEvent(e);
    }
}
