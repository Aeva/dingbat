#version 300 es


layout(location = 0) in highp vec3 VertexPosition;
layout(location = 1) in highp vec3 VertexColor;

out highp vec3 Color;


layout(std140) uniform SomeBlock
{
    float SomeParam;
    //vec2 Offset;
};




void main()
{
    Color = VertexColor;
    gl_Position = vec4(VertexPosition.x + SomeParam, VertexPosition.yz, 1.0);
}
