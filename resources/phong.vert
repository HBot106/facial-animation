#version 330 core
layout(location = 0) in vec3 face_closed;
layout(location = 1) in vec3 face_generic_consonant;
layout(location = 2) in vec3 face_AEI;
layout(location = 3) in vec3 face_BMP;
layout(location = 4) in vec3 face_Ch;
layout(location = 5) in vec3 face_O;
layout(location = 6) in vec3 face_Th;
layout(location = 7) in vec3 face_FV;
layout(location = 8) in vec3 face_R;
layout(location = 9) in vec3 face_U;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform float current;
uniform float next;
uniform float t;

out vec3 fragPos;

void main() 
{
	vec3 face1, face2, faceDisplay;

	float newCurrent = current;
	float newNext = next;
	
	if (newCurrent == 0.0f) { face1 = face_closed; }
	else if (newCurrent == 1.0f) { face1 = face_generic_consonant; }
	else if (newCurrent == 2.0f) { face1 = face_AEI; }
	else if (newCurrent == 3.0f) { face1 = face_BMP; }
	else if (newCurrent == 4.0f) { face1 = face_Ch; }
	else if (newCurrent == 5.0f) { face1 = face_O; }
	else if (newCurrent == 6.0f) { face1 = face_Th; }
	else if (newCurrent == 7.0f) { face1 = face_FV; }
	else if (newCurrent == 8.0f) { face1 = face_R; }
	else if (newCurrent == 9.0f) { face1 = face_U; }

	if (newNext == 0.0f) { face2 = face_closed; }
	else if (newNext == 1.0f) { face2 = face_generic_consonant; }
	else if (newNext == 2.0f) { face2 = face_AEI; }
	else if (newNext == 3.0f) { face2 = face_BMP; }
	else if (newNext == 4.0f) { face2 = face_Ch; }
	else if (newNext == 5.0f) { face2 = face_O; }
	else if (newNext == 6.0f) { face2 = face_Th; }
	else if (newNext == 7.0f) { face2 = face_FV; }
	else if (newNext == 8.0f) { face2 = face_R; }
	else if (newNext == 9.0f) { face2 = face_U; }

	faceDisplay = ((1.0f - t) * face1) + (t * face2);

    gl_Position = P * V * M * vec4(faceDisplay, 1.0);
    fragPos = vec4(V * M * vec4(faceDisplay, 1.0)).xyz;
}
