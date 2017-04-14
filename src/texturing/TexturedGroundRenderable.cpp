#include "../../include/texturing/TexturedGroundRenderable.hpp"
#include "../../include/gl_helper.hpp"
#include "../../include/log.hpp"
#include "../../teachers/Geometries.hpp"

#include <math.h>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <SFML/Graphics/Image.hpp>
#include <iostream>

const int tailleX = 1000;
const int tailleY = 400;
float matriceElevation[tailleX][tailleY];

void initinlizeMatriceElevation(float mat[][tailleY])
{
  for (int i=0; i<tailleX; i++){
    for (int j=0; j<tailleY; j++){
      mat[i][j] = 0;
    }
  }
  return ;
}

float Get2DPerlinNoiseValue(float x, float y, float res)
{
    float tempX,tempY;
    int x0,y0,ii,jj,gi0,gi1,gi2,gi3;
    float unit = 1.0f/sqrt(2);
    float tmp,s,t,u,v,Cx,Cy,Li1,Li2;
    float gradient2[][2] = {{unit,unit},{-unit,unit},{unit,-unit},{-unit,-unit},{1,0},{-1,0},{0,1},{0,-1}};
    unsigned int perm[] =
       {151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,
        142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,
        203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,
        74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,
        105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,
        187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,
        64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,
        47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,
        153,101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,
        112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,
        235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,
        127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,
        156,180};

    //Adapter pour la résolution
    x /= res;
    y /= res;

    //On récupère les positions de la grille associée à (x,y)
    x0 = (int)(x);
    y0 = (int)(y);

    //Masquage
    ii = x0 & 255;
    jj = y0 & 255;

    //Pour récupérer les vecteurs
    gi0 = perm[ii + perm[jj]] % 8;
    gi1 = perm[ii + 1 + perm[jj]] % 8;
    gi2 = perm[ii + perm[jj + 1]] % 8;
    gi3 = perm[ii + 1 + perm[jj + 1]] % 8;

    //on récupère les vecteurs et on pondère
    tempX = x-x0;
    tempY = y-y0;
    s = gradient2[gi0][0]*tempX + gradient2[gi0][1]*tempY;
    tempX = x-(x0+1);
    tempY = y-y0;
    t = gradient2[gi1][0]*tempX + gradient2[gi1][1]*tempY;
    tempX = x-x0;
    tempY = y-(y0+1);
    u = gradient2[gi2][0]*tempX + gradient2[gi2][1]*tempY;
    tempX = x-(x0+1);
    tempY = y-(y0+1);
    v = gradient2[gi3][0]*tempX + gradient2[gi3][1]*tempY;

    //Lissage
    tmp = x-x0;
    Cx = 3 * tmp * tmp - 2 * tmp * tmp * tmp;
    Li1 = s + Cx*(t-s);
    Li2 = u + Cx*(v-u);
    tmp = y - y0;
    Cy = 3 * tmp * tmp - 2 * tmp * tmp * tmp;

    return Li1 + Cy*(Li2-Li1);

}

float TexturedGroundRenderable::getMatriceElevation(int x, int y){
  return matriceElevation[x][y];
}

