#ifndef TEXTURED_SLALOM_GATE_HPP
#define TEXTURED_SLALOM_GATE_HPP

#include "./../HierarchicalRenderable.hpp"
#include "../../include/lighting/LightedCylinderRenderable.hpp"
#include "./../lighting/Material.hpp"
#include <vector>
#include <glm/glm.hpp>

class TexturedSlalomGate : public LightedCylinderRenderable
{
public:
    TexturedSlalomGate(ShaderProgramPtr program, const MaterialPtr& mat);
    ~TexturedSlalomGate();
};

typedef std::shared_ptr<TexturedSlalomGate> TexturedSlalomGatePtr;

TexturedSlalomGatePtr createGate(ShaderProgramPtr program, const std::string& gateTextureFilename, const MaterialPtr& mat);


#endif
