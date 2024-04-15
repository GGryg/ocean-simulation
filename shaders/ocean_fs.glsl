#version 430 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 fragPosition;
in vec3 normals;

uniform vec3 lightPosition;
uniform vec3 viewPosition;
uniform vec3 lightColor;

void main()
{

    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(normals);
    vec3 ligthDir = normalize(lightPosition - fragPosition);
    float diff = max(dot(norm, ligthDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPosition - fragPosition);
    vec3 reflectDir = reflect(-ligthDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * vec3(0.1333, 0.4078, 1.0);

    //FragColor = vec4(0.1333, 0.4078, 1.0, 1.0);
    //FragColor = vec4(1.0);
    FragColor = vec4(result, 1.0);
}