#include "../../include/texturing/TexturedTruncRenderable.hpp"
#include "../../include/texturing/TexturedLeafRenderable.hpp"
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

TexturedTruncRenderable::TexturedTruncRenderable(ShaderProgramPtr shaderProgram, const std::string&TruncTextureFilename)
        : HierarchicalRenderable(shaderProgram),
          m_pBuffer(0), m_nBuffer(0), m_tBuffer(0), m_texId(0)
{

    teachers::getUnitConeTextured(m_positions, m_normals, m_texCoords, 10, 10);

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
    image.loadFromFile(TruncTextureFilename);
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

TexturedTruncRenderable::~TexturedTruncRenderable()
{
    glcheck(glDeleteBuffers(1, &m_pBuffer));
    glcheck(glDeleteBuffers(1, &m_tBuffer));
    glcheck(glDeleteBuffers(1, &m_nBuffer));

    glcheck(glDeleteTextures(1, &m_texId));
}

void TexturedTruncRenderable::do_draw()
{
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

void TexturedTruncRenderable::do_animate(float time)
{
}

void TexturedTruncRenderable::setMaterial(const MaterialPtr& material)
{
    m_material = material;
}

TexturedTruncRenderablePtr createTree(ShaderProgramPtr program, const std::string& TruncTextureFilename,
                                           const std::string& LeafTextureFilename, DynamicSystemPtr system,
                                           const ParticlePtr& particle) {

      //Temporary variables
      glm::mat4 parentTransformation(1.0), localTransformation(1.0), scale(1.0), rot(1.0), trans(1.0);
      MaterialPtr pearl = Material::Pearl();
      // ---------- Trunc -----------
      TexturedTruncRenderablePtr tronc = std::make_shared<TexturedTruncRenderable>(program, TruncTextureFilename);
      scale = glm::scale(glm::mat4(1.0), glm::vec3(1.0,1.0,21.0));
      tronc->setLocalTransform(tronc->getModelMatrix()*scale);
      tronc->setParentTransform(parentTransformation);
      tronc->setMaterial(pearl);
      particle->setFixed(true);
      system->addParticle(particle);

      int nb = 6;
      for (int i = 1; i < nb; i++){
        TexturedTruncRenderablePtr part = std::make_shared<TexturedTruncRenderable>(program,LeafTextureFilename);
        scale = glm::scale(glm::mat4(1.0), glm::vec3(5.0+0.5*i,5.0+0.5*i,21.0 - nb));
        trans = glm::translate(glm::mat4(1.0), glm::vec3(0.0,0.0,2.0*(nb-i) + 4));
        part->setLocalTransform(part->getModelMatrix()*scale);
        part->setParentTransform(trans);
        part->setMaterial(pearl);
        HierarchicalRenderable::addChild(tronc,part);
      }

      return tronc;
}
