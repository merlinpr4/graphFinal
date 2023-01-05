//Fog and lighting
#version 330 core
out vec4 fragColour;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 texCoord;

in vec3 normal;  
in vec3 fragPos;  

uniform vec3 viewPos; 
uniform Material material;
uniform Light light;
uniform bool fog;

uniform sampler2D texture_diffuse1;

void main()
{
   // ambient
    vec3 ambient = light.ambient * material.ambient;
  	
    // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.pos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // specular
    vec3 viewDir = normalize(viewPos - fragPos);

    //blinn phong
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);

    vec3 specular = light.specular * (spec * material.specular);  
    vec3 result = (ambient + diffuse + specular) ;
        
    //direct light
    fragColour = texture(texture_diffuse1, texCoord) * vec4(result, 1.0) ;

     if(fog)
    {
    fragColour = vec4(result, 1.0);
    float fogMax = 10.0;
    float fogDensity = 0.30 ;
    vec4  fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

    // Calculate fog
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