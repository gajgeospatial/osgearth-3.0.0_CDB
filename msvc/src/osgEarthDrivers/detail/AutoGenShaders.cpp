/** CMake Template File - compiled into AutoGenShaders.cpp */
#include <osgEarthDrivers/detail/DetailShaders>

#define MULTILINE(...) #__VA_ARGS__

using namespace osgEarth::Detail;

Shaders::Shaders()
{
    VertexView = "Detail.vert.view.glsl";
    _sources[VertexView] = "#version $GLSL_VERSION_STR%EOL%$GLSL_DEFAULT_PRECISION_FLOAT%EOL%%EOL%#pragma vp_entryPoint oe_detail_vertexView%EOL%#pragma vp_location   vertex_view%EOL%%EOL%uniform float oe_detail_lod;    // uniform of base LOD%EOL%uniform float oe_detail_maxRange;%EOL%uniform float oe_detail_attenDist;%EOL%vec4 oe_layer_tilec;        // stage global - tile coordinates%EOL%out vec2 detailCoords;      // output to fragment stage%EOL%out float detailIntensity;  // output to fragment stage.%EOL%                %EOL%// Terrain SDK function%EOL%vec2 oe_terrain_scaleCoordsToRefLOD(in vec2 tc, in float refLOD);%EOL%               %EOL%void oe_detail_vertexView(inout vec4 VertexView)%EOL%{%EOL%  detailCoords = oe_terrain_scaleCoordsToRefLOD(oe_layer_tilec.st, int(oe_detail_lod));%EOL%  detailIntensity = clamp((oe_detail_maxRange - (-VertexView.z))/oe_detail_attenDist, 0.0, 1.0);%EOL%}%EOL%%EOL%";

    Fragment = "Detail.frag.glsl";
    _sources[Fragment] = "#version $GLSL_VERSION_STR%EOL%$GLSL_DEFAULT_PRECISION_FLOAT%EOL%%EOL%#pragma vp_entryPoint oe_detail_fragment%EOL%#pragma vp_location   fragment_coloring%EOL%#pragma vp_order      1%EOL%                %EOL%uniform sampler2D oe_detail_tex; // uniform of detail texture%EOL%uniform float oe_detail_alpha;   // The detail textures alpha.%EOL%in vec2 detailCoords;            // input from vertex stage%EOL%in float detailIntensity;        // The intensity of the detail effect.%EOL%                %EOL%void oe_detail_fragment(inout vec4 color)%EOL%{%EOL%    vec4 texel = texture(oe_detail_tex, detailCoords);%EOL%    color.rgb = mix(color.rgb, texel.rgb, oe_detail_alpha * detailIntensity);%EOL%}                %EOL%%EOL%";
};
