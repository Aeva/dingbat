#version 300 es


layout(location = 0) in highp vec3 VertexPosition;
//layout(location = 1) in highp vec3 VertexColor;

out highp vec3 Color;


layout(std140) uniform SomeBlock
{
    mat4 ProjectionMatrix;
    mat4 ViewMatrix;
    mat4 WorldMatrix;
};




void main()
{
    //vec4 WorldSpace = WorldMatrix * vec4(VertexPosition.xyz, 1.0);
    //Color = WorldSpace.xyz * 0.5 + 0.5;
    float LinearDepth = length((ViewMatrix * WorldMatrix) * vec4(VertexPosition.xyz, 1.0));
    float Low = 2.5;
    float High = 5.0;
    float Adjusted = (LinearDepth - Low) / (High - Low);
    
    Color = vec3(1.0 - Adjusted);
    gl_Position = ProjectionMatrix * ViewMatrix * WorldMatrix * vec4(VertexPosition.xyz, 1.0);
}
