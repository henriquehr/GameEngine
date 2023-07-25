
struct PointLight {
    vec4 position;// ignore w
    vec4 color;   // w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    mat4 invView;
    vec4 ambientLightColor;// w is intensity
    PointLight pointLights[10];
    int activeLights;
}
ubo;
