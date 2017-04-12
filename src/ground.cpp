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

void initinlizeMatriceElevation(float mat[][100])
{
  for (int i=0; i<100; i++){
    for (int j=0; j<100; j++){
      mat[i][j] = 100 -j;
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

void recursive_Elevation(float matriceElevation[][100], int x, int y, int decalage, int profondeur )
{
  if (profondeur == 0 ){
    return;
  }
  std::cout << std::endl << "On essaye de faire du bruit en " << x << ", " << y << " avec une profondeur de " << profondeur << std::endl;
  matriceElevation[x][y]=Get2DPerlinNoiseValue((float)x,(float)y, profondeur+1)*100;
  std::cout << std::endl << "Le bruit vaut " << matriceElevation[x][y]<< std::endl;
  recursive_Elevation(matriceElevation,x/2,y/2,decalage/2,profondeur-1);
  recursive_Elevation(matriceElevation,x/2+decalage,y/2,decalage/2,profondeur-1);
  recursive_Elevation(matriceElevation,x/2,y/2+decalage,decalage/2,profondeur-1);
  recursive_Elevation(matriceElevation,x/2+decalage,y/2+decalage,decalage/2,profondeur-1);

}



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

    //Create the elvation matrice of the ground
    float matriceElevation[100][100];
    initinlizeMatriceElevation(matriceElevation);
    std::cout<< std::endl << "on arrive à initialiser notre matrice" << std::endl << std::endl;
    recursive_Elevation(matriceElevation, 50, 50, 25, 3);
    std::cout << std::endl << "on arrive à faire notre bruit de perrain" << std::endl << std::endl;

    //Textured plane
    filename = "../textures/ice_texture.png";
    TexturedPlaneRenderablePtr texPlane = std::make_shared<TexturedPlaneRenderable>(texShader, filename);
    for (int i=0; i<100; i++){
      for (int j=0; j<100; j++){
        std::cout << std::endl << "On crér notre terrain en " << i << ", " << j << ", "  << matriceElevation[i][j]<< std::endl << std::endl;
        //parentTransformation = glm::scale(glm::mat4(1.0), glm::vec3(i,j,matriceElevation[i][j]));
        texPlane->setParentTransform(parentTransformation);
        texPlane->setMaterial(pearl);
        viewer.addRenderable(texPlane);
      }
    }
    std::cout << std::endl << "on arrive à afficher le tout" << std::endl << std::endl;

    viewer.startAnimation();
}
