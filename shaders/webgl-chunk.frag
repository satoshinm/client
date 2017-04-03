precision mediump float;
uniform sampler2D sampler;
uniform sampler2D sky_sampler;
uniform sampler2D damage_sampler;
uniform float timer;
uniform vec3 ambient_color;
uniform vec3 ambient_light;

varying vec2 fragment_uv;
varying vec2 damage_uv;
varying float damage_factor;
varying float fragment_ao;
varying float ambient;
varying float fog_factor;
varying float fog_height;
varying float diffuse;
varying vec3 light;

const vec3 damage_color = vec3(0,0,0);

void main() {
    vec3 color = vec3(texture2D(sampler, fragment_uv));
    if (color == vec3(1.0, 0.0, 1.0)) {
        discard;
    }
    float damage = texture2D(damage_sampler, damage_uv).y;
    color = mix(color, damage_color, damage * damage_factor);
    vec3 light_sum = (ambient_light + ambient_color * diffuse) * ambient + light;
    color = clamp(color * light_sum * fragment_ao, vec3(0.0), vec3(1.0));
    vec3 sky_color = vec3(texture2D(sky_sampler, vec2(timer, fog_height)));
    color = mix(color, sky_color, fog_factor);
    gl_FragColor = vec4(color, 1.0);
}
