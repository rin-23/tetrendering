set(TYRO_INC_ES2 
"minityro/Core/ES2/RAES2Context.h" 
"minityro/Core/ES2/RAES2CoreVisualEffects.h" 
"minityro/Core/ES2/RAES2IndexHardwareBuffer.h" 
"minityro/Core/ES2/RAES2OffScreenBuffer.h" 
"minityro/Core/ES2/RAES2Polyline.h" 
"minityro/Core/ES2/RAES2Polypoint.h" 
"minityro/Core/ES2/RAES2Renderable.h" 
"minityro/Core/ES2/RAES2Renderer.h" 
"minityro/Core/ES2/RAES2ShaderProgram.h" 
"minityro/Core/ES2/RAES2ShaderUniforms.h" 
"minityro/Core/ES2/RAES2Texture2D.h" 
"minityro/Core/ES2/RAES2TriMesh.h" 
"minityro/Core/ES2/RAES2VertexArray.h" 
"minityro/Core/ES2/RAES2VertexFormat.h" 
"minityro/Core/ES2/RAES2VertexHardwareBuffer.h" 
"minityro/Core/ES2/RAES2VisualEffect.h" 
"minityro/Core/ES2/RAES2VisualStates.h" 
"minityro/Core/ES2/RAES2BufferTexture.h" 
)

set(TYRO_SRC_ES2 
"minityro/Core/ES2/RAES2Context.cpp" 
"minityro/Core/ES2/RAES2CoreVisualEffects.cpp" 
"minityro/Core/ES2/RAES2IndexHardwareBuffer.cpp" 
"minityro/Core/ES2/RAES2OffScreenBuffer.cpp" 
"minityro/Core/ES2/RAES2Polyline.cpp" 
"minityro/Core/ES2/RAES2Polypoint.cpp" 
"minityro/Core/ES2/RAES2Renderable.cpp" 
"minityro/Core/ES2/RAES2Renderer.cpp" 
"minityro/Core/ES2/RAES2ShaderProgram.cpp" 
"minityro/Core/ES2/RAES2ShaderUniforms.cpp" 
"minityro/Core/ES2/RAES2Texture2D.cpp" 
"minityro/Core/ES2/RAES2TriMesh.cpp" 
"minityro/Core/ES2/RAES2VertexArray.cpp" 
"minityro/Core/ES2/RAES2VertexFormat.cpp" 
"minityro/Core/ES2/RAES2VertexHardwareBuffer.cpp" 
"minityro/Core/ES2/RAES2VisualEffect.cpp" 
"minityro/Core/ES2/RAES2VisualStates.cpp" 
"minityro/Core/ES2/RAES2BufferTexture.cpp" 
)

set(TYRO_INC_RENDERABLES 
# "minityro/Core/Renderables/RAES2BBox.h" 
# "minityro/Core/Renderables/RAES2DefaultPolyline.h" 
# "minityro/Core/Renderables/RAES2DefaultTriMesh.h" 
#"minityro/Core/Renderables/RAES2Octree.h" 
# "minityro/Core/Renderables/RAES2STLMesh.h"
# "minityro/Core/Renderables/RAES2StandardMesh.h" 
)

set(TYRO_SRC_RENDERABLES 
# "minityro/Core/Renderables/RAES2BBox.cpp" 
# "minityro/Core/Renderables/RAES2DefaultPolyline.cpp" 
# "minityro/Core/Renderables/RAES2DefaultTriMesh.cpp" 
#"minityro/Core/Renderables/RAES2Octree.cpp" 
# "minityro/Core/Renderables/RAES2STLMesh.cpp" 
# "minityro/Core/Renderables/RAES2StandardMesh.cpp" 
)

set(TYRO_INC_SCENE 
# "minityro/Core/Scene/RANode.h" 
"minityro/Core/Scene/RASpatial.h" 
)

set(TYRO_SRC_SCENE 
# "minityro/Core/Scene/RANode.cpp" 
"minityro/Core/Scene/RASpatial.cpp" 
)

# set(TYRO_INC_TOOLS 
# "minityro/Core/Tools/RASTLExporter.h" 
# "minityro/Core/Tools/RASTLLoader.h" 
# )

# set(TYRO_SRC_TOOLS 
# "minityro/Core/Tools/RASTLExporter.cpp" 
# "minityro/Core/Tools/RASTLLoader.cpp" 
# )

set(TYRO_INC_LOCAL 
"minityro/Core/GLStructures.h" 
"minityro/Core/RACamera.h" 
#minityro/"Core/RACameraOld.h" 
"minityro/Core/RAFileManager.h" 
# "minityro/Core/RAFont.h" 
"minityro/Core/RAHardwareBuffer.h" 
"minityro/Core/RAIndexHardwareBuffer.h" 
"minityro/Core/RALogManager.h" 
"minityro/Core/RAMath.h" 
# "minityro/Core/RASkeleton.h" 
"minityro/Core/RATexture.h" 
"minityro/Core/RATextureBuffer.h" 
# "minityro/Core/RATouchable.h" 
#minityro/"Core/RATriOctree.h" 
"minityro/Core/RAVertexBufferAccessor.h" 
"minityro/Core/RAVertexHardwareBuffer.h" 
"minityro/Core/RAVisibleSet.h" 
"minityro/Core/RAiOSCamera.h" 
)

set(TYRO_SRC_LOCAL 
"minityro/Core/RACamera.cpp" 
#minityro/"Core/RACameraOld.cpp" 
"minityro/Core/RAFileManager.cpp" 
# "minityro/Core/RAFont.cpp" 
"minityro/Core/RAHardwareBuffer.cpp" 
"minityro/Core/RAIndexHardwareBuffer.cpp" 
"minityro/Core/RALogManager.cpp" 
"minityro/Core/RAMath.cpp" 
# "minityro/Core/RASkeleton.cpp" 
"minityro/Core/RATexture.cpp" 
"minityro/Core/RATextureBuffer.cpp" 
# "minityro/Core/RATouchable.cpp" 
#minityro/"Core/RATriOctree.cpp" 
"minityro/Core/RAVertexBufferAccessor.cpp" 
"minityro/Core/RAVertexHardwareBuffer.cpp" 
"minityro/Core/RAVisibleSet.cpp" 
"minityro/Core/RAiOSCamera.cpp" 
)

set(TYRO_CORE_SRC
${TYRO_SRC_ES2}
# ${TYRO_SRC_RENDERABLES}
${TYRO_SRC_SCENE}
# ${TYRO_SRC_TOOLS}
${TYRO_SRC_LOCAL}
)