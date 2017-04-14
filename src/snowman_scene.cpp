#include "../include/ShaderProgram.hpp"
#include "../include/Viewer.hpp"
#include "../include/FrameRenderable.hpp"
#include "../include/lighting/Lights.hpp"

#include "../include/texturing/TexturedPlaneRenderable.hpp"
#include "../include/texturing/TexturedCubeRenderable.hpp"
#include "../include/texturing/MultiTexturedCubeRenderable.hpp"
#include "../include/texturing/MipMapCubeRenderable.hpp"
#include "../include/texturing/TexturedMeshRenderable.hpp"
#include "../include/texturing/TexturedSnowmanRenderable.hpp"
#include "../include/texturing/TexturedSlalomGate.hpp"
#include "../include/texturing/TexturedSphereRenderable.hpp"
#include "../include/texturing/TexturedTruncRenderable.hpp"
#include "../include/texturing/TexturedGroundRenderable.hpp"
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

#include "../include/keyframes/KeyframedMeshRenderable.hpp"
#include "../include/keyframes/GeometricTransformation.hpp"


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
    // glm::vec3 d_direction = glm::normalize(glm::vec3(-10.0,-1.5,-1.0));

    glm::vec3 d_direction = glm::normalize(glm::vec3(-1.0,-1.0,0.0));
    glm::vec3 d_ambient(2,2,2), d_diffuse(1.0,1.0,1.0), d_specular(1.0,1.0,1.0);
    DirectionalLightPtr directionalLight = std::make_shared<DirectionalLight>(d_direction, d_ambient, d_diffuse, d_specular);
    viewer.setDirectionalLight(directionalLight);


    // ---------- Textured plane -------------
    filename = "../textures/ice_texture.png";
    TexturedPlaneRenderablePtr texPlaneEnd = std::make_shared<TexturedPlaneRenderable>(texShader, filename);
    parentTransformation = glm::translate(glm::mat4(1.0), glm::vec3(400.0,0,-200.0));
    parentTransformation = glm::scale(parentTransformation, glm::vec3(1000.0,1000.0,1.0));
    texPlaneEnd->setParentTransform(parentTransformation);
    texPlaneEnd->setMaterial(snow);
    viewer.addRenderable(texPlaneEnd);
    // We add this textured plane as an obstacle
    glm::vec3 planeNormal(0, 0, 1);
    glm::vec3 planePoint(800, 0, -200);
    PlanePtr groundEnd = std::make_shared<Plane>(planeNormal, planePoint);
    system->addPlaneObstacle(groundEnd);


    filename = "../textures/ice_texture.png";
    TexturedPlaneRenderablePtr texPlane = std::make_shared<TexturedPlaneRenderable>(texShader, filename);
    float planeRotation = (float)M_PI/7.0f;
    parentTransformation = glm::rotate(glm::mat4(1.0), planeRotation, glm::vec3(0, 1, 0));
    parentTransformation = glm::scale(parentTransformation, glm::vec3(1000.0,1000.0,1.0));
    texPlane->setParentTransform(parentTransformation);
    texPlane->setMaterial(snow);
    viewer.addRenderable(texPlane);
    // We add this textured plane as an obstacle
    planeNormal = glm::vec3(sin(planeRotation), 0, cos(planeRotation));
    planePoint = glm::vec3(0, 0, 0);
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
    filename = "../textures/ice_texture.png";
    TexturedGroundRenderablePtr texGroundLeft = std::make_shared<TexturedGroundRenderable>(texShader, filename);
    float planeRotationLeft = (float)M_PI/7.0f;
    parentTransformation = glm::rotate(glm::mat4(1.0), planeRotationLeft, glm::vec3(0.0, 1.0, 0.0));
    parentTransformation = glm::translate(parentTransformation, glm::vec3(250.0,-50.0,sin(planeRotationLeft)*25.0));
    parentTransformation = glm::rotate(parentTransformation, planeRotationLeft, glm::vec3(-1, 0.0, 0.0));
    texGroundLeft->setParentTransform(parentTransformation);
    texGroundLeft->setMaterial(snow);
    viewer.addRenderable(texGroundLeft);

    TexturedGroundRenderablePtr texGroundRight = std::make_shared<TexturedGroundRenderable>(texShader, filename);
    float planeRotationRight = (float)M_PI/7.0f;
    parentTransformation = glm::rotate(glm::mat4(1.0), planeRotationRight, glm::vec3(0.0, 1.0, 0.0));
    parentTransformation = glm::translate(parentTransformation, glm::vec3(250.0,50.0,sin(planeRotationRight)*25.0));
    parentTransformation = glm::rotate(parentTransformation, planeRotationRight, glm::vec3(1.0, 0.0, 0.0));
    texGroundRight->setParentTransform(parentTransformation);
    texGroundRight->setMaterial(snow);
    viewer.addRenderable(texGroundRight);

    // --------------- Trees ---------------
    for (int k=0; k<40; k++){
        int x = rand()%500;
        int y = rand()%80;
        glm::vec3 tv(0.0, 0.0, 0.0);
        float tm = 1.0, tr = 2.5, ta = 0.0;
        glm::vec3 treeOffset(0, 0, 1.0);
        glm::vec3 treePosition;
        if(y<40){
          treePosition = glm::vec3(x, y+28,y*tan(planeRotationRight+0.05)- x*tan(planeRotation));//+ texGroundRight->getMatriceElevation(x,y)) ;
        }else{
          treePosition = glm::vec3(x,-y-28+40,(y-40)*tan(planeRotationRight+0.05)- x*tan(planeRotation));
        }

        filename = "../textures/tree_texture.png";
        std::string leafFilename = "../textures/grass_texture2.png";
        ParticlePtr treeParticle = std::make_shared<Particle>(treePosition + treeOffset, tv, tm, tr, ta);
        TexturedTruncRenderablePtr tronc = createTree(texShader, filename, leafFilename, system, treeParticle);
        parentTransformation = glm::translate(glm::mat4(1.0), treePosition);
        parentTransformation = glm::scale(parentTransformation, glm::vec3(0.3, 0.3, 0.4));
        tronc->setParentTransform(parentTransformation);
        viewer.addRenderable(tronc);
    }


    // -------------- Skybox ---------------
    parentTransformation = glm::mat4(1.0);
    filename = "../textures/sky.png";
    TexturedSphereRenderablePtr skybox = std::make_shared<TexturedSphereRenderable>(texShader,filename);
    localTransformation = glm::scale(skybox->getModelMatrix(), glm::vec3(500.0,500.0,500.0));
    skybox->setLocalTransform(localTransformation);
    skybox->setParentTransform(parentTransformation);
    skybox->setMaterial(pearl);
    viewer.addRenderable(skybox);

    // ----------- Slalom Gates -------------
    glm::vec3 gx(0.0, -3, 0.0);
    glm::vec3 gv(0.0, 0.0, 0.0);
    float gm = 100.0, gr = 1.0, ga = 0.0;
    glm::vec3 particleOffset(0, 0, 1.0);

    int numberOfGates = 15;
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
    float aleat;
    for (int gateNumber = 0; gateNumber < numberOfGates; gateNumber++) {
        if (gateNumber%2 == 1) {
            aleat = (rand()%10 - 5)/10.0;
            gx = glm::vec3(spaceBetweenGates*(gateNumber+1)+aleat, 7.0 + (rand()%40 - 20)/10.0, -tan(planeRotation)*(spaceBetweenGates*(gateNumber+1)+aleat));
            gateMaterial = redPlastic;
        } else {
            gx = glm::vec3(spaceBetweenGates*(gateNumber+1)+aleat, -7.0 + (rand()%40 - 20)/10.0, -tan(planeRotation)*(spaceBetweenGates*(gateNumber+1)+aleat));
            gateMaterial = bluePlastic;
        }
        gateParticle = std::make_shared<Particle>(gx + particleOffset, gv, gm, gr, ga);
        slalomGate = createGate(texShader, filenameSlalomGate, gateMaterial, system, gateParticle);
        parentTransformation = glm::translate(glm::mat4(1.0), gx);
        slalomGate->setParentTransform(parentTransformation);
        HierarchicalRenderable::addChild(systemRenderable, slalomGate);
    }
    viewer.addRenderable(startingGate);

    // ----------- Finish Flag --------------
    std::string filenameFinishGate = "../textures/finish_gate.png";
    gateParticle = std::make_shared<Particle>(glm::vec3(330.0,13.0,-tan(planeRotation)*330), gv, gm, gr, ga);
    TexturedSlalomGatePtr finish = createGate(texShader, filenameFinishGate, pearl, system, gateParticle);
    parentTransformation = glm::scale(glm::translate(glm::mat4(1.0), glm::vec3(330.0,0.0,-tan(planeRotation)*330)), glm::vec3(1.0,18.0,8.0));
    finish->setParentTransform(parentTransformation);
    HierarchicalRenderable::addChild(systemRenderable, finish);

    // Flocons sur piste
