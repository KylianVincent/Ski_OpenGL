#include "../include/ShaderProgram.hpp"
#include "../include/Viewer.hpp"
#include "../include/FrameRenderable.hpp"
#include "../include/lighting/DirectionalLightRenderable.hpp"

#include "../include/texturing/TexturedPlaneRenderable.hpp"
#include "../include/texturing/TexturedCubeRenderable.hpp"
#include "../include/texturing/MultiTexturedCubeRenderable.hpp"
#include "../include/texturing/MipMapCubeRenderable.hpp"
#include "../include/texturing/TexturedMeshRenderable.hpp"
#include "../include/texturing/TexturedSnowmanRenderable.hpp"
#include "../include/texturing/TexturedSlalomGate.hpp"
#include "../include/lighting/LightedCylinderRenderable.hpp"

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


void initialize_snowman_scene(Viewer& viewer)
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
    MaterialPtr snow = Material::Snow();

    //Define a directional light for the whole scene
    glm::vec3 d_direction = glm::normalize(glm::vec3(-10.0,-1.5,-1.0));
    glm::vec3 d_ambient(1.0,1.0,1.0), d_diffuse(1.0,1.0,0.8), d_specular(1.0,1.0,1.0);
    DirectionalLightPtr directionalLight = std::make_shared<DirectionalLight>(d_direction, d_ambient, d_diffuse, d_specular);
    viewer.setDirectionalLight(directionalLight);

    // ---------- Textured plane -------------
    filename = "../textures/ice_texture.png";
    TexturedPlaneRenderablePtr texPlane = std::make_shared<TexturedPlaneRenderable>(texShader, filename);
    float planeRotation = (float)M_PI/7.0f;
    parentTransformation = glm::rotate(glm::mat4(1.0), planeRotation, glm::vec3(0, 1, 0));
    parentTransformation = glm::scale(parentTransformation, glm::vec3(300.0,30.0,30.0));
    texPlane->setParentTransform(parentTransformation);
    texPlane->setMaterial(snow);
    viewer.addRenderable(texPlane);
    // We add this textured plane as an obstacle
    glm::vec3 planeNormal(sin(planeRotation), 0, cos(planeRotation));
    glm::vec3 planePoint(0, 0, 0);
    PlanePtr ground = std::make_shared<Plane>(planeNormal, planePoint);
    system->addPlaneObstacle(ground);

    // -------------- Snowman ----------------
    // Movement Mapping
    glm::vec3 px(-5.0, 0.0, 5.0);
    glm::vec3 pv(0.0, 0.0, 0.0);
    float pm = 1.0, pr, pa = 0.0;
    // Particle radius depends on the snowman
    pr = 0.9;
    DynamicSnowmanPtr snowmanMvt = std::make_shared<DynamicSnowman>( px, pv, pm, pr, pa);

    // Shape and textures
    std::string filenameBody = "../textures/snowman_body_texture.png";
    std::string filenameHead = "../textures/snowman_head_texture.png";
    std::string filenameNose = "../textures/snowman_nose_texture.png";
    std::string filenameArm = "../textures/tree_texture.png";
    std::string filenameBaseHat = "../textures/snowman_baseHat_texture.png";
    std::string filenameTopHat = "../textures/snowman_topHat_texture.png";
    std::string filenameSkis = "../textures/ross_hero.png";
    std::string filenameSkiSticks = "../textures/leki_pole.png";
    std::string filenameSnowProjections = "../textures/snow_projections.png";
    TexturedSnowmanRenderablePtr snowman = createSnowman(texShader, filenameBody, filenameHead, filenameNose, filenameArm,
                                                         filenameBaseHat, filenameTopHat, filenameSkis, filenameSkiSticks,
                                                         filenameSnowProjections, snowmanMvt);
    system->addParticle(snowmanMvt);
    snowman->setMaterial(pearl);
    HierarchicalRenderable::addChild(systemRenderable, snowman);

    // Initialize a force field that apply only to the moving snowman
    glm::vec3 nullForce(0.0, 0.0, 0.0);
    std::vector<ParticlePtr> vSnowman;
    vSnowman.push_back(snowmanMvt);

    ConstantForceFieldPtr force = std::make_shared<ConstantForceField>(vSnowman, nullForce);
    system->addForceField(force);
    ControlledForceFieldRenderablePtr forceRenderable = std::make_shared<ControlledForceFieldRenderable>(flatShader, force);
    HierarchicalRenderable::addChild(systemRenderable, forceRenderable);

    // Add a damping force field to the mobile and enable the collisions
    DampingForceFieldPtr dampingForceField = std::make_shared<DampingForceField>(vSnowman, 0.9, 5);
    system->addForceField(dampingForceField);
    system->setCollisionsDetection(true);
    // Initialize the restitution coefficient for collision
    // Snow is soft
    system->setRestitution(0.5f);

    // Add the gravity force field
    ConstantForceFieldPtr gravityForceField = std::make_shared<ConstantForceField>(system->getParticles(), glm::vec3{0,0,-15} );
    system->addForceField(gravityForceField);

    // ----------- Mountains ---------------


    // --------------- Trees ---------------
    // filename = "../textures/tree_texture.png";
    // std::string leafFilename = "../textures/grass_texture.png";
    // TexturedTruncRenderablePtr tronc = createTree(texShader,filename,leafFilename, system);
    // viewer.addRenderable(tronc);


    // ----------- Slalom Gates -------------
    glm::vec3 gx(0.0, -3, 0.0);
    glm::vec3 gv(0.0, 0.0, 0.0);
    float gm = 100.0, gr = 1.0, ga = 0.0;
    glm::vec3 particleOffset(0, 0, 1.0);

    int numberOfGates = 5;
    int spaceBetweenGates = 20;
    MaterialPtr redPlastic = Material::RedPlastic();
    MaterialPtr bluePlastic = Material::BluePlastic();
    std::string filenameSlalomGate = "../textures/slalom_gate.png";


    // Starting gates
    ParticlePtr gateParticle = std::make_shared<Particle>(gx + particleOffset, gv, gm, gr, ga);
    TexturedSlalomGatePtr startingGate = createGate(texShader, filenameSlalomGate, redPlastic, system, gateParticle);
    localTransformation = glm::translate(glm::mat4(1.0), gx);
    startingGate->setParentTransform(localTransformation);
    HierarchicalRenderable::addChild(systemRenderable, startingGate);

    gx = glm::vec3(0.0, 3.0, 0.0);
    ParticlePtr gateParticle2 = std::make_shared<Particle>(gx + particleOffset, gv, gm, gr, ga);
    TexturedSlalomGatePtr startingGate2 = createGate(texShader, filenameSlalomGate, redPlastic, system, gateParticle2);
    parentTransformation = glm::translate(glm::mat4(1.0), gx);
    startingGate2->setParentTransform(parentTransformation);
    HierarchicalRenderable::addChild(systemRenderable, startingGate2);

    TexturedSlalomGatePtr slalomGate;
    MaterialPtr gateMaterial;
    for (int gateNumber = 0; gateNumber < numberOfGates; gateNumber++) {
        if (gateNumber%2 == 1) {
            gx = glm::vec3(spaceBetweenGates*(gateNumber+1), 7.0 + (rand()%10 - 5)/10.0, -tan(planeRotation)*spaceBetweenGates*(gateNumber+1));
            gateMaterial = redPlastic;
        } else {
            gx = glm::vec3(spaceBetweenGates*(gateNumber+1), -7.0 - (rand()%10 - 5)/10.0, -tan(planeRotation)*spaceBetweenGates*(gateNumber+1));
            gateMaterial = bluePlastic;
        }
        gateParticle = std::make_shared<Particle>(gx + particleOffset, gv, gm, gr, ga);
        slalomGate = createGate(texShader, filenameSlalomGate, gateMaterial, system, gateParticle);
        parentTransformation = glm::translate(glm::mat4(1.0), gx);
        slalomGate->setParentTransform(parentTransformation);
        HierarchicalRenderable::addChild(systemRenderable, slalomGate);
    }
    viewer.addRenderable(startingGate);


    // ----------- Camera -------------
    // Position the camera with regards to the skiing snowman, it's then animated
    // using its velocity and position
    viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3(-8, 0, 8 ) + px, px, glm::vec3( 0, 0, 1 ) ) );
    // Control it by the snowman
    viewer.setGuidingRenderable(snowmanMvt);
    viewer.startAnimation();
}
