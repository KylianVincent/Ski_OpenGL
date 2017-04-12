#include "../../include/texturing/TexturedSlalomGate.hpp"
#include "../../include/texturing/TexturedPlaneRenderable.hpp"
#include "../../include/gl_helper.hpp"
#include "../../teachers/Geometries.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <SFML/Graphics/Image.hpp>
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

TexturedSlalomGate::TexturedSlalomGate(ShaderProgramPtr program, const MaterialPtr& material)
    : LightedCylinderRenderable(program, material)
{
}

TexturedSlalomGate::~TexturedSlalomGate()
{
}


TexturedSlalomGatePtr createGate(ShaderProgramPtr program, const std::string& gateTextureFilename, const MaterialPtr& mat) {
    //Temporary variables
    glm::mat4 parentTransformation(1.0), localTransformation(1.0);

    // ---------- Poles -----------
    TexturedSlalomGatePtr leftPole = std::make_shared<TexturedSlalomGate>(program, mat);
    localTransformation = glm::translate(glm::mat4(1.0), glm::vec3(0, 0.7, 0));
    localTransformation = glm::scale(localTransformation, glm::vec3(0.05,0.05,3));
    leftPole->setLocalTransform(localTransformation);

    TexturedSlalomGatePtr rightPole = std::make_shared<TexturedSlalomGate>(program, mat);
    localTransformation = glm::translate(glm::mat4(1.0), glm::vec3(0, -0.7, 0));
    localTransformation = glm::scale(localTransformation, glm::vec3(0.05,0.05,3));
    rightPole->setLocalTransform(localTransformation);

    // ---------- Fabric -----------
    TexturedPlaneRenderablePtr fabric = std::make_shared<TexturedPlaneRenderable>(program, gateTextureFilename);
    localTransformation = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 2));
    localTransformation = glm::scale(localTransformation, glm::vec3(1,1.4,1));
    localTransformation = glm::rotate(localTransformation, (float) M_PI/2.0f, glm::vec3(0,1,0));
    fabric->setLocalTransform(localTransformation);

    HierarchicalRenderable::addChild(leftPole, rightPole);
    HierarchicalRenderable::addChild(leftPole, fabric);

    return leftPole;
}
