TEMPLATE = lib

CONFIG += static

TARGET = vesta

DESTDIR = build

VESTA_PATH = .
LIB3DS_PATH = ../lib3ds
GLEW_PATH = ../glew

VESTA_SOURCES = \
    $$VESTA_PATH/Arc.cpp \
    $$VESTA_PATH/ArrowGeometry.cpp \
    $$VESTA_PATH/ArrowVisualizer.cpp \
    $$VESTA_PATH/Atmosphere.cpp \
    $$VESTA_PATH/AxesVisualizer.cpp \
    $$VESTA_PATH/BillboardGeometry.cpp \
    $$VESTA_PATH/Body.cpp \
    $$VESTA_PATH/BodyFixedFrame.cpp \
    $$VESTA_PATH/CelestialCoordinateGrid.cpp \
    $$VESTA_PATH/Chronology.cpp \
    $$VESTA_PATH/ConeGeometry.cpp \
    $$VESTA_PATH/ConstellationsLayer.cpp \
    $$VESTA_PATH/CubeMapFramebuffer.cpp \
    $$VESTA_PATH/DataChunk.cpp \
    $$VESTA_PATH/DDSLoader.cpp \
    $$VESTA_PATH/Debug.cpp \
    $$VESTA_PATH/Entity.cpp \
    $$VESTA_PATH/FixedPointTrajectory.cpp \
    $$VESTA_PATH/FixedRotationModel.cpp \
    $$VESTA_PATH/Framebuffer.cpp \
    $$VESTA_PATH/Geometry.cpp \
    $$VESTA_PATH/HierarchicalTiledMap.cpp \
    $$VESTA_PATH/InertialFrame.cpp \
    $$VESTA_PATH/KeplerianTrajectory.cpp \
    $$VESTA_PATH/LabelGeometry.cpp \
    $$VESTA_PATH/LightSource.cpp \
    $$VESTA_PATH/MapLayer.cpp \
    $$VESTA_PATH/MeshGeometry.cpp \
    $$VESTA_PATH/NadirVisualizer.cpp \
    $$VESTA_PATH/Observer.cpp \
    $$VESTA_PATH/OrbitalElements.cpp \
    $$VESTA_PATH/ParticleSystemGeometry.cpp \
    $$VESTA_PATH/PlaneGeometry.cpp \
    $$VESTA_PATH/PlaneVisualizer.cpp \
    $$VESTA_PATH/PrimitiveBatch.cpp \
    $$VESTA_PATH/RenderContext.cpp \
    $$VESTA_PATH/LightingEnvironment.cpp \
    $$VESTA_PATH/SensorVisualizer.cpp \
    $$VESTA_PATH/ShaderBuilder.cpp \
    $$VESTA_PATH/Spectrum.cpp \
    $$VESTA_PATH/StarCatalog.cpp \
    $$VESTA_PATH/StarsLayer.cpp \
    $$VESTA_PATH/Submesh.cpp \
    $$VESTA_PATH/TextureFont.cpp \
    $$VESTA_PATH/TextureMap.cpp \
    $$VESTA_PATH/TextureMapLoader.cpp \
    $$VESTA_PATH/TrajectoryGeometry.cpp \
    $$VESTA_PATH/TwoBodyRotatingFrame.cpp \
    $$VESTA_PATH/UniformRotationModel.cpp \
    $$VESTA_PATH/Universe.cpp \
    $$VESTA_PATH/UniverseRenderer.cpp \
    $$VESTA_PATH/VelocityVisualizer.cpp \
    $$VESTA_PATH/VertexArray.cpp \
    $$VESTA_PATH/VertexPool.cpp \
    $$VESTA_PATH/VertexSpec.cpp \
    $$VESTA_PATH/Visualizer.cpp \
    $$VESTA_PATH/WorldGeometry.cpp \
    $$VESTA_PATH/interaction/ObserverController.cpp \
    $$VESTA_PATH/internal/InputDataStream.cpp

