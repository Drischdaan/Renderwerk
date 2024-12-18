#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec4 outColor;

struct Vertex {
	vec3 position;
	vec4 color;
}; 

layout(buffer_reference, std430) readonly buffer VertexBuffer{ 
	Vertex vertices[];
};

layout( push_constant ) uniform constants
{
	VertexBuffer vertexBuffer;
} PushConstants;

void main() 
{
	Vertex vertex = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

	gl_Position = vec4(vertex.position, 1.0f);
	outColor = vertex.color;
}
