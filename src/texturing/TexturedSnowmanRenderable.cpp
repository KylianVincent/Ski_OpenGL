#include "../../include/texturing/TexturedSnowmanRenderable.hpp"
#include "../../include/texturing/TexturedSnowmanHeadRenderable.hpp"
#include "../../include/texturing/TexturedSnowmanNoseRenderable.hpp"
#include "../../include/texturing/TexturedSnowmanHatRenderable.hpp"
#include "../../include/texturing/TexturedCubeRenderable.hpp"
#include "../../include/gl_helper.hpp"
#include "../../include/log.hpp"
#include "../../include/Utils.hpp"
#include "../../teachers/Geometries.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <math.h>
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

TexturedSnowmanRenderable::TexturedSnowmanRenderable(ShaderProgramPtr shaderProgram, const std::string& bodyTextureFilename,
            DynamicSnowmanPtr snowman)
        : HierarchicalRenderable(shaderProgram),
        m_snowman(snowman),
          m_pBuffer(0), m_nBuffer(0), m_tBuffer(0), m_texId(0)
{
    // ----------- Body ------------------
    teachers::getUnitSphereTextured(m_positions, m_normals, m_texCoords, 10, 10);

    m_model = glm::mat4(1.0);

    glGenBuffers(1, &m_pBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_pBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_positions.size()*sizeof(glm::vec3), m_positions.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_tBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_tBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_texCoords.size()*sizeof(glm::vec2), m_texCoords.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_nBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_normals.size()*sizeof(glm::vec3), m_normals.data(), GL_STATIC_DRAW);


    // now handle the "texture image" itself
    // load the image (here using the sfml library)
    sf::Image image;
    image.loadFromFile(bodyTextureFilename);
    // sfml inverts the v axis...
    // Hence, flip it to put the image in OpenGL convention: lower left corner is (0,0)
    image.flipVertically();

    // create a GPU buffer then bind the texture
    glcheck(glGenTextures(1, &m_texId));
    glcheck(glBindTexture(GL_TEXTURE_2D, m_texId));

    // texture options
    glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    // Transfer the texture image texture to the GPU
    glcheck(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,
        image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
        (const GLvoid*)image.getPixelsPtr()));

    // Release the texture
    glcheck(glBindTexture(GL_TEXTURE_2D, 0));
}

TexturedSnowmanRenderable::~TexturedSnowmanRenderable()
{
    glcheck(glDeleteBuffers(1, &m_pBuffer));
    glcheck(glDeleteBuffers(1, &m_tBuffer));
    glcheck(glDeleteBuffers(1, &m_nBuffer));

    glcheck(glDeleteTextures(1, &m_texId));
}

