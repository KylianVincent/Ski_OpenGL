#include "../include/ShaderProgram.hpp"
#include "../include/Viewer.hpp"
#include "../include/FrameRenderable.hpp"
#include "../include/lighting/DirectionalLightRenderable.hpp"

#include "../include/texturing/TexturedGroundRenderable.hpp"
#include "../include/texturing/TexturedPlaneRenderable.hpp"
#include "../include/texturing/TexturedCubeRenderable.hpp"
#include "../include/texturing/MultiTexturedCubeRenderable.hpp"
#include "../include/texturing/MipMapCubeRenderable.hpp"
#include "../include/texturing/TexturedMeshRenderable.hpp"
#include "../include/texturing/TexturedSnowmanRenderable.hpp"
#include "../include/texturing/TexturedTruncRenderable.hpp"

#include "../include/dynamics/DynamicSystem.hpp"
#include "../include/dynamics/DampingForceField.hpp"
#include "../include/dynamics/ConstantForceField.hpp"
#include "../include/dynamics/SpringForceField.hpp"
#include "../include/dynamics/EulerExplicitSolver.hpp"

#include "../include/dynamics_rendering/DynamicSystemRenderable.hpp"
#include "../include/dynamics_rendering/ParticleRenderable.hpp"
#include "../include/dynamics_rendering/DynamicMeshRenderable.hpp"
#include "../include/dynamics_rendering/ParticleListRenderable.hpp"
#include "../include/dynamics_rendering/ConstantForceFieldRenderable.hpp"
#include "../include/dynamics_rendering/SpringForceFieldRenderable.hpp"
#include "../include/dynamics_rendering/SpringListRenderable.hpp"
#include "../include/dynamics_rendering/ControlledForceFieldRenderable.hpp"
#include "../include/dynamics_rendering/QuadRenderable.hpp"

void initialize_ground_scene(Viewer& viewer)
{
    // create all shaders of this scene, then add them to the viewer
    ShaderProgramPtr flatShader
        = std::make_shared<ShaderProgram>("../shaders/flatVertex.glsl",
                                          "../shaders/flatFragment.glsl");
    viewer.addShaderProgram(flatShader);

    // Two texture shaders: simple and multi-texturing
    ShaderProgramPtr texShader
        = std::make_shared<ShaderProgram>("../shaders/textureVertex.glsl",
                                          "../shaders/textureFragment.glsl");
    viewer.addShaderProgram(texShader);

    ShaderProgramPtr multiTexShader
        = std::make_shared<ShaderProgram>("../shaders/multiTextureVertex.glsl",
                                          "../shaders/multiTextureFragment.glsl");
    viewer.addShaderProgram(multiTexShader);


    //Add a 3D frame to the viewer
    FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
    viewer.addRenderable(frame);

    //Position the camera
    viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3(0, -8, 8 ), glm::vec3(0, 0, 0), glm::vec3( 0, 0, 1 ) ) );

    //Initialize a dynamic system (Solver, Time step, Restitution coefficient)
    DynamicSystemPtr system = std::make_shared<DynamicSystem>();
    EulerExplicitSolverPtr solver = std::make_shared<EulerExplicitSolver>();
    system->setSolver(solver);
    system->setDt(0.01);
    DynamicSystemRenderablePtr systemRenderable = std::make_shared<DynamicSystemRenderable>(system);
    viewer.addRenderable(systemRenderable);

    //Temporary variables
    glm::mat4 parentTransformation(1.0), localTransformation(1.0);
    std::string filename;
    MaterialPtr pearl = Material::Pearl();

    //Define a directional light for the whole scene
    glm::vec3 d_direction = glm::normalize(glm::vec3(0.0,1.5,-1.0));
    glm::vec3 d_ambient(1.0,1.0,1.0), d_diffuse(1.0,1.0,0.8), d_specular(1.0,1.0,1.0);
    DirectionalLightPtr directionalLight = std::make_shared<DirectionalLight>(d_direction, d_ambient, d_diffuse, d_specular);
    viewer.setDirectionalLight(directionalLight);

    //Textured Ground
    filename = "../textures/snow_texture.png";
    TexturedGroundRenderablePtr texGround = std::make_shared<TexturedGroundRenderable>(texShader, filename);
    texGround->setParentTransform(parentTransformation);
    texGround->setMaterial(pearl);
    viewer.addRenderable(texGround);


    //Textured Tree
    //filename = "../textures/tree_texture.png";
    //std::string leafFilename = "../textures/grass_texture.png";
    //TexturedTruncRenderablePtr tronc = createTree(texShader,filename,leafFilename);
    //viewer.addRenderable(tronc);

    viewer.startAnimation();
}
