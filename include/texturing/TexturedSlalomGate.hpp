#ifndef TEXTURED_SLALOM_GATE_HPP
#define TEXTURED_SLALOM_GATE_HPP

#include "./../HierarchicalRenderable.hpp"
#include "./../lighting/LightedCylinderRenderable.hpp"
#include "./../lighting/Material.hpp"
#include "./../dynamics/Particle.hpp"
#include "./../dynamics/DynamicSystem.hpp"
#include <vector>
#include <glm/glm.hpp>

class TexturedSlalomGate : public LightedCylinderRenderable
{
public:
    TexturedSlalomGate(ShaderProgramPtr program, const MaterialPtr& mat, ParticlePtr particle);
    ~TexturedSlalomGate();
private:
    ParticlePtr m_particle;
};

typedef std::shared_ptr<TexturedSlalomGate> TexturedSlalomGatePtr;

TexturedSlalomGatePtr createGate(ShaderProgramPtr program, const std::string& gateTextureFilename, const MaterialPtr& mat, DynamicSystemPtr system, const ParticlePtr& particle);


#endif