TexturedGroundRenderable::TexturedGroundRenderable(
        ShaderProgramPtr shaderProgram, const std::string& textureFilename)
    : HierarchicalRenderable(shaderProgram),
      m_pBuffer(0), m_nBuffer(0), m_tBuffer(0), m_texId(0),
      m_wrapOption(0), m_filterOption(0)
{


  //float matriceElevation[tailleX][tailleY];
  initinlizeMatriceElevation(matriceElevation);
  float test;
  for (int i=0; i<tailleX; i++){
    for (int j=0; j<tailleY; j++){
      matriceElevation[i][j] = Get2DPerlinNoiseValue((float)i,(float)j,10)*5;
    }
  }

    // Initialize geometry
    m_positions.resize(6*tailleX*tailleY, glm::vec3(0.0,0.0,0.0));
    m_normals.resize(6*tailleX*tailleY, glm::vec3(0.0,0.0,0.0));
    m_texCoords.resize(6*tailleX*tailleY, glm::vec2(0.0,0.0));
    glm::vec3 ptg = glm::vec3(0,0,0);
    glm::vec3 ptd = glm::vec3(0,0,0);
    glm::vec3 pth = glm::vec3(0,0,0);
    glm::vec3 ptb = glm::vec3(0,0,0);
    glm::vec3 ptc = glm::vec3(0,0,0);


    for (int i=1; i<tailleX-2; i++){
          for (int j=1; j<tailleY-2; j++){
            //1er face du "carré"
            m_positions.push_back(glm::vec3(i-tailleX/2,j-tailleY/2,matriceElevation[i][j]));
            m_positions.push_back(glm::vec3(i-tailleX/2,j+1-tailleY/2,matriceElevation[i][j+1]));
            m_positions.push_back(glm::vec3(i+1-tailleX/2,j-tailleY/2,matriceElevation[i+1][j]));

            m_texCoords.push_back(glm::vec2(0.0,0.0));
            m_texCoords.push_back(glm::vec2(0.0,1.0));
            m_texCoords.push_back(glm::vec2(1.0,1.0));

            ptg = glm::vec3(i-1,j,matriceElevation[i-1][j]);
            ptd = glm::vec3(i+1,j,matriceElevation[i+1][j]);
            pth = glm::vec3(i,j+1,matriceElevation[i][j+1]);
            ptb = glm::vec3(i,j-1,matriceElevation[i][j-1]);
            ptc = glm::vec3(i,j,matriceElevation[i][j]);
            m_normals.push_back(glm::normalize(glm::normalize(cross(ptg-ptc,ptb-ptc))
                                              +glm::normalize(cross(ptb-ptc,ptd-ptc))
                                              +glm::normalize(cross(ptd-ptc,pth-ptc))
                                              +glm::normalize(cross(pth-ptc,ptg-ptc)) ));

            ptg = glm::vec3(i-1,j+1,matriceElevation[i-1][j+1]);
            ptd = glm::vec3(i+1,j+1,matriceElevation[i+1][j+1]);
            pth = glm::vec3(i,j+2,matriceElevation[i][j+2]);
            ptb = glm::vec3(i,j,matriceElevation[i][j]);
            ptc = glm::vec3(i,j+1,matriceElevation[i][j+1]);
            m_normals.push_back(glm::normalize(glm::normalize(cross(ptg-ptc,ptb-ptc))
                                              +glm::normalize(cross(ptb-ptc,ptd-ptc))
                                              +glm::normalize(cross(ptd-ptc,pth-ptc))
                                              +glm::normalize(cross(pth-ptc,ptg-ptc)) ));

            ptg = glm::vec3(i,j,matriceElevation[i][j]);
            ptd = glm::vec3(i+2,j,matriceElevation[i+2][j]);
            pth = glm::vec3(i+1,j+1,matriceElevation[i+1][j+1]);
            ptb = glm::vec3(i+1,j-1,matriceElevation[i+1][j-1]);
            ptc = glm::vec3(i+1,j,matriceElevation[i+1][j]);
            m_normals.push_back(glm::normalize(glm::normalize(cross(ptg-ptc,ptb-ptc))
                                              +glm::normalize(cross(ptb-ptc,ptd-ptc))
                                              +glm::normalize(cross(ptd-ptc,pth-ptc))
                                              +glm::normalize(cross(pth-ptc,ptg-ptc)) ));


            //2eme face du "carré"
            m_positions.push_back(glm::vec3(i-tailleX/2,j+1-tailleY/2,matriceElevation[i][j+1]));
            m_positions.push_back(glm::vec3(i+1-tailleX/2,j-tailleY/2,matriceElevation[i+1][j]));
            m_positions.push_back(glm::vec3(i+1-tailleX/2,j+1-tailleY/2,matriceElevation[i+1][j+1]));

            m_texCoords.push_back(glm::vec2(0.0,0.0));
            m_texCoords.push_back(glm::vec2(1.0,1.0));
            m_texCoords.push_back(glm::vec2(0.0,1.0));

            ptg = glm::vec3(i-1,j+1,matriceElevation[i-1][j+1]);
            ptd = glm::vec3(i+1,j+1,matriceElevation[i+1][j+1]);
            pth = glm::vec3(i,j+2,matriceElevation[i][j+2]);
            ptb = glm::vec3(i,j,matriceElevation[i][j]);
            ptc = glm::vec3(i,j+1,matriceElevation[i][j+1]);
            m_normals.push_back(glm::normalize(glm::normalize(cross(ptg-ptc,ptb-ptc))
                                              +glm::normalize(cross(ptb-ptc,ptd-ptc))
                                              +glm::normalize(cross(ptd-ptc,pth-ptc))
                                              +glm::normalize(cross(pth-ptc,ptg-ptc)) ));

            ptg = glm::vec3(i,j,matriceElevation[i][j]);
            ptd = glm::vec3(i+2,j,matriceElevation[i+2][j]);
            pth = glm::vec3(i+1,j+1,matriceElevation[i+1][j+1]);
            ptb = glm::vec3(i+1,j-1,matriceElevation[i+1][j-1]);
            ptc = glm::vec3(i+1,j,matriceElevation[i+1][j]);
            m_normals.push_back(glm::normalize(glm::normalize(cross(ptg-ptc,ptb-ptc))
                                              +glm::normalize(cross(ptb-ptc,ptd-ptc))
                                              +glm::normalize(cross(ptd-ptc,pth-ptc))
                                              +glm::normalize(cross(pth-ptc,ptg-ptc)) ));

            ptg = glm::vec3(i,j+1,matriceElevation[i][j+1]);
            ptd = glm::vec3(i+2,j+1,matriceElevation[i+2][j+1]);
            pth = glm::vec3(i+1,j+2,matriceElevation[i+1][j+2]);
            ptb = glm::vec3(i+1,j,matriceElevation[i+1][j]);
            ptc = glm::vec3(i+1,j+1,matriceElevation[i+1][j+1]);
            m_normals.push_back(glm::normalize(glm::normalize(cross(ptg-ptc,ptb-ptc))
                                              +glm::normalize(cross(ptb-ptc,ptd-ptc))
                                              +glm::normalize(cross(ptd-ptc,pth-ptc))
                                              +glm::normalize(cross(pth-ptc,ptg-ptc)) ));
          }
    }


    // === PART 1: Vertex attributes, except texture coordinates
    //Create buffers
    glGenBuffers(1, &m_pBuffer); //vertices
    glGenBuffers(1, &m_nBuffer); //normals
    //Activate buffer and send data to the graphics card
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_pBuffer));
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_pBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_positions.size()*sizeof(glm::vec3), m_positions.data(), GL_STATIC_DRAW));
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_normals.size()*sizeof(glm::vec3), m_normals.data(), GL_STATIC_DRAW));

    // === PART 2: Texture
    // texture coordinates: just like any other vertex attributes!
    glGenBuffers(1, &m_tBuffer); //texture coords
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_tBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_texCoords.size()*sizeof(glm::vec2), m_texCoords.data(), GL_STATIC_DRAW));

    // now handle the "texture image" itself
    // load the image (here using the sfml library)
    sf::Image image;
    image.loadFromFile(textureFilename);
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