void TexturedSnowmanRenderable::do_draw()
{

    const glm::vec3& pPosition = m_snowman->getPosition();
    glm::mat4 translate = glm::translate(glm::mat4(1.0), glm::vec3(pPosition));
    setParentTransform(translate);

    //Locations
    int modelLocation = m_shaderProgram->getUniformLocation("modelMat");
    int nitLocation = m_shaderProgram->getUniformLocation("NIT");
    int texSamplerLocation = m_shaderProgram->getUniformLocation("texSampler");

    int positionLocation = m_shaderProgram->getAttributeLocation("vPosition");
    int normalLocation = m_shaderProgram->getAttributeLocation("vNormal");
    int texCoordLocation = m_shaderProgram->getAttributeLocation("vTexCoord");

    //Send material as uniform to GPU
    Material::sendToGPU(m_shaderProgram, m_material);

    //Send uniforms to GPU
    if (modelLocation != ShaderProgram::null_location) {
        glcheck(glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(getModelMatrix())));
    }
    if (nitLocation != ShaderProgram::null_location) {
        glcheck(glUniformMatrix3fv(nitLocation, 1, GL_FALSE,
            glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(getModelMatrix()))))));
    }

    //Send vertex attributes to GPU
    if (positionLocation != ShaderProgram::null_location) {
        glcheck(glEnableVertexAttribArray(positionLocation));
        glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_pBuffer));
        glcheck(glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    }
    if (normalLocation != ShaderProgram::null_location) {
        glcheck(glEnableVertexAttribArray(normalLocation));
        glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer));
        glcheck(glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    }

    // Texture and texture coordinates
    if (texCoordLocation != ShaderProgram::null_location) {
    // Handle transparency
    glcheck(glEnable(GL_BLEND));
    glcheck(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        // Bind texture on texture unit 0
        glcheck(glActiveTexture(GL_TEXTURE0));
        glcheck(glBindTexture(GL_TEXTURE_2D, m_texId));

        // Tells the sampler to use the texture unit 0
        glcheck(glUniform1i(texSamplerLocation, 0));

        // Send texture coordinates attributes
        glcheck(glEnableVertexAttribArray(texCoordLocation));
        glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_tBuffer));
        glcheck(glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));
    }

    //Draw triangles elements
    glcheck(glDrawArrays(GL_TRIANGLES,0, m_positions.size()));

    //Release everything
    if (positionLocation != ShaderProgram::null_location) {
        glcheck(glDisableVertexAttribArray(positionLocation));
    }
    if (normalLocation != ShaderProgram::null_location) {
        glcheck(glDisableVertexAttribArray(normalLocation));
    }

    if (texCoordLocation != ShaderProgram::null_location) {
        glcheck(glDisableVertexAttribArray(texCoordLocation));
        glcheck(glBindTexture(GL_TEXTURE_2D, 0));   // unbind the texture!
    }
}

void TexturedSnowmanRenderable::do_animate(float time)
{
}