VESTA_HEADERS = \
    $$VESTA_PATH/Arc.h \
    $$VESTA_PATH/ArrowGeometry.h \
    $$VESTA_PATH/ArrowVisualizer.h \
    $$VESTA_PATH/Atmosphere.h \
    $$VESTA_PATH/AxesVisualizer.h \
    $$VESTA_PATH/BillboardGeometry.h \
    $$VESTA_PATH/Body.h \
    $$VESTA_PATH/BodyFixedFrame.h \
    $$VESTA_PATH/BoundingBox.h \
    $$VESTA_PATH/BoundingSphere.h \
    $$VESTA_PATH/CelestialCoordinateGrid.h \
    $$VESTA_PATH/Chronology.h \
    $$VESTA_PATH/ConeGeometry.h \
    $$VESTA_PATH/ConstellationsLayer.h \
    $$VESTA_PATH/CubeMapFramebuffer.h \
    $$VESTA_PATH/DataChunk.h \
    $$VESTA_PATH/Debug.h \
    $$VESTA_PATH/DDSLoader.h \
    $$VESTA_PATH/Entity.h \
    $$VESTA_PATH/Frame.h \
    $$VESTA_PATH/Framebuffer.h \
    $$VESTA_PATH/FixedPointTrajectory.h \
    $$VESTA_PATH/FixedRotationModel.h \
    $$VESTA_PATH/Geometry.h \
    $$VESTA_PATH/HierarchicalTiledMap.h \
    $$VESTA_PATH/InertialFrame.h \
    $$VESTA_PATH/IntegerTypes.h \
    $$VESTA_PATH/Intersect.h \
    $$VESTA_PATH/JavaCallbackTrajectory.h \
    $$VESTA_PATH/KeplerianTrajectory.h \
    $$VESTA_PATH/LabelGeometry.h \
    $$VESTA_PATH/LightSource.h \
    $$VESTA_PATH/MapLayer.h \
    $$VESTA_PATH/Material.h \
    $$VESTA_PATH/MeshGeometry.h \
    $$VESTA_PATH/NadirVisualizer.h \
    $$VESTA_PATH/Object.h \
    $$VESTA_PATH/Observer.h \
    $$VESTA_PATH/OGLHeaders.h \
    $$VESTA_PATH/OrbitalElements.h \
    $$VESTA_PATH/ParticleSystemGeometry.h \
    $$VESTA_PATH/PickResult.h \
    $$VESTA_PATH/PlaneGeometry.h \
    $$VESTA_PATH/PlaneVisualizer.h \
    $$VESTA_PATH/PrimitiveBatch.h \
    $$VESTA_PATH/RenderContext.h \
    $$VESTA_PATH/LightingEnvironment.h \
    $$VESTA_PATH/RotationModel.h \
    $$VESTA_PATH/SensorVisualizer.h \
    $$VESTA_PATH/ShaderBuilder.h \
    $$VESTA_PATH/ShaderInfo.h \
    $$VESTA_PATH/SingleTextureTiledMap.h \
    $$VESTA_PATH/SkyLayer.h \
    $$VESTA_PATH/Spectrum.h \
    $$VESTA_PATH/StarCatalog.h \
    $$VESTA_PATH/StarsLayer.h \
    $$VESTA_PATH/StateVector.h \
    $$VESTA_PATH/Submesh.h \
    $$VESTA_PATH/TextureFont.h \
    $$VESTA_PATH/TextureMap.h \
    $$VESTA_PATH/TextureMapLoader.h \
    $$VESTA_PATH/TiledMap.h \
    $$VESTA_PATH/Trajectory.h \
    $$VESTA_PATH/TrajectoryGeometry.h \
    $$VESTA_PATH/TwoBodyRotatingFrame.h \
    $$VESTA_PATH/UniformRotationModel.h \
    $$VESTA_PATH/Units.h \
    $$VESTA_PATH/Universe.h \
    $$VESTA_PATH/UniverseRenderer.h \
    $$VESTA_PATH/VelocityVisualizer.h \
    $$VESTA_PATH/VertexArray.h \
    $$VESTA_PATH/VertexAttribute.h \
    $$VESTA_PATH/VertexPool.h \
    $$VESTA_PATH/VertexSpec.h \
    $$VESTA_PATH/Viewport.h \
    $$VESTA_PATH/Visualizer.h \
    $$VESTA_PATH/WorldGeometry.h \
    $$VESTA_PATH/interaction/ObserverController.h \
    $$VESTA_PATH/internal/InputDataStream.h


### particle system module ###

VESTA_SOURCES += \
    $$VESTA_PATH/particlesys/ParticleEmitter.cpp

