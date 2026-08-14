#version 330 core

in vec3 vWorldPosition;
in vec3 vNormal;

uniform vec3 uBaseColor;
uniform vec3 uLightDirection;
uniform vec3 uViewPosition;

out vec4 FragColor;

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 directionToLight = normalize(-uLightDirection);
    float diffuseStrength = max(dot(normal, directionToLight), 0.0);

    vec3 viewDirection = normalize(uViewPosition - vWorldPosition);
    vec3 halfwayDirection = normalize(directionToLight + viewDirection);
    float specularStrength = pow(max(dot(normal, halfwayDirection), 0.0), 48.0) * 0.22;

    float skyContribution = normal.y * 0.5 + 0.5;
    vec3 ambient = mix(vec3(0.055, 0.065, 0.09), vec3(0.20, 0.24, 0.32), skyContribution);
    vec3 lighting = ambient + vec3(0.82, 0.78, 0.70) * diffuseStrength;
    vec3 color = uBaseColor * lighting + vec3(specularStrength);

    FragColor = vec4(color, 1.0);
}
