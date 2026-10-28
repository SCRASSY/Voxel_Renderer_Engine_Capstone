#version 460 core

struct Material {
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};

in vec3 normal;
in vec2 texCoord;
in vec3 fragPos;

out vec4 FragColor;

uniform sampler2D texture1;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform Material material;

void main()
{
//    // AMBIENT
//    vec3 ambient = material.ambient * lightColor;
//
//    // DIFFUSE
//    vec3 norm = normalize(normal);
//    vec3 lightDir = normalize(lightPos - fragPos);
//    float diff = max(dot(norm, lightDir), 0.0);
//    vec3 diffuse = lightColor * (diff * material.diffuse);
//
//    // SPECULAR
//    vec3 viewDir = normalize(-fragPos);
//    vec3 reflectDir = reflect(-lightDir, norm);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//    vec3 specular = lightColor * (spec * material.specular);
//
//    // FINAL COLOR
//    vec3 combinedMat = ambient + diffuse + specular;
//    vec3 result = combinedMat * texture(texture1, texCoord).rgb;
//    FragColor = vec4(result, 1.0);

    // FOR NOW DON'T WORRY ABOUT LIGHTING
    FragColor = texture(texture1, texCoord);
}