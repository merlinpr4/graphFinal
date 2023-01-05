//Fog and lighting
#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoords;

in vec3 Normal;  
in vec3 FragPos;  

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
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);

    //blinn phong
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);

    vec3 specular = light.specular * (spec * material.specular);  
    vec3 result = (ambient + diffuse + specular) ;
        
  //direct light
  FragColor = texture(texture_diffuse1, TexCoords) * vec4(result, 1.0) ;

 
     if(fog)
    {
    FragColor = vec4(result, 1.0);
    float fogMax = 10.0;
    float fogDensity = 0.30 ;
    vec4  fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

    // Calculate fog
    //distance from pixel to camera
    float dist = length(FragPos.xyz - viewPos.xyz);
    //exponential squared fog
    float distRatio = 4.0 * dist/fogMax ;
    float fogFactor = exp(-distRatio * fogDensity * distRatio * fogDensity);
        FragColor = mix(fogColor, FragColor, fogFactor);
    }
    else 
    {      
    FragColor = vec4(result, 1.0);
    }
  
}