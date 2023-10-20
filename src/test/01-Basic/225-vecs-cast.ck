// casting between vec2, vec3, vec4

// vec2 to vec3
@(1,2) $ vec4 => vec4 v4; <<< v4 >>>;
// vec2 to vec4
@(1,2) $ vec4 => v4; <<< v4 >>>;
// vec3 to vec4
@(1,2,3) $ vec4 => v4; <<< v4 >>>;

// vec4 to vec3
@(1,2,3,4) $ vec3 => vec3 v3; <<< v3 >>>;
// vec4 to vec2
@(1,2,3,4) $ vec2 => vec2 v2; <<< v2 >>>;
