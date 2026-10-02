# Object Shader Specification (AI generated)

This document specifies the required inputs, uniform buffer objects (UBOs), uniforms, interface variables, and texture unit bindings that object vertex and fragment shaders must contain to be compatible with the renderer.

---

## 1. Object Vertex Shaders (`.vert`)

All object vertex shaders must use `#version 460 core` and define `MAX_LIGHT_COUNT 16`.

### 1.1. Vertex Inputs (Attributes)
Must contain vertex attributes with position, normal, and UV texture coordinates bound to locations 0, 1, and 2 in that order:
- **Location 0 (`aPos`)**: Vertex position in local/model space (`vec3`).
- **Location 1 (`aNormal`)**: Vertex normal vector in local/model space (`vec3`).
- **Location 2 (`aTexCoords`)**: Vertex texture coordinates (`vec2`).

**Example:**
```glsl
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
```

---

### 1.2. Uniform Buffer Objects (UBOs)

#### Matrices (`binding = 0`)
A uniform buffer object containing the camera projection and view matrices, bound to binding point `0` using the `std140` layout:
- `mat4 projection`: Perspective or orthographic projection matrix.
- `mat4 view`: Camera view matrix.

**Example:**
```glsl
layout(std140, binding = 0) uniform Matrices {
    mat4 projection;
    mat4 view;
};
```

#### Directional Shadow Matrices (`binding = 2`)
A uniform buffer object containing the light-space view-projection transformation matrices for directional shadow mapping, bound to binding point `2` using the `std140` layout:
- `mat4 lightSpaceMatrices[MAX_LIGHT_COUNT]`: Array of light view-projection matrices corresponding to each light in the scene (where `MAX_LIGHT_COUNT = 16`).

**Example:**
```glsl
#define MAX_LIGHT_COUNT 16

layout(std140, binding = 2) uniform directionalShadowMatrices {
    mat4 lightSpaceMatrices[MAX_LIGHT_COUNT];
};
```

---

### 1.3. Uniforms

#### Model Matrix (`location = 0`)
A 4x4 matrix representing the model (world transform) matrix of the mesh being rendered:
- **Type**: `mat4`
- **Location**: `0`

**Example:**
```glsl
layout(location = 0) uniform mat4 modelMatrix;
```

#### Normal Matrix (`location = 1`)
A 3x3 normal matrix (`transpose(inverse(mat3(modelMatrix)))`) used to transform normal vectors into world space under non-uniform scaling:
- **Type**: `mat3`
- **Location**: `1`

**Example:**
```glsl
layout(location = 1) uniform mat3 normalMatrix;
```

---

## 2. Object Fragment Shaders (`.frag`)

All object fragment shaders must use `#version 460 core` and define `MAX_LIGHT_COUNT 16`.

### 2.1. Uniform Buffer Objects (UBOs)

#### Lights (`binding = 1`)
A uniform buffer object containing light data for up to `16` lights, bound to binding point `1` using the `std140` layout:
- `LightData lights[MAX_LIGHT_COUNT]`: Array of `LightData` structures.

The `LightData` struct fields (aligned to 16-byte boundaries for `std140` layout):
- `vec3 position`: Light position in world coordinates (used for point and spot lights).
- `vec3 direction`: Light direction vector (used for directional and spot lights).
- `vec3 color`: Light color (RGB).
- `vec4 ambientDiffuseSpecularLightType`:
  - `x`: Multiplier for ambient lighting component.
  - `y`: Multiplier for diffuse lighting component.
  - `z`: Multiplier for specular lighting component.
  - `w`: Light type indicator (`0.0` = Directional, `1.0` = Point, `2.0` = Spot).
- `vec4 cutoffsAndAttenuation`:
  - `x`: Inner cutoff angle in radians (for spot lights).
  - `y`: Outer cutoff angle in radians (for spot lights).
  - `z`: Linear attenuation coefficient $k_l$ (for point and spot lights).
  - `w`: Quadratic attenuation coefficient $k_q$ (for point and spot lights).

**Example:**
```glsl
#define MAX_LIGHT_COUNT 16

struct LightData {
    vec3 position;
    vec3 direction;
    vec3 color;
    vec4 ambientDiffuseSpecularLightType;  // xyz = ambient, diffuse, specular multipliers; w = light type (0 = Directional, 1 = Point, 2 = Spot)
    vec4 cutoffsAndAttenuation;            // x = inner cutoff, y = outer cutoff, z = linear term, w = quadratic term
};

layout(std140, binding = 1) uniform Lights {
    LightData lights[MAX_LIGHT_COUNT];
};
```

