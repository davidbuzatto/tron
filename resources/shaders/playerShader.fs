#version 330

in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform float screenWidth;
uniform float screenHeight;
uniform float blurAmount;

vec3 colorFromHSV( float hue, float saturation, float value );

void main() {
    
    vec2 texCoord = gl_FragCoord.xy / vec2(screenWidth, screenHeight);

    // tamanho do blur em pixels
    float blur = blurAmount / screenWidth;

    // kernel gaussiano 13x13
    vec4 color = vec4(0.0);
    float totalWeight = 0.0;

    for(int x = -6; x <= 6; x++) {
        for(int y = -6; y <= 6; y++) {
            float distance = sqrt(float(x*x + y*y));
            float weight = exp(-distance * distance / (2.0 * 2.0)); // Gaussiana
            vec2 offset = vec2(float(x), float(y)) * blur;
            color += texture(texture0, texCoord + offset) * weight;
            totalWeight += weight;
        }
    }

    color /= totalWeight;

    // pega a cor original
    vec4 original = texture(texture0, texCoord);

    // combina: mantém o core sólido e adiciona o glow ao redor
    finalColor = original + color * 2.5;

}

vec3 colorFromHSV( float hue, float saturation, float value ) {
    
    vec3 color = vec3( 0, 0, 0 );

    float k = mod( 5.0 + hue / 60.0, 6.0 );
    float t = 4.0 - k;
    k = (t < k) ? t : k;
    k = clamp( k, 0.0, 1.0 );
    color.r = value - value * saturation * k;

    k = mod( 3.0 + hue / 60.0, 6.0 );
    t = 4.0 - k;
    k = (t < k) ? t : k;
    k = clamp( k, 0.0, 1.0 );
    color.g = value - value * saturation * k;

    k = mod( 1.0f + hue / 60.0, 6.0 );
    t = 4.0 - k;
    k = (t < k) ? t : k;
    k = clamp( k, 0.0, 1.0 );
    color.b = value - value * saturation * k;

    return color;
    
}
