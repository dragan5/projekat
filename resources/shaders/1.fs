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
    vec3 position;
    vec3 direction;
    float constant;
    float linear;
    float quadratic;
};

out vec4 FragColor;
in vec2 tc;
in vec3 normal;
in vec3 fragPos;

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

    float distance = length(light.position - fragPos);
    float att = 1.0/(light.constant + light.linear*distance + light.quadratic*distance*distance);

    vec3 result = att*(ambient + diffuse + specular);
    FragColor = vec4(result,1.0);
}