TexturedSnowmanRenderablePtr createSnowman(ShaderProgramPtr program, const std::string& bodyTextureFilename,
                                           const std::string& headTextureFilename, const std::string& noseTextureFilename,
                                           const std::string& armTextureFilename,  const std::string& baseHatTextureFilename,
                                           const std::string& topHatTextureFilename, const std::string& skiTextureFilename,
                                           const std::string& skiStickTextureFilename, const DynamicSnowmanPtr snowman) {


        //Temporary variables
    glm::mat4 parentTransformation(1.0), localTransformation(1.0);

    // ---------- Body -----------
    TexturedSnowmanRenderablePtr body = std::make_shared<TexturedSnowmanRenderable>(program, bodyTextureFilename, snowman);
    localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(1,1,0.9));
    body->setLocalTransform(localTransformation);

    TexturedSnowmanHeadRenderablePtr upperBody = std::make_shared<TexturedSnowmanHeadRenderable>(program, bodyTextureFilename);
    parentTransformation = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 1));
    localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(0.6,0.6,0.4));
    upperBody->setParentTransform(parentTransformation);
    upperBody->setLocalTransform(localTransformation);

    // ---------- Head -----------
    TexturedSnowmanHeadRenderablePtr head = std::make_shared<TexturedSnowmanHeadRenderable>(program, headTextureFilename);
    parentTransformation = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0.7));
    localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(0.5,0.5,0.5));
    head->setParentTransform(parentTransformation);
    head->setLocalTransform(localTransformation);

    // ---------- Nose -----------
    TexturedSnowmanNoseRenderablePtr nose = std::make_shared<TexturedSnowmanNoseRenderable>(program, noseTextureFilename);
    parentTransformation = glm::rotate(glm::mat4(1.0), 1.57f, glm::vec3(0, 1, 0));
    localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(0.1, 0.1, 1.3));
    nose->setParentTransform(parentTransformation);
    nose->setLocalTransform(localTransformation);


    // -------- Left arm ---------
    TexturedSnowmanHatRenderablePtr lUpperArm = std::make_shared<TexturedSnowmanHatRenderable>(program, armTextureFilename);
    parentTransformation = glm::mat4(1.0);
    localTransformation = glm::mat4(1.0);
    float lArmAngle = 1.5f;
    parentTransformation = glm::translate(parentTransformation, glm::vec3(0, -0.5, 0));
    parentTransformation = glm::rotate(parentTransformation, -0.5f, glm::vec3(0, 0, 1));
    parentTransformation = glm::rotate(parentTransformation, lArmAngle, glm::vec3(0, 1, 0));
    localTransformation = glm::scale(localTransformation, glm::vec3(0.08, 0.08, 0.6));
    lUpperArm->setParentTransform(parentTransformation);
    lUpperArm->setLocalTransform(localTransformation);

    TexturedSnowmanHatRenderablePtr lLowerArm = std::make_shared<TexturedSnowmanHatRenderable>(program, armTextureFilename);
    parentTransformation = glm::mat4(1.0);
    localTransformation = glm::mat4(1.0);
    parentTransformation = glm::translate(parentTransformation, glm::vec3(0, 0, 0.55));
    parentTransformation = glm::rotate(parentTransformation, -0.3f, glm::vec3(1, 0, 0));
    parentTransformation = glm::rotate(parentTransformation, -0.4f-lArmAngle/10, glm::vec3(0, 1, 0));
    localTransformation = glm::scale(localTransformation, glm::vec3(0.08, 0.08, 0.6));
    lLowerArm->setParentTransform(parentTransformation);
    lLowerArm->setLocalTransform(localTransformation);

    TexturedSnowmanHatRenderablePtr lSkiStick = std::make_shared<TexturedSnowmanHatRenderable>(program, skiStickTextureFilename);
    parentTransformation = glm::mat4(1.0);
    localTransformation = glm::mat4(1.0);
    parentTransformation = glm::translate(parentTransformation, glm::vec3(0, 0, 0.55));
    parentTransformation = glm::rotate(parentTransformation, -0.2f, glm::vec3(0, 0, 1));
    parentTransformation = glm::rotate(parentTransformation, lArmAngle, glm::vec3(0, 1, 0));
    localTransformation = glm::scale(localTransformation, glm::vec3(0.02, 0.02, 1.5));
    localTransformation = glm::translate(localTransformation, glm::vec3(0, 0, 1));
    localTransformation = glm::rotate(localTransformation, 3.14f, glm::vec3(1, 0, 0));
    lSkiStick->setParentTransform(parentTransformation);
    lSkiStick->setLocalTransform(localTransformation);



    // -------- Right arm --------
    TexturedSnowmanHatRenderablePtr rUpperArm = std::make_shared<TexturedSnowmanHatRenderable>(program, armTextureFilename);
    parentTransformation = glm::mat4(1.0);
    localTransformation = glm::mat4(1.0);
    float rArmAngle = 1.5f;
    parentTransformation = glm::translate(parentTransformation, glm::vec3(0, 0.5, 0));
    parentTransformation = glm::rotate(parentTransformation, 0.5f, glm::vec3(0, 0, 1));
    parentTransformation = glm::rotate(parentTransformation, rArmAngle, glm::vec3(0, 1, 0));
    localTransformation = glm::scale(localTransformation, glm::vec3(0.08, 0.08, 0.6));
    rUpperArm->setParentTransform(parentTransformation);
    rUpperArm->setLocalTransform(localTransformation);

    TexturedSnowmanHatRenderablePtr rLowerArm = std::make_shared<TexturedSnowmanHatRenderable>(program, armTextureFilename);
    parentTransformation = glm::mat4(1.0);
    localTransformation = glm::mat4(1.0);
    parentTransformation = glm::translate(parentTransformation, glm::vec3(0, 0, 0.55));
    parentTransformation = glm::rotate(parentTransformation, 0.3f, glm::vec3(1, 0, 0));
    parentTransformation = glm::rotate(parentTransformation, -0.4f-rArmAngle/10, glm::vec3(0, 1, 0));
    localTransformation = glm::scale(localTransformation, glm::vec3(0.08, 0.08, 0.6));
    rLowerArm->setParentTransform(parentTransformation);
    rLowerArm->setLocalTransform(localTransformation);

    TexturedSnowmanHatRenderablePtr rSkiStick = std::make_shared<TexturedSnowmanHatRenderable>(program, skiStickTextureFilename);
    parentTransformation = glm::mat4(1.0);
    localTransformation = glm::mat4(1.0);
    parentTransformation = glm::translate(parentTransformation, glm::vec3(0, 0, 0.55));
    parentTransformation = glm::rotate(parentTransformation, 0.2f, glm::vec3(0, 0, 1));
    parentTransformation = glm::rotate(parentTransformation, rArmAngle, glm::vec3(0, 1, 0));
    localTransformation = glm::scale(localTransformation, glm::vec3(0.02, 0.02, 1.5));
    localTransformation = glm::translate(localTransformation, glm::vec3(0, 0, 1));
    localTransformation = glm::rotate(localTransformation, 3.14f, glm::vec3(1, 0, 0));
    rSkiStick->setParentTransform(parentTransformation);
    rSkiStick->setLocalTransform(localTransformation);


    // ----------- Hat -----------
    TexturedSnowmanHatRenderablePtr hat = std::make_shared<TexturedSnowmanHatRenderable>(program, baseHatTextureFilename);
    parentTransformation = glm::rotate(glm::mat4(1.0), 0.3f, glm::vec3(1, 0, 0));
    parentTransformation = glm::translate(parentTransformation, glm::vec3(0, 0, 0.4));
    localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.01));
    hat->setParentTransform(parentTransformation);
    hat->setLocalTransform(localTransformation);

    TexturedSnowmanHatRenderablePtr topHat = std::make_shared<TexturedSnowmanHatRenderable>(program, topHatTextureFilename);
    localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(0.3, 0.3, 1));
    topHat->setLocalTransform(localTransformation);

    // ----------- Skis -----------
    TexturedCubeRenderablePtr leftSki = std::make_shared<TexturedCubeRenderable>(program, skiTextureFilename);
    parentTransformation = glm::translate(glm::mat4(1.0), glm::vec3(0.3, 0.3, -0.8));
    parentTransformation = glm::rotate(parentTransformation, (float)-(M_PI/2), glm::vec3(0, 0, 1));
    localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(0.5,5,0.01));
    leftSki->setParentTransform(parentTransformation);
    leftSki->setLocalTransform(localTransformation);

    TexturedCubeRenderablePtr rightSki = std::make_shared<TexturedCubeRenderable>(program, skiTextureFilename);
    parentTransformation = glm::translate(glm::mat4(1.0), glm::vec3(0.3, -0.3, -0.8));
    parentTransformation = glm::rotate(parentTransformation, (float)-(M_PI/2), glm::vec3(0, 0, 1));
    localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(0.5,5,0.01));
    rightSki->setParentTransform(parentTransformation);
    rightSki->setLocalTransform(localTransformation);



    HierarchicalRenderable::addChild(body, upperBody);
    HierarchicalRenderable::addChild(upperBody, head);
    HierarchicalRenderable::addChild(head, nose);
    HierarchicalRenderable::addChild(head, hat);
    HierarchicalRenderable::addChild(hat, topHat);

    HierarchicalRenderable::addChild(upperBody, rUpperArm);
    HierarchicalRenderable::addChild(rUpperArm, rLowerArm);
    HierarchicalRenderable::addChild(rLowerArm, rSkiStick);

    HierarchicalRenderable::addChild(upperBody, lUpperArm);
    HierarchicalRenderable::addChild(lUpperArm, lLowerArm);
    HierarchicalRenderable::addChild(lLowerArm, lSkiStick);

    HierarchicalRenderable::addChild(body, leftSki);
    HierarchicalRenderable::addChild(body, rightSki);

    return body;
}
