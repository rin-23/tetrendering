set(TYRO_INC_LOCAL 
# "minityro/Engine/RAAxisAlignedBBox.h" 
# "minityro/Engine/RADepthProxy.h" 
# "minityro/Engine/RAES2TextOverlay.h" 
# "minityro/Engine/RAHitProxy.h" 
# "minityro/Engine/RANormalMapProxy.h" 
# "minityro/Engine/RAOverlayManager.h" 
# "minityro/Engine/load_mesh_sequence.h"
#"Engine/RAPointOctree.h" 
"minityro/Engine/TyroWindow.h"
#"Engine/TyroApp.h"
# "minityro/Engine/Console.h"
# "minityro/Engine/stb_image_write.h"
# "minityro/Engine/stb_image.h"
"minityro/Engine/TyroIGLMesh.h"
#"Engine/stop_motion_data.h"
#"Engine/TyroTimeLine.h"


#"Engine/stbi_DDS_aug.h"
#"Engine/stbi_DDS_aug_c.h"
)

set(TYRO_SRC_LOCAL 
# "minityro/Engine/load_mesh_sequence.cpp"
# "minityro/Engine/RAAxisAlignedBBox.cpp" 
# "minityro/Engine/RADepthProxy.cpp" 
# "minityro/Engine/RAES2TextOverlay.cpp" 
# "minityro/Engine/RAHitProxy.cpp" 
# "minityro/Engine/RANormalMapProxy.cpp" 
# "minityro/Engine/RAOverlayManager.cpp" 
#"Engine/RAPointOctree.cpp"
"minityro/Engine/TyroWindow.cpp" 
#"Engine/TyroApp.cpp"
# "minityro/Engine/Console.cpp"
# "minityro/Engine/stb_image_write.c"
# "minityro/Engine/stb_image.c"
"minityro/Engine/TyroIGLMesh.cpp"
#"Engine/stop_motion_data.cpp"
#"minityro/Engine/TyroTimeLine.cpp"
)

set(TYRO_ENGINE_SRC
${TYRO_SRC_LOCAL}
)