#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
//
// cbuffer PsPerObject
// {
//
//   float2 inverseTilesetTextureSize;  // Offset:    0 Size:     8
//   float2 inverseTilemapTextureSize;  // Offset:    8 Size:     8 [unused]
//   float tileSize;                    // Offset:   16 Size:     4
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- -------------- ------
// texSampler                        sampler      NA          NA             s0      1 
// tileset                           texture  float4          2d             t0      1 
// tilemap                           texture  float4          2d             t1      1 
// PsPerObject                       cbuffer      NA          NA            cb0      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float       
// TEXCOORD                 0   xy          1     NONE   float       
// TEXCOORD                 1     zw        1     NONE   float     zw
// TEXCOORD                 2   xy          2     NONE   float   xy  
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_TARGET                0   xyzw        0   TARGET   float   xyzw
//
//
// Constant buffer to DX9 shader constant mappings:
//
// Target Reg Buffer  Start Reg # of Regs        Data Conversion
// ---------- ------- --------- --------- ----------------------
// c0         cb0             0         2  ( FLT, FLT, FLT, FLT)
//
//
// Sampler/Resource to DX9 shader sampler mappings:
//
// Target Sampler Source Sampler  Source Resource
// -------------- --------------- ----------------
// s0             s0              t0               
// s1             s0              t1               
//
//
// Level9 shader bytecode:
//
    ps_2_x
    def c2, -1, -0, 0, 256
    dcl t0
    dcl t1.xy
    dcl_2d s0
    dcl_2d s1
    texld r0, t1, s1
    add r0.zw, r0.xyxy, c2.x
    mul r0.zw, r0, r0
    cmp r0.w, -r0.w, c2.x, c2.y
    cmp r1, -r0.z, r0.w, c2.z
    texkill r1
    mul r0.xy, r0, c2.w
    frc r0.zw, r0.xyxy
    add r0.xy, -r0.zwzw, r0
    rcp r0.z, c1.x
    mul r0.zw, r0.z, t0.xywz
    frc r1.xy, r0.zwzw
    add r0.zw, r0, -r1.xyxy
    mad r0.zw, c1.x, -r0, t0.xywz
    mad r0.xy, r0, c1.x, r0.zwzw
    mul r0.xy, r0, c0
    texld r0, r0, s0
    mov oC0, r0

// approximately 18 instruction slots used (2 texture, 16 arithmetic)
ps_4_0
dcl_constantbuffer CB0[2], immediateIndexed
dcl_sampler s0, mode_default
dcl_resource_texture2d (float,float,float,float) t0
dcl_resource_texture2d (float,float,float,float) t1
dcl_input_ps linear v1.zw
dcl_input_ps linear v2.xy
dcl_output o0.xyzw
dcl_temps 1
sample r0.xyzw, v2.xyxx, t1.xyzw, s0
eq r0.zw, r0.xxxy, l(0.000000, 0.000000, 1.000000, 1.000000)
and r0.z, r0.w, r0.z
discard_nz r0.z
mul r0.xy, r0.xyxx, l(256.000000, 256.000000, 0.000000, 0.000000)
div r0.zw, v1.zzzw, cb0[1].xxxx
round_ni r0.xyzw, r0.xyzw
mad r0.zw, -cb0[1].xxxx, r0.zzzw, v1.zzzw
mad r0.xy, r0.xyxx, cb0[1].xxxx, r0.zwzz
mul r0.xy, r0.xyxx, cb0[0].xyxx
sample o0.xyzw, r0.xyxx, t0.xyzw, s0
ret 
// Approximately 12 instruction slots used
#endif

