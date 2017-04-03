#include "../include/Viewer.hpp"
#include "../include/log.hpp"

#include <sstream>

// forward declararion of scene building functions, in separate files
void initialize_snowman_scene(Viewer& viewer);
void initialize_ground_scene(Viewer& viewer);


int main(int argc, char* argv[])
{
    Viewer viewer(1280, 720);   // window size in pixels

    unsigned int scene_to_load = 1;

    for (unsigned int i = 1; i < argc; ++i) {
        std::istringstream tokenizer(std::string(argv[i]));
        tokenizer >> scene_to_load;
        if (tokenizer.fail()) {
            LOG(fatal, "unknown argument " << argv[i] << ". Please launch without argument or the number of the practical you want to load");
            exit(EXIT_FAILURE);
        }
    }
 switch (scene_to_load) {
        case 1:
            initialize_snowman_scene(viewer);
            break;
        case 2:
            initialize_ground_scene(viewer);
            break;
        default:
            LOG(fatal, "unknown practical number " << scene_to_load);
            exit(EXIT_FAILURE);
    }

    while (viewer.isRunning()) {
        viewer.handleEvent();
        viewer.animate();
        viewer.draw();
        viewer.display();
    }

    return EXIT_SUCCESS;
}
