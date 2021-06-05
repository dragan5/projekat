#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Pointlight{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

struct Dirlight{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

vec3 calcdirlight(Dirlight light,vec3 normal,vec3 viewDir,Material material);
vec3 calcpointlight(Pointlight light,vec3 normal,vec3 viewDir,vec3 fragPos,Material material);

out vec4 FragColor;
in vec3 normal;
in vec3 fragPos;

uniform vec3 viewPos;
uniform Material material;
uniform Dirlight dlight;
uniform Pointlight plight;

void main()
{
    vec3 normal = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 result = calcdirlight(dlight,normal,viewDir,material) + calcpointlight(plight,normal,viewDir,fragPos,material);

    FragColor = vec4(result,1.0);
}

vec3 calcdirlight(Dirlight light,vec3 normal,vec3 viewDir,Material material){

    vec3 lightDir = normalize(-dlight.direction);
    float diff = max(dot(normal,lightDir),0.0);

    vec3 reflectDir = reflect(-lightDir,normal);
    float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);

    vec3 ambient  = dlight.ambient * material.ambient;
    vec3 diffuse = material.diffuse * diff * dlight.diffuse;
    vec3 specular = material.specular * spec * dlight.specular;


    return (ambient + diffuse + specular);
}

vec3 calcpointlight(Pointlight light,vec3 normal,vec3 viewDir,vec3 fragPos,Material material){

    vec3 lightDir = normalize(plight.position - fragPos);
    float diff = max(dot(normal,lightDir),0.0);

    vec3 reflectDir = reflect(-lightDir,normal);
    float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);

    vec3 ambient  = plight.ambient * material.ambient;
    vec3 diffuse = material.diffuse * diff * plight.diffuse;
    vec3 specular = material.specular * spec * plight.specular;

    float distance = length(plight.position - fragPos);
    float att = 1.0/(plight.constant + plight.linear*distance + plight.quadratic*distance*distance);

    return att*(ambient + diffuse + specular);
}