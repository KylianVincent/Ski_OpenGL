#ifndef SNOW_PROJECTIONS_HPP
#define SNOW_PROJECTIONS_HPP

#include "./../HierarchicalRenderable.hpp"
#include "./../lighting/Material.hpp"
#include "./../dynamics/Particle.hpp"
#include <vector>
#include <glm/glm.hpp>

class SnowProjections : public HierarchicalRenderable
{
public:
    ~SnowProjections();
    SnowProjections(ShaderProgramPtr program, const std::string& snowProjectionsTextureFilename,
                    ParticlePtr particle);

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
    ParticlePtr m_particle;
    float m_previousTime = 0.0f;
    float m_previousAngle = 0.0f;
};

typedef std::shared_ptr<SnowProjections> SnowProjectionsPtr;

#endif
