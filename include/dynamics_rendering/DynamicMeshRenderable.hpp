#ifndef DYNAMIC_MESH_RENDERABLE_HPP
#define DYNAMIC_MESH_RENDERABLE_HPP

#include "./../HierarchicalRenderable.hpp"
#include "./../lighting/Material.hpp"
#include "../lighting/LightedMeshRenderable.hpp"
#include "../../include/dynamics/DynamicMesh.hpp"

#include <string>
#include <vector>
#include <glm/glm.hpp>

/* Stand-alone class, without inheritance from existing objects */

class DynamicMeshRenderable : public HierarchicalRenderable
{
public:
    ~DynamicMeshRenderable();
    DynamicMeshRenderable(ShaderProgramPtr program,
        const std::string& meshFilename,
        const std::string& textureFilename,
        DynamicMeshPtr mesh);
    void setMaterial(const MaterialPtr& material);

protected:
    virtual void do_draw();
    void do_animate(float time);

    DynamicMeshPtr m_mesh;

    std::vector<glm::vec3> m_positions;
    std::vector<glm::vec3> m_normals;
    std::vector<unsigned int> m_indices;
    std::vector<glm::vec2> m_texCoords;

    unsigned int m_pBuffer;
    unsigned int m_nBuffer;
    unsigned int m_iBuffer;
    unsigned int m_tBuffer;

    unsigned int m_texId;

    MaterialPtr m_material;
};

typedef std::shared_ptr<DynamicMeshRenderable> DynamicMeshRenderablePtr;

#endif