---

### 2.2. Uniforms

#### Shadow Maps Array (`location = 2`)
A 2D texture array containing the depth shadow maps for all lights:
- **Type**: `sampler2DArray`
- **Location**: `2`
- **Texture Unit**: `GL_TEXTURE2` (`SHADOWMAPS_TEXTURE_UNIT`)

Each layer index `i` in the texture array corresponds to `lights[i]`.

**Example:**
```glsl
layout(location = 2) uniform sampler2DArray shadowMaps;
```

#### Camera Position (`location = 3`)
The camera's position in world space, used to calculate the view direction vector (`normalize(cameraPos - FragPos)` or `FragPos - cameraPos`):
- **Type**: `vec3`
- **Location**: `3`

**Example:**
```glsl
layout(location = 3) uniform vec3 cameraPos;
```

#### Material Properties (`location = 4` to `7`)
A uniform struct defining the material properties for the mesh. When `layout(location = 4)` is assigned to the `Material` struct, its members are automatically assigned sequential locations:
- **Location 4 (`material.color`)**: Base tint color of the material (`vec3`).
- **Location 5 (`material.diffuseTexture`)**: Diffuse texture sampler (`sampler2D`), bound to texture unit 0 (`GL_TEXTURE0`).
- **Location 6 (`material.specularTexture`)**: Specular map sampler (`sampler2D`), bound to texture unit 1 (`GL_TEXTURE1`).
- **Location 7 (`material.shininess`)**: Specular exponent / shininess factor (`float`).

> **Note**: If a mesh material does not define a diffuse or specular texture, the renderer automatically binds a 1x1 white fallback texture to avoid sampling undefined textures.

**Example:**
```glsl
struct Material {
    vec3 color;
    sampler2D diffuseTexture;
    sampler2D specularTexture;
    float shininess;
};

layout(location = 4) uniform Material material;
```

---

### 2.3. Fragment Outputs
Fragment shaders must output the final color vector:
- `out vec4 FragColor`: Output RGBA color for the fragment.

**Example:**
```glsl
out vec4 FragColor;
```

---

## 3. Quick Reference

### 3.1. Uniform Locations (`ObjectShaderUniformLocation`)

| Location | Identifier | Type | Stage | Bound Texture Unit / Description |
| :---: | :--- | :--- | :---: | :--- |
| **0** | `modelMatrix` | `mat4` | Vertex | Mesh world transformation matrix |
| **1** | `normalMatrix` | `mat3` | Vertex | Inverse transpose of mesh model matrix |
| **2** | `shadowMaps` | `sampler2DArray` | Fragment | `GL_TEXTURE2` (`SHADOWMAPS_TEXTURE_UNIT`) |
| **3** | `cameraPos` | `vec3` | Fragment | World-space camera position |
| **4** | `material.color` | `vec3` | Fragment | Material base color |
| **5** | `material.diffuseTexture` | `sampler2D` | Fragment | `GL_TEXTURE0` (`DIFFUSE_TEXTURE_UNIT`) |
| **6** | `material.specularTexture` | `sampler2D` | Fragment | `GL_TEXTURE1` (`SPECULAR_TEXTURE_UNIT`) |
| **7** | `material.shininess` | `float` | Fragment | Material specular exponent |

### 3.2. Uniform Buffer Object (UBO) Binding Points

| Binding | Block Name | Layout | Used In | Description |
| :---: | :--- | :---: | :---: | :--- |
| **0** | `Matrices` | `std140` | Vertex | Projection matrix (`mat4`) and view matrix (`mat4`) |
| **1** | `Lights` | `std140` | Fragment | Array of 16 `LightData` structures |
| **2** | `directionalShadowMatrices` | `std140` | Vertex | Array of 16 light-space transformation matrices (`mat4`) |

### 3.3. Texture Unit Bindings

| Texture Unit | GL Identifier | Uniform Sampler | Description |
| :---: | :--- | :--- | :--- |
| **Unit 0** | `GL_TEXTURE0` | `material.diffuseTexture` | Diffuse color map (fallback 1x1 white if none) |
| **Unit 1** | `GL_TEXTURE1` | `material.specularTexture` | Specular map (fallback 1x1 white if none) |
| **Unit 2** | `GL_TEXTURE2` | `shadowMaps` | 2D texture array containing shadow maps |
