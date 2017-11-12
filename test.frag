#version 300 es


in highp vec3 Color;
out highp vec4 FinalColor;



void main()
{
    FinalColor = vec4(Color, 1.0);
}
