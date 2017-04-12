#include "./../../include/dynamics/DynamicSnowman.hpp"

    DynamicSnowman::DynamicSnowman(const glm::vec3& position, const glm::vec3& velocity,
            const float& mass, const float& radius, const float& angle,
            const float& xAngle, const float& yAngle)
            : Particle(position, velocity, mass, radius, angle, xAngle, yAngle) {
        m_leftArmAngle = 1.5f;
        m_rightArmAngle = 1.5f;

            }
    DynamicSnowman::~DynamicSnowman()
    {}

float DynamicSnowman::getRightArmAngle() const {
    return m_rightArmAngle;
}

float DynamicSnowman::getLeftArmAngle() const {
    return m_leftArmAngle;
}

void DynamicSnowman::setRightArmAngle(float m_rightArmAngle) {
    DynamicSnowman::m_rightArmAngle = m_rightArmAngle;
}

void DynamicSnowman::setLeftArmAngle(float m_leftArmAngle) {
    DynamicSnowman::m_leftArmAngle = m_leftArmAngle;
}


void DynamicSnowman::doRightAction(float &dt) {
    float newArmAngle = (float) (m_rightArmAngle + dt* 1.0);
    m_rightArmAngle = glm::clamp(newArmAngle, 1.5f, 2.3f);
//    std::cout << "D : Je baisse le bras droit ! : " << newArmAngle  << std::endl;
}

void DynamicSnowman::undoRightAction(float &dt) {
    float newArmAngle = (float) (m_rightArmAngle - dt* 1.0);
    m_rightArmAngle = glm::clamp(newArmAngle, 1.5f, 2.3f);
//    std::cout << "D : Je lève le bras droit ! : " << newArmAngle  << std::endl;
}

void DynamicSnowman::doLeftAction(float &dt) {
    float newArmAngle = (float) (m_leftArmAngle + dt* 1.0);
    m_leftArmAngle = glm::clamp(newArmAngle, 1.5f, 2.3f);
//    std::cout << "R : Je baisse le bras gauche ! : " << newArmAngle << std::endl;
}


void DynamicSnowman::undoLeftAction(float &dt) {
    float newArmAngle = (float) (m_leftArmAngle - dt* 1.0);
    m_leftArmAngle = glm::clamp(newArmAngle, 1.5f, 2.3f);
//    std::cout << "R : Je lève le bras gauche ! : " << newArmAngle << std::endl;
}

std::ostream& operator<<(std::ostream& os, const DynamicSnowmanPtr& p)
{
    const glm::vec3& x = p->getPosition();
    const glm::vec3& v = p->getVelocity();

    os << "pos (" << x[0] << ", " << x[1] << ", " << x[2] << ")";
    os << " ; ";
    os << "vel (" << v[0] << ", " << v[1] << ", " << v[2] << ")";
    os << " ; ";
    os << "angle :" << p->getAngle();
    os << " ; ";
    os << "arm angles (l, r) : (" << p->getRightArmAngle() << ", " << p->getLeftArmAngle() << ")";

    return os;
}
