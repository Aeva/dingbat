#version 300 es


layout(location = 0) in highp vec3 VertexPosition;
layout(location = 1) in highp vec3 VertexColor;

out highp vec3 Color;


layout(std140) uniform SomeBlock
{
    mat4 CameraMatrix;
    mat4 WorldMatrix;
};




void main()
{
    Color = VertexColor;
    gl_Position = CameraMatrix * WorldMatrix * vec4(VertexPosition.xyz, 1.0);
}
