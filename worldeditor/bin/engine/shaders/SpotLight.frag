#pragma version

#include "Common.vert"
#include "BRDF.frag"

layout(location = 0) uniform mat4 t_model;
layout(location = 1) uniform mat4 t_view;

layout(location = 32) uniform sampler2D normalsMap;
layout(location = 34) uniform sampler2D paramsMap;
layout(location = 35) uniform sampler2D depthMap;
layout(location = 36) uniform sampler2D shadowMap;

layout(location = 0) in vec4 _vertex;

layout(location = 0) out vec4 rgb;

void main (void) {
    vec2 proj   = ((_vertex.xyz / _vertex.w) * 0.5 + 0.5).xy;

    vec4 slice0 = texture( normalsMap,  proj );
    vec3 n      = normalize( slice0.xyz * 2.0 - 1.0 );

    // Light model LIT
    if(slice0.w > 0.33) {
        float depth = texture( depthMap, proj ).x;
        vec4 world = getWorld(camera.mvpi, proj, depth );

        vec3 dir = light.position - (world.xyz / world.w);
        vec3 normDir = normalize(dir);
        float dist  = length(dir);

        float spot  = dot(normDir, light.direction);
        float fall = 0.0;
        if(spot > light.params.z) {
            fall  = 1.0 - (1.0 - spot) / (1.0 - light.params.z);
            fall  = getAttenuation( dist, light.params.y ) * light.params.x * fall;
        }

        vec4 slice1 = texture( paramsMap,   proj );
        float rough = slice1.x;
        float metal = slice1.z;
        float spec  = slice1.w;

        vec3 albedo = slice1.xyz;
        vec3 v      = normalize( camera.position.xyz - (world.xyz / world.w) );
        vec3 h      = normalize( normDir + v );

        float ln    = dot(normDir, n);

        float shadow = 1.0;
        if(light.shadows == 1.0) {
            vec4 offset = light.tiles[0];
            vec4 proj   = light.matrix[0] * world;
            vec3 coord  = (proj.xyz / proj.w);
            shadow  = getShadow(shadowMap, (coord.xy * offset.zw) + offset.xy, coord.z - light.bias);
        }

        float refl  = getCookTorrance( n, v, h, ln, rough ) * spec;
        float diff  = max(getLambert( ln, light.params.x ) * fall * shadow, 0.0);

        rgb = vec4( light.color.xyz * (diff + refl), 1.0 );
    } else {
        rgb = vec4( vec3(0.0), 1.0 );
    }
}
