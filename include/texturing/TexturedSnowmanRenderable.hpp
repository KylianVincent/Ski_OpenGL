#ifndef TEXTURED_SNOWMAN_RENDERABLE_HPP
#define TEXTURED_SNOWMAN_RENDERABLE_HPP

#include "./../HierarchicalRenderable.hpp"
#include "./../lighting/Material.hpp"
#include "../../include/dynamics/DynamicSnowman.hpp"
#include <vector>
#include <glm/glm.hpp>

class TexturedSnowmanRenderable : public HierarchicalRenderable
{
    public:
        ~TexturedSnowmanRenderable();
        TexturedSnowmanRenderable(ShaderProgramPtr program, const std::string& bodyTextureFilename,
        DynamicSnowmanPtr snowman);

    void setMaterial(const MaterialPtr& material);

    private:
        void do_draw();
        void do_animate(float time);

        DynamicSnowmanPtr m_snowman;

        std::vector<glm::vec3> m_positions;
        std::vector<glm::vec3> m_normals;
        std::vector<glm::vec2> m_texCoords;

        unsigned int m_pBuffer;
        unsigned int m_nBuffer;
        unsigned int m_tBuffer;
        unsigned int m_texId;

        MaterialPtr m_material;
};

typedef std::shared_ptr<TexturedSnowmanRenderable> TexturedSnowmanRenderablePtr;


TexturedSnowmanRenderablePtr createSnowman(ShaderProgramPtr program, const std::string& bodyTextureFilename, const
                std::string& headTextureFilename, const std::string& noseTextureFilename, const std::string& armTextureFilename,
                const std::string& baseHatTextureFilename, const std::string& topHatTextureFilename, const DynamicSnowmanPtr m_snowman);

#endif
