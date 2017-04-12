#ifndef TEXTURED_Leaf_RENDERABLE_HPP
#define TEXTURED_LEAF_RENDERABLE_HPP

#include "./../HierarchicalRenderable.hpp"
#include "./../lighting/Material.hpp"
#include <vector>
#include <glm/glm.hpp>

class TexturedLeafRenderable : public HierarchicalRenderable
{
    public:
        ~TexturedLeafRenderable();
        TexturedLeafRenderable(ShaderProgramPtr program, const std::string& LeafFilename);

    void setMaterial(const MaterialPtr& material);

    private:
        void do_draw();
        void do_animate(float time);

        std::vector<glm::vec3> m_positions;
        std::vector<glm::vec3> m_normals;
        std::vector<glm::vec2> m_texCoords;

        unsigned int m_pBuffer;
        unsigned int m_nBuffer;
        unsigned int m_tBuffer;
        unsigned int m_texId;

        MaterialPtr m_material;
};

typedef std::shared_ptr<TexturedLeafRenderable> TexturedLeafRenderablePtr;

#endif
