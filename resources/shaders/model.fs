//model
#version 330 core

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

vec3 calcdirlight(Dirlight light,vec3 normal,vec3 viewDir);
vec3 calcpointlight(Pointlight light,vec3 normal,vec3 viewDir,vec3 fragPos);

out vec4 FragColor;
in vec3 normal;
in vec3 fragPos;
in vec2 tc;

uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform Dirlight dlight;
uniform Pointlight plight;

void main()
{
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 result = calcpointlight(plight,normal,viewDir,fragPos) + calcdirlight(dlight,normal,viewDir);
    FragColor = vec4(result,1.0);
}

vec3 calcdirlight(Dirlight light,vec3 norm,vec3 viewDir){

    vec3 lightDir = normalize(-dlight.direction);
    float diff = max(dot(norm,lightDir),0.0);

    vec3 reflectDir = reflect(-lightDir,normal);
    float spec = pow(max(dot(viewDir,reflectDir),0.0),32.0f);

    vec3 ambient  = dlight.ambient * vec3(texture(texture_diffuse1,tc));
    vec3 diffuse = vec3(texture(texture_diffuse1,tc)) * diff * dlight.diffuse;
    vec3 specular = vec3(texture(texture_specular1,tc)) * spec * dlight.specular;


    return (ambient + diffuse + specular);
}

vec3 calcpointlight(Pointlight light,vec3 norm,vec3 viewDir,vec3 fragPos){

    vec3 lightDir = normalize(plight.position - fragPos);
    float diff = max(dot(norm,lightDir),0.0);

    vec3 reflectDir = reflect(-lightDir,normal);
    float spec = pow(max(dot(viewDir,reflectDir),0.0),32.0f);

    vec3 ambient  = plight.ambient * vec3(texture(texture_diffuse1,tc));
    vec3 diffuse = vec3(texture(texture_diffuse1,tc)) * diff * plight.diffuse;
    vec3 specular = vec3(texture(texture_specular1,tc)) * spec * plight.specular;

    float distance = length(plight.position - fragPos);
    float att = 1.0/(plight.constant + plight.linear*distance + plight.quadratic*distance*distance);

    return att*(ambient + diffuse + specular);
}