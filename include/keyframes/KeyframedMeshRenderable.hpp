#ifndef KEYFRAMED_MESH_RENDERABLE_HPP
#define KEYFRAMED_MESH_RENDERABLE_HPP

#include "../texturing/TexturedMeshRenderable.hpp"
#include "KeyframeCollection.hpp"

#include <glm/glm.hpp>

class GeometricTransformation;

class KeyframedMeshRenderable : public TexturedMeshRenderable
{
public:
    KeyframedMeshRenderable(ShaderProgramPtr program,
        const MaterialPtr& material,
        const std::string& meshFilename,
        const std::string& textureFilename);
    ~KeyframedMeshRenderable();

    /**
    * \brief Add a keyframe for the parent transformation of the renderable.
    *
    * Add a keyframe to m_parentKeyframes described by a geometric transformation and a time.
    * \param time The time of the keyframe.
    * \param transformation The geometric transformation of the keyframe.
    */
    void addParentTransformKeyframe(float time, const GeometricTransformation& transformation);

    /**
     * \brief Add a keyframe for the local transformation of the renderable.
     *
     * Add a keyframe to m_localKeyframes described by a geometric transformation and a time.
     * \param time The time of the keyframe.
     * \param transformation The geometric transformation of the keyframe.
     */
    void addLocalTransformKeyframe(float time, const GeometricTransformation& transformation);

protected:
    // void do_draw();
    void do_animate(float time);

private:
    KeyframeCollection m_localKeyframes; /*!< A collection of keyframes for the local transformation of renderable. */
    KeyframeCollection m_parentKeyframes; /*!< A collection of keyframes for the parent transformation of renderable. */
};

typedef std::shared_ptr<KeyframedMeshRenderable> KeyframedCylinderRenderablePtr;

#endif