VESTA_HEADERS += \
    $$VESTA_PATH/particlesys/ParticleEmitter.h \
    $$VESTA_PATH/particlesys/ParticleRenderer.h \
    $$VESTA_PATH/particlesys/PseudorandomGenerator.h \
    $$VESTA_PATH/particlesys/InitialStateGenerator.h \
    $$VESTA_PATH/particlesys/BoxGenerator.h \
    $$VESTA_PATH/particlesys/DiscGenerator.h \
    $$VESTA_PATH/particlesys/PointGenerator.h


### glhelp module ###

VESTA_SOURCES += \
    $$VESTA_PATH/glhelp/GLFramebuffer.cpp \
    $$VESTA_PATH/glhelp/GLShader.cpp \
    $$VESTA_PATH/glhelp/GLShaderProgram.cpp \
    $$VESTA_PATH/glhelp/GLBufferObject.cpp \
    $$VESTA_PATH/glhelp/GLElementBuffer.cpp \
    $$VESTA_PATH/glhelp/GLVertexBuffer.cpp

VESTA_HEADERS += \
    $$VESTA_PATH/glhelp/GLFramebuffer.h \
    $$VESTA_PATH/glhelp/GLShader.h \
    $$VESTA_PATH/glhelp/GLShaderProgram.h \
    $$VESTA_PATH/glhelp/GLBufferObject.h \
    $$VESTA_PATH/glhelp/GLElementBuffer.h \
    $$VESTA_PATH/glhelp/GLVertexBuffer.h


### lib3ds ###

LIB3DS_SOURCES = \
    $$LIB3DS_PATH/lib3ds_atmosphere.c \
    $$LIB3DS_PATH/lib3ds_background.c \
    $$LIB3DS_PATH/lib3ds_camera.c \
    $$LIB3DS_PATH/lib3ds_chunk.c \
    $$LIB3DS_PATH/lib3ds_chunktable.c \
    $$LIB3DS_PATH/lib3ds_file.c \
    $$LIB3DS_PATH/lib3ds_io.c \
    $$LIB3DS_PATH/lib3ds_light.c \
    $$LIB3DS_PATH/lib3ds_material.c \
    $$LIB3DS_PATH/lib3ds_math.c \
    $$LIB3DS_PATH/lib3ds_matrix.c \
    $$LIB3DS_PATH/lib3ds_mesh.c \
    $$LIB3DS_PATH/lib3ds_node.c \
    $$LIB3DS_PATH/lib3ds_quat.c \
    $$LIB3DS_PATH/lib3ds_shadow.c \
    $$LIB3DS_PATH/lib3ds_track.c \
    $$LIB3DS_PATH/lib3ds_util.c \
    $$LIB3DS_PATH/lib3ds_vector.c \
    $$LIB3DS_PATH/lib3ds_viewport.c

LIB3DS_HEADERS = \
    $$LIB3DS_PATH/lib3ds.h \
    $$LIB3DS_PATH/lib3ds_impl.h


### GL extension wrangler ###

GLEW_SOURCES = \
    $$GLEW_PATH/src/glew.c

GLEW_HEADERS = \
    $$GLEW_PATH/include/GL/glew.h \
    $$GLEW_PATH/include/GL/glxew.h \
    $$GLEW_PATH/include/GL/wglew.h

DEFINES += GLEW_STATIC


### CurvePlot sources ###

CURVEPLOT_SOURCES = \
    ../curveplot/curveplot.cpp

CURVEPLOT_HEADERS = \
    ../curveplot/curveplot.h


SOURCES = \
    $$VESTA_SOURCES \
    $$LIB3DS_SOURCES \
    $$GLEW_SOURCES \
    $$CURVEPLOT_SOURCES

HEADERS = \
    $$VESTA_HEADERS \
    $$LIB3DS_HEADERS \
    $$GLEW_HEADERS \
    $$CURVEPLOT_HEADERS \
    $$APP_HEADERS


INCLUDEPATH += .. ../glew/include

DEFINES += EIGEN_USE_NEW_STDVECTOR

win32-g++ {
    # Work around alignment problems with MinGW. Fixed-size Eigen matrices
    # are sometimes allocated on the stack at unaligned addresses even though
    # __alignof e.g. Vector4d is 16. The flag below works on g++ versions 4 and
    # higher. For older versions, we need to define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
    QMAKE_CXXFLAGS += -mincoming-stack-boundary=2

    # Disable min and max preprocessor on Win32 platform that interfere
    # with the C++ Standard Library template functions.
    DEFINES += NOMINMAX
}