//    int nombreDeFlocons = 500;
//    float x;
//    float y;
//    for (int i = 0; i < nombreDeFlocons; i++) {
//        x = (rand()%700 - 100);
//        y = (rand()%20 - 10);
//        glm::vec3 fx(x, y, -tan(planeRotation)*x + 10);
//        glm::vec3 fv(0.0, 0.0, 0.0);
//        float fm = 10, fr = 0.3, fa = 0.0;
//        ParticlePtr part = std::make_shared<Particle>(fx, fv, fm, fr, fa);
//        part->setFixed(false);
//        ParticleRenderablePtr partRend = std::make_shared<ParticleRenderable>(texShader, part);
//        parentTransformation = glm::translate(glm::mat4(1.0), fx);
//        partRend->setParentTransform(parentTransformation);
//        HierarchicalRenderable::addChild(systemRenderable, partRend);
//    }


    // ----------- Airplane ----------
    std::string filenameAirplaneMesh = "../meshes/airplane.obj";
    std::string filenameAirplaneTexture = "../textures/airplane.png";
    auto airplane = std::make_shared<KeyframedMeshRenderable>(texShader, Material::Pearl(), filenameAirplaneMesh, filenameAirplaneTexture);
    float heightInSky = -20;
    glm::vec3 center(120.0 , 0.0, heightInSky);
    float trajRadius = 30;
    float paperangle =(float)M_PI/2.0f;
    airplane->setParentTransform(glm::mat4(1.0));
    airplane->setMaterial(pearl);
    localTransformation = glm::rotate(glm::mat4(1.0), paperangle, glm::vec3(1.0, 0.0, 0.0));
    airplane->setLocalTransform(localTransformation*glm::scale(glm::mat4(1.0), glm::vec3(3, 3, 3)));

    //Keyframes on parent transformation: pairs of (time, transformation)
    airplane->addParentTransformKeyframe(0.0, GeometricTransformation(center + glm::vec3(1.0, 0.0, 0.0)*trajRadius, glm::angleAxis((float) (0), glm::vec3(0.0, 0.0, 1.0))));
    airplane->addParentTransformKeyframe(1.0, GeometricTransformation(center + glm::vec3(0.7, 0.7, 0.0)*trajRadius, glm::angleAxis((float) (M_PI/4.0f), glm::vec3(0.0, 0.0, 1.0))));
    airplane->addParentTransformKeyframe(2.0, GeometricTransformation(center + glm::vec3(0.0, 1, 0.0)*trajRadius, glm::angleAxis((float) (M_PI/2.0f), glm::vec3(0.0, 0.0, 1.0))));
    airplane->addParentTransformKeyframe(3.0, GeometricTransformation(center + glm::vec3(-0.7, 0.7, 0.0)*trajRadius, glm::angleAxis((float) (3*M_PI/4.0f), glm::vec3(0.0, 0.0, 1.0))));
    airplane->addParentTransformKeyframe(4.0, GeometricTransformation(center + glm::vec3(-1, 0, 0.0)*trajRadius, glm::angleAxis((float) (M_PI), glm::vec3(0.0, 0.0, 1.0))));
    airplane->addParentTransformKeyframe(5.0, GeometricTransformation(center + glm::vec3(-0.7, -0.7, 0.0)*trajRadius, glm::angleAxis((float) (5*M_PI/4.0f), glm::vec3(0.0, 0.0, 1.0))));
    airplane->addParentTransformKeyframe(6.0, GeometricTransformation(center + glm::vec3(0, -1, 0.0)*trajRadius, glm::angleAxis((float) (3*M_PI/2.0f), glm::vec3(0.0, 0.0, 1.0))));
    airplane->addParentTransformKeyframe(7.0, GeometricTransformation(center + glm::vec3(0.7, -0.7, 0.0)*trajRadius, glm::angleAxis((float) (7*M_PI/4.0f), glm::vec3(0.0, 0.0, 1.0))));
    airplane->addParentTransformKeyframe(8.0, GeometricTransformation(center + glm::vec3(1, 0, 0.0)*trajRadius, glm::angleAxis((float) (2*M_PI), glm::vec3(0.0, 0.0, 1.0))));
    viewer.addRenderable(airplane);



    // ---------- Boundaries ---------
    planeNormal = glm::vec3(0, 1, 0);
    planePoint = glm::vec3(0, -500, 0);
    PlanePtr wall1 = std::make_shared<Plane>(planeNormal, planePoint);
    system->addPlaneObstacle(wall1);

    planeNormal = glm::vec3(1, 0, 0);
    planePoint = glm::vec3(-500, 0, 0);
    PlanePtr wall2 = std::make_shared<Plane>(planeNormal, planePoint);
    system->addPlaneObstacle(wall2);

    planeNormal = glm::vec3(0, -1, 0);
    planePoint = glm::vec3(0, 500, 0);
    PlanePtr wall3 = std::make_shared<Plane>(planeNormal, planePoint);
    system->addPlaneObstacle(wall3);

    planeNormal = glm::vec3(-1, 0, 0);
    planePoint = glm::vec3(500, 0, 0);
    PlanePtr wall4 = std::make_shared<Plane>(planeNormal, planePoint);
    system->addPlaneObstacle(wall4);



    // ----------- Camera -------------
    // Position the camera with regards to the skiing snowman, it's then animated
    // using its velocity and position
    viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3(-8, 0, 8 ) + px, px, glm::vec3( 0, 0, 1 ) ) );
    // Control it by the snowman
    viewer.setGuidingRenderable(snowmanMvt);
    viewer.setAnimationLoop(true, 8.0);
    viewer.startAnimation();
}
