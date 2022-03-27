//drvene kocke
#version 330 core

struct Material {
    //vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
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
in vec2 tc;
in vec3 normal;
in vec3 fragPos;

uniform vec3 viewPos;
uniform Material material;
uniform Dirlight dlight;
uniform Pointlight plight;
uniform Pointlight bluelight;

void main()
{
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 norm = normalize(normal);

    vec3 result = calcdirlight(dlight,norm,viewDir,material) + calcpointlight(plight,norm,viewDir,fragPos,material)
    + calcpointlight(bluelight,norm,viewDir,fragPos,material);;
    FragColor = vec4(result,1.0);
}

vec3 calcdirlight(Dirlight light,vec3 norm,vec3 viewDir,Material material){

    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm,lightDir),0.0);

    vec3 reflectDir = reflect(-lightDir,norm);
    float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);

    vec3 ambient  = light.ambient * texture(material.diffuse,tc).rgb;
    vec3 diffuse = texture(material.diffuse,tc).rgb * diff * light.diffuse;
    vec3 specular = texture(material.specular,tc).rgb * spec * light.specular;


    return (ambient + diffuse + specular);
}

vec3 calcpointlight(Pointlight light,vec3 norm,vec3 viewDir,vec3 fragPos,Material material){

    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm,lightDir),0.0);

    vec3 reflectDir = reflect(-lightDir,norm);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfwayDir,norm),0.0),material.shininess);

    vec3 ambient  = light.ambient * texture(material.diffuse,tc).rgb;
    vec3 diffuse = texture(material.diffuse,tc).rgb * diff * light.diffuse;
    vec3 specular = texture(material.specular,tc).rgb * spec * light.specular;

    float distance = length(light.position - fragPos);
    float att = 1.0/(light.constant + light.linear*distance + light.quadratic*distance*distance);

    return att*(ambient + diffuse + specular);
}