const BYTE ps_tilemap_code[] =
{
     68,  88,  66,  67, 134,  96, 
    197, 123,   6, 130, 240, 166, 
    138,  25,  80,  59, 206, 199, 
     60, 218,   1,   0,   0,   0, 
    176,   6,   0,   0,   6,   0, 
      0,   0,  56,   0,   0,   0, 
    224,   1,   0,   0, 196,   3, 
      0,   0,  64,   4,   0,   0, 
    244,   5,   0,   0, 124,   6, 
      0,   0,  65, 111, 110,  57, 
    160,   1,   0,   0, 160,   1, 
      0,   0,   0,   2, 255, 255, 
    104,   1,   0,   0,  56,   0, 
      0,   0,   1,   0,  44,   0, 
      0,   0,  56,   0,   0,   0, 
     56,   0,   2,   0,  36,   0, 
      0,   0,  56,   0,   0,   0, 
      0,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   2, 255, 255,  81,   0, 
      0,   5,   2,   0,  15, 160, 
      0,   0, 128, 191,   0,   0, 
      0, 128,   0,   0,   0,   0, 
      0,   0, 128,  67,  31,   0, 
      0,   2,   0,   0,   0, 128, 
      0,   0,  15, 176,  31,   0, 
      0,   2,   0,   0,   0, 128, 
      1,   0,   3, 176,  31,   0, 
      0,   2,   0,   0,   0, 144, 
      0,   8,  15, 160,  31,   0, 
      0,   2,   0,   0,   0, 144, 
      1,   8,  15, 160,  66,   0, 
      0,   3,   0,   0,  15, 128, 
      1,   0, 228, 176,   1,   8, 
    228, 160,   2,   0,   0,   3, 
      0,   0,  12, 128,   0,   0, 
     68, 128,   2,   0,   0, 160, 
      5,   0,   0,   3,   0,   0, 
     12, 128,   0,   0, 228, 128, 
      0,   0, 228, 128,  88,   0, 
      0,   4,   0,   0,   8, 128, 
      0,   0, 255, 129,   2,   0, 
      0, 160,   2,   0,  85, 160, 
     88,   0,   0,   4,   1,   0, 
     15, 128,   0,   0, 170, 129, 
      0,   0, 255, 128,   2,   0, 
    170, 160,  65,   0,   0,   1, 
      1,   0,  15, 128,   5,   0, 
      0,   3,   0,   0,   3, 128, 
      0,   0, 228, 128,   2,   0, 
    255, 160,  19,   0,   0,   2, 
      0,   0,  12, 128,   0,   0, 
     68, 128,   2,   0,   0,   3, 
      0,   0,   3, 128,   0,   0, 
    238, 129,   0,   0, 228, 128, 
      6,   0,   0,   2,   0,   0, 
      4, 128,   1,   0,   0, 160, 
      5,   0,   0,   3,   0,   0, 
     12, 128,   0,   0, 170, 128, 
      0,   0, 180, 176,  19,   0, 
      0,   2,   1,   0,   3, 128, 
      0,   0, 238, 128,   2,   0, 
      0,   3,   0,   0,  12, 128, 
      0,   0, 228, 128,   1,   0, 
     68, 129,   4,   0,   0,   4, 
      0,   0,  12, 128,   1,   0, 
      0, 160,   0,   0, 228, 129, 
      0,   0, 180, 176,   4,   0, 
      0,   4,   0,   0,   3, 128, 
      0,   0, 228, 128,   1,   0, 
      0, 160,   0,   0, 238, 128, 
      5,   0,   0,   3,   0,   0, 
      3, 128,   0,   0, 228, 128, 
      0,   0, 228, 160,  66,   0, 
      0,   3,   0,   0,  15, 128, 
      0,   0, 228, 128,   0,   8, 
    228, 160,   1,   0,   0,   2, 
      0,   8,  15, 128,   0,   0, 
    228, 128, 255, 255,   0,   0, 
     83,  72,  68,  82, 220,   1, 
      0,   0,  64,   0,   0,   0, 
    119,   0,   0,   0,  89,   0, 
      0,   4,  70, 142,  32,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,  90,   0,   0,   3, 
      0,  96,  16,   0,   0,   0, 
      0,   0,  88,  24,   0,   4, 
      0, 112,  16,   0,   0,   0, 
      0,   0,  85,  85,   0,   0, 
     88,  24,   0,   4,   0, 112, 
     16,   0,   1,   0,   0,   0, 
     85,  85,   0,   0,  98,  16, 
      0,   3, 194,  16,  16,   0, 
      1,   0,   0,   0,  98,  16, 
      0,   3,  50,  16,  16,   0, 
      2,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      0,   0,   0,   0, 104,   0, 
      0,   2,   1,   0,   0,   0, 
     69,   0,   0,   9, 242,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  16,  16,   0,   2,   0, 
      0,   0,  70, 126,  16,   0, 
      1,   0,   0,   0,   0,  96, 
     16,   0,   0,   0,   0,   0, 
     24,   0,   0,  10, 194,   0, 
     16,   0,   0,   0,   0,   0, 
      6,   4,  16,   0,   0,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 128,  63, 
      0,   0, 128,  63,   1,   0, 
      0,   7,  66,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  13,   0,   4,   3, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  56,   0,   0,  10, 
     50,   0,  16,   0,   0,   0, 
      0,   0,  70,   0,  16,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128,  67, 
      0,   0, 128,  67,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     14,   0,   0,   8, 194,   0, 
     16,   0,   0,   0,   0,   0, 
    166,  30,  16,   0,   1,   0, 
      0,   0,   6, 128,  32,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  65,   0,   0,   5, 
    242,   0,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,  50,   0, 
      0,  11, 194,   0,  16,   0, 
      0,   0,   0,   0,   6, 128, 
     32, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0, 166,  14,  16,   0, 
      0,   0,   0,   0, 166,  30, 
     16,   0,   1,   0,   0,   0, 
     50,   0,   0,  10,  50,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   0,  16,   0,   0,   0, 
      0,   0,   6, 128,  32,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0, 230,  10,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   8,  50,   0,  16,   0, 
      0,   0,   0,   0,  70,   0, 
     16,   0,   0,   0,   0,   0, 
     70, 128,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     69,   0,   0,   9, 242,  32, 
     16,   0,   0,   0,   0,   0, 
     70,   0,  16,   0,   0,   0, 
      0,   0,  70, 126,  16,   0, 
      0,   0,   0,   0,   0,  96, 
     16,   0,   0,   0,   0,   0, 
     62,   0,   0,   1,  83,  84, 
     65,  84, 116,   0,   0,   0, 
     12,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   7,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  82,  68,  69,  70, 
    172,   1,   0,   0,   1,   0, 
      0,   0, 196,   0,   0,   0, 
      4,   0,   0,   0,  28,   0, 
      0,   0,   0,   4, 255, 255, 
      0,   1,   0,   0, 132,   1, 
      0,   0, 156,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0, 167,   0, 
      0,   0,   2,   0,   0,   0, 
      5,   0,   0,   0,   4,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  13,   0,   0,   0, 
    175,   0,   0,   0,   2,   0, 
      0,   0,   5,   0,   0,   0, 
      4,   0,   0,   0, 255, 255, 
    255, 255,   1,   0,   0,   0, 
      1,   0,   0,   0,  13,   0, 
      0,   0, 183,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0, 116, 101, 
    120,  83,  97, 109, 112, 108, 
    101, 114,   0, 116, 105, 108, 
    101, 115, 101, 116,   0, 116, 
    105, 108, 101, 109,  97, 112, 
      0,  80, 115,  80, 101, 114, 
     79,  98, 106, 101,  99, 116, 
      0, 171, 183,   0,   0,   0, 
      3,   0,   0,   0, 220,   0, 
      0,   0,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  36,   1,   0,   0, 
      0,   0,   0,   0,   8,   0, 
      0,   0,   2,   0,   0,   0, 
     64,   1,   0,   0,   0,   0, 
      0,   0,  80,   1,   0,   0, 
      8,   0,   0,   0,   8,   0, 
      0,   0,   0,   0,   0,   0, 
     64,   1,   0,   0,   0,   0, 
      0,   0, 106,   1,   0,   0, 
     16,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
    116,   1,   0,   0,   0,   0, 
      0,   0, 105, 110, 118, 101, 
    114, 115, 101,  84, 105, 108, 
    101, 115, 101, 116,  84, 101, 
    120, 116, 117, 114, 101,  83, 
    105, 122, 101,   0, 171, 171, 
      1,   0,   3,   0,   1,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 105, 110, 
    118, 101, 114, 115, 101,  84, 
    105, 108, 101, 109,  97, 112, 
     84, 101, 120, 116, 117, 114, 
    101,  83, 105, 122, 101,   0, 
    116, 105, 108, 101,  83, 105, 
    122, 101,   0, 171,   0,   0, 
      3,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  77, 105,  99, 114, 
    111, 115, 111, 102, 116,  32, 
     40,  82,  41,  32,  72,  76, 
     83,  76,  32,  83, 104,  97, 
    100, 101, 114,  32,  67, 111, 
    109, 112, 105, 108, 101, 114, 
     32,  49,  48,  46,  49,   0, 
     73,  83,  71,  78, 128,   0, 
      0,   0,   4,   0,   0,   0, 
      8,   0,   0,   0, 104,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0, 116,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0, 116,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     12,  12,   0,   0, 116,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
      3,   3,   0,   0,  83,  86, 
     95,  80,  79,  83,  73,  84, 
     73,  79,  78,   0,  84,  69, 
     88,  67,  79,  79,  82,  68, 
      0, 171, 171, 171,  79,  83, 
     71,  78,  44,   0,   0,   0, 
      1,   0,   0,   0,   8,   0, 
      0,   0,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  15,   0, 
      0,   0,  83,  86,  95,  84, 
     65,  82,  71,  69,  84,   0, 
    171, 171
};