TexturedGroundRenderable::~TexturedGroundRenderable()
{
    glcheck(glDeleteBuffers(1, &m_pBuffer));
    glcheck(glDeleteBuffers(1, &m_nBuffer));
    glcheck(glDeleteBuffers(1, &m_tBuffer));

    glcheck(glDeleteTextures(1, &m_texId));
}

void TexturedGroundRenderable::do_draw()
{
    //Locations
    int modelLocation = m_shaderProgram->getUniformLocation("modelMat");
    int nitLocation = m_shaderProgram->getUniformLocation("NIT");
    int texSamplerLocaction = m_shaderProgram->getUniformLocation("texSampler");

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
        glcheck(glUniform1i(texSamplerLocaction, 0));

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

void TexturedGroundRenderable::do_animate(float time)
{
}


void TexturedGroundRenderable::updateTextureOption()
{
    //Resize texture coordinates factor
    float factor = 10.0;

    std::string text;

    //Bind the texture
    glBindTexture(GL_TEXTURE_2D, m_texId);

    //Textured options
    switch(m_wrapOption)
    {
    case 0:
        for(size_t i=0; i<m_texCoords.size(); ++i)
        {
            m_texCoords[i] = m_origTexCoords[i];
        }
        glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        text.append("Wrapping: CLAMP_TO_EDGE");
        break;

    case 1:
        for(size_t i=0; i<m_texCoords.size(); ++i)
        {
            m_texCoords[i] = factor*m_origTexCoords[i];
        }
        glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        text.append("Wrapping: REPEAT");
        break;

    case 2:
        for(size_t i=0; i<m_texCoords.size(); ++i)
        {
            m_texCoords[i] = factor*m_origTexCoords[i];
        }
        glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT));
        glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT));
        text.append("Wrapping: MIRRORED_REPEAT");
        break;

    case 3:
        for(size_t i=0; i<m_texCoords.size(); ++i)
        {
            m_texCoords[i] = factor*m_origTexCoords[i] - glm::vec2(factor/2.0, factor/2.0);
        }
        glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        text.append("Wrapping: CLAMP_TO_EDGE");
        break;

    case 4:
        for(size_t i=0; i<m_texCoords.size(); ++i)
        {
            m_texCoords[i] = factor*m_origTexCoords[i] - glm::vec2(factor/2.0, factor/2.0);
        }
        float borderColor[] = { 1.0f, 0.0f, 1.0f, 1.0f };
        glcheck(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));
        glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
        glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
        text.append("Wrapping: CLAMP_TO_BORDER");
        break;
    }


    switch(m_filterOption)
    {
    case 0:
        glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        text.append(", Filtering: NEAREST");
        break;

    case 1:
        glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        text.append(", Filtering: LINEAR");
        break;

//    case 2:
//        // generate mipmap images
//        glcheck(glGenerateMipmap(GL_TEXTURE_2D));
//        glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
//        glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
//        text.append(", Filtering: MIPMAP");
//        break;
    }

    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_tBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_texCoords.size()*sizeof(glm::vec2), m_texCoords.data(), GL_STATIC_DRAW));

    //Release the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    displayTextInViewer(text);
}

void TexturedGroundRenderable::do_keyPressedEvent( sf::Event& e )
{
    if (e.key.code == sf::Keyboard::F6) {
        m_wrapOption = m_wrapOption+1 > 4 ? 0 : m_wrapOption+1;
        updateTextureOption();
    }
    if (e.key.code == sf::Keyboard::F7) {
        m_filterOption = m_filterOption+1 > 1 ? 0 : m_filterOption+1;
        updateTextureOption();
    }
}

void TexturedGroundRenderable::setMaterial(const MaterialPtr& material)
{
    m_material = material;
}
