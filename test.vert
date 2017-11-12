#version 300 es


layout(location = 0) in highp vec3 VertexPosition;
layout(location = 1) in highp vec3 VertexColor;

out highp vec3 Color;




void main()
{
    Color = VertexColor;
    gl_Position = vec4(VertexPosition, 1.0);
}
