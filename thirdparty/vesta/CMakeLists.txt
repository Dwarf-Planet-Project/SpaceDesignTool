#
# $Revision: 389 $ $Date: 2010-07-26 19:34:58 -0700 (Mon, 26 Jul 2010) $
#
# Copyright by Astos Solutions GmbH, Germany
#
# this file is published under the Astos Solutions Free Public License
# For details on copyright and terms of use see 
# http://www.astos.de/Astos_Solutions_Free_Public_License.html
#
#
# CMake file for building the vesta library

FIND_PACKAGE(SWIG)
INCLUDE(${SWIG_USE_FILE})

FIND_PACKAGE(JNI REQUIRED)
#FIND_PACKAGE(PythonLibs REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)

SET_SOURCE_FILES_PROPERTIES(vesta.swg PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(vesta.swg PROPERTIES SWIG_FLAGS "-includeall")

add_definitions(
     -DEIGEN_USE_NEW_STDVECTOR
     -DGLEW_STATIC
     -DLIB3DSAPI=
)

set (CMAKE_OSX_SYSROOT /Developer/SDKs/MacOSX10.6.sdk)

set (SRCS 
    Arc.cpp
    ArrowGeometry.cpp
    ArrowVisualizer.cpp
    Atmosphere.cpp
    AxesVisualizer.cpp
    Body.cpp
    BodyFixedFrame.cpp
    CelestialCoordinateGrid.cpp
    Chronology.cpp
    ConeGeometry.cpp
    CubeMapFramebuffer.cpp
    DataChunk.cpp
    DDSLoader.cpp
    Debug.cpp
    Entity.cpp
    FixedPointTrajectory.cpp
    FixedRotationModel.cpp
    Framebuffer.cpp
    Geometry.cpp
    HierarchicalTiledMap.cpp
    InertialFrame.cpp
    KeplerianTrajectory.cpp
    LabelGeometry.cpp
    LightingEnvironment.cpp
    LightSource.cpp
    MapLayer.cpp
    MeshGeometry.cpp
    NadirVisualizer.cpp
    Observer.cpp
    OrbitalElements.cpp
    ParticleSystemGeometry.cpp
    PlaneGeometry.cpp
    PlaneVisualizer.cpp
    PrimitiveBatch.cpp
    RenderContext.cpp
    SensorVisualizer.cpp
    ShaderBuilder.cpp
    StarCatalog.cpp
    StarsLayer.cpp
    Submesh.cpp
    TextureFont.cpp
    TextureMap.cpp
    TextureMapLoader.cpp
    TrajectoryGeometry.cpp
    TwoBodyRotatingFrame.cpp
    UniformRotationModel.cpp
    Universe.cpp
    UniverseRenderer.cpp
    VelocityVisualizer.cpp
    VertexArray.cpp
    VertexPool.cpp
    VertexSpec.cpp
    Visualizer.cpp
    WorldGeometry.cpp
    internal/InputDataStream.cpp
    particlesys/ParticleEmitter.cpp
    interaction/ObserverController.cpp
    glhelp/GLShader.cpp
    glhelp/GLShaderProgram.cpp
    glhelp/GLFramebuffer.cpp
    glhelp/GLBufferObject.cpp
    glhelp/GLVertexBuffer.cpp
    glhelp/GLElementBuffer.cpp
)

set (LIB3DSDIR ${VESTA_SOURCE_DIR}/libraries/src/lib3ds )

set (LIB3DS_SRCS 
    ${LIB3DSDIR}/lib3ds_atmosphere.c
    ${LIB3DSDIR}/lib3ds_background.c
    ${LIB3DSDIR}/lib3ds_camera.c
    ${LIB3DSDIR}/lib3ds_chunk.c
    ${LIB3DSDIR}/lib3ds_chunktable.c
    ${LIB3DSDIR}/lib3ds_file.c
    ${LIB3DSDIR}/lib3ds_io.c
    ${LIB3DSDIR}/lib3ds_light.c
    ${LIB3DSDIR}/lib3ds_material.c
    ${LIB3DSDIR}/lib3ds_math.c
    ${LIB3DSDIR}/lib3ds_matrix.c
    ${LIB3DSDIR}/lib3ds_mesh.c
    ${LIB3DSDIR}/lib3ds_node.c
    ${LIB3DSDIR}/lib3ds_quat.c
    ${LIB3DSDIR}/lib3ds_shadow.c
    ${LIB3DSDIR}/lib3ds_track.c
    ${LIB3DSDIR}/lib3ds_util.c
    ${LIB3DSDIR}/lib3ds_vector.c
    ${LIB3DSDIR}/lib3ds_viewport.c
)

set (GLEWDIR ${VESTA_SOURCE_DIR}/libraries/src/glew )

set (GLEW_SRCS
    ${GLEWDIR}/src/glew.c
)

set (CURVEPLOTDIR ${VESTA_SOURCE_DIR}/libraries/src/curveplot )

set (CURVEPLOT_SRCS
    ${CURVEPLOTDIR}/curveplot.cpp
)


INCLUDE_DIRECTORIES (${VESTA_SOURCE_DIR}/libraries/inc)
INCLUDE_DIRECTORIES (${VESTA_SOURCE_DIR}/libraries/src)
INCLUDE_DIRECTORIES (${VESTA_SOURCE_DIR}/libraries/src/glew/include)
INCLUDE_DIRECTORIES (${VESTA_SOURCE_DIR}/libraries/inc/curveplot)

#INCLUDE_DIRECTORIES (${PYTHON_INCLUDE_PATH})
INCLUDE_DIRECTORIES (${JNI_INCLUDE_DIRS})

set (LIBS ${LIBS} ${OPENGL_LIBRARIES})

add_library (vestacore STATIC ${SRCS} ${LIB3DS_SRCS} ${GLEW_SRCS} ${CURVEPLOT_SRCS})
target_link_libraries(vestacore ${LIBS})

set (CMAKE_SWIG_OUTDIR wrappers)
#SWIG_ADD_MODULE(vesta python vesta.swg vesta_wrap.cxx)
SWIG_ADD_MODULE(vesta java vesta.swg vesta_wrap.cxx)
SWIG_LINK_LIBRARIES(vesta vestacore
# ${PYTHON_LIBRARIES}
)