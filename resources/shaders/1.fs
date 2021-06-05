#version 330 core

struct Material {
    //vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 lightPos;
    vec3 direction;
};

out vec4 FragColor;
in vec2 tc;
in vec3 normal;
in vec3 fragPos;

//uniform sampler2D t0;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    vec3 ambient = texture(material.diffuse,tc).rgb * light.ambient;

    vec3 norm = normalize(normal);
    //vec3 lightDir = normalize(light.lightPos - fragPos);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm,lightDir),0.0);
    vec3 diffuse = texture(material.diffuse,tc).rgb * light.diffuse * diff;

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir,norm);
    float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
    vec3 specular = texture(material.specular,tc).rgb * spec * light.specular;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result,1.0);
//     FragColor = vec4(1.0,1.0,0.0,1.0);
    //FragColor = texture(t0,tc);
}