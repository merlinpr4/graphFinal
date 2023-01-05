//Fog and multiple lights
//Lighting reference https://learnopengl.com/Lighting/Multiple-lights
//Direct , Spotlight and Point light calculations aswell as exponential squared fog are here
// Blinn - Phong implemented aswell
#version 330 core
out vec4 fragColour;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct DirectLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define NO_LIGHTS 4
in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform vec3 viewPos;
uniform DirectLight directLight;
uniform PointLight pointLights[NO_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform bool fog;

vec3 DirectLightCalc(DirectLight light, vec3 norm, vec3 viewDir);
vec3 PointLightCalc(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir);
vec3 SpotLightCalc(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir);

void main()
{    
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos -fragPos);
    
    // directional lighting
    vec3 result = DirectLightCalc(directLight, norm, viewDir);
    // point lights
    for(int i = 0; i < NO_LIGHTS; i++)
        result += PointLightCalc(pointLights[i], norm,fragPos, viewDir);    
    //spot light
    result += SpotLightCalc(spotLight, norm,fragPos, viewDir);   
    
    if(fog)
    {
    fragColour = vec4(result, 1.0);
    float fogMax = 10.0;
    float fogDensity = 0.30 ;
    vec4  fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

    // fog -----------------------------------------------------------------------------------------------------------------------------------
    //distance from pixel to camera
    float dist = length(fragPos.xyz - viewPos.xyz);
    //exponential squared fog
    float distRatio = 4.0 * dist/fogMax ;
    float fogFactor = exp(-distRatio * fogDensity * distRatio * fogDensity);
        fragColour = mix(fogColor, fragColour, fogFactor);
    }
    else 
    {      
    fragColour = vec4(result, 1.0);
    }
}

// calcs the color when using a directional light.
vec3 DirectLightCalc(DirectLight light, vec3 norm, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    // specular shading

    //phong
   // vec3 reflectDir = reflect(-lightDir, normal);
   // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess );

    //blinn phong
    vec3 halfwayDirection = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDirection), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoord));
    return (ambient + diffuse + specular);
}


//Spotlight
vec3 SpotLightCalc(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position -fragPos);
    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    // specular shading
    //blinn phong
    vec3 halfwayDirection = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDirection), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoord));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

// Point light.
vec3 PointLightCalc(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position -fragPos);
    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    // specular shading
     //blinn phong
    vec3 halfwayDirection = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDirection), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position -fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoord));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}
