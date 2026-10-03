# OpenGL-Renderer

This is a simple 3d renderer using OpenGL that I have been working on as a hobby project.

## Features
  - **Dynamic Lighting**:
    - Includes directiona, spot, and point lights, with configurable color and intensity 
  - **Shadow Mapping**:
    - Directional & Spot Shadows
    - Omnidirectional Point Light Shadows
  - **Model Loader**:
    - Load almost any 3D model, powered by Assimp
  - **Skybox**:
    - Cubemap skybox rendering
   
## Planned Features
  - **Physically Based Rendering (PBR)**
  - **Cascaded Shadow Maps (CSM)**
  - **Post-Processing**
  - **Performance Optimizations**:
    - Frustum culling
    - Instanced rendering.
   
  ## Loading Your Own Models

  You can easily preview your own 3D models in the renderer:

  1. Place your model files and associated textures in a folder under the `models/` directory (e.g., `models/MyModel/MyModel.gltf`).
  2. Supported formats include **`.obj`**, **`.gltf` / `.glb`**, **`.fbx`**, **`.dae`**, **`.blend`**, **`.stl`**, and **`.ply`**.
  3. In the application, click **"Rescan Models Folder"** under the *Model Selection* header in the UI.
  4. Select your model from the dropdown to load and inspect it

  ---

## Images

<img width="2557" height="1353" alt="Screenshot_20261003_164403" src="https://github.com/user-attachments/assets/2354f1d1-2fca-452e-a869-5e9ff7ceb87c" />
<img width="2560" height="1352" alt="Screenshot_20261003_164003" src="https://github.com/user-attachments/assets/5ede4ac2-929d-4279-bc5a-b4589263a2fa" />
