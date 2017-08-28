 #version 430 core
layout (location = 0) out vec4 color;
/* Use all textures ever */
layout (binding = 0) uniform sampler3D volume_data;
layout (binding = 1) uniform sampler1D transfer_function;
layout (binding = 2) uniform sampler2D front_faces;
layout (binding = 3) uniform sampler2D back_faces;

uniform vec2 viewport;

in VS_OUT {
  vec3 V;
} fs_in;
uniform float step_size;
uniform bool first_hit;
const vec4 specularProduct = vec4(1.0, 1.0, 1.0, 1.0);

//const vec3 delta = vec3(step_size / 4.0 ,0.0,0.0);
// Volume Fragment Shader

 /*Function that samples the surrounding region and returns an
   average to remove 'noisy' data
   I don't really use this function, except for testing purposes.
*/
vec4 calculate_lighting(vec3 center, vec4 color) {
  vec3 delta= vec3(step_size, 0.0, 0.0);
  float left = texture(volume_data, center - delta).r;
  float right  = texture(volume_data, center + delta).r;
  float down = texture(volume_data, center - delta.yxz).r;
  float up  = texture(volume_data, center + delta.yxz).r;
  float forward = texture(volume_data, center - delta.yzx).r;
  float back = texture(volume_data, center + delta.yzx).r;

  vec3 norm = normalize(vec3(left - right,down - up, back - forward));
  vec3 light_reflect = reflect(-fs_in.V,norm);
  float Ks = pow(max(dot(fs_in.V, light_reflect),0.0),2.0);
  vec4 specular = Ks * specularProduct;
  if(dot(norm,fs_in.V) < 0.0) {
    specular = vec4(0.0,0.0,0.0,0.0);
  }
  return color + specular;
}


void main(void) {
  /* Calculate the screen space position of the fragment to
     use for our Framebuffer texture lookups */
  vec2 coord =  gl_FragCoord.xy;
  coord.x /= viewport.x;
  coord.y /= viewport.y;

  // Retrieve front and back bounding box locations
  vec3 front = texture(front_faces, coord).xyz;
  vec3 back = texture(back_faces, coord).xyz;
  // Calculate Ray Direction
  vec3 dir = normalize(back - front) * step_size;

  vec3 current = front.xyz;
  // Traverse Ray accumulating color contributions to dst
  vec4 dst = vec4(0.0);

  float voxel;

  /* Ray marching loop. Terminates once ALPHA of color contributions
     is equal to one, or if we hit an isosurcae,
     or once the depth component of the ray traversal
     position is greater than the back face
     */
  for(int i = 0; i < 2000; i++) {
    voxel =  texture(volume_data, current.zxy).r;
    vec4 src = texture(transfer_function, voxel);

    dst = (1.0 - dst.a) * src + dst;
    // Break out of loop if we 'hit' a surface
    if(first_hit && voxel > 0.1) {
      //dst = calculate_lighting(current, dst);
      break;
    }
    //Increment current starting position by dir ammt
    current = current + dir;
    //Test if we need to break out of ray traversal loop
    // Will rewrite to test dot product of current compared 3.0

    bool behind = current.z < back.z;
    if((behind && dir.z < 0) || (!behind && dir.z >= 0) || dst.a  == 1.0 ) {
      break;
    }
  }
  color = vec4(mix(dst.xyz, current,0.35), 1.0);
}
