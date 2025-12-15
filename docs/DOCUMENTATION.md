# Solheim Studios - Documentation

# Rendering

# Optimization
So the engine used a bunch of different Optimization technics, mostly to improve rendering, tick speed, or memory. bellow is a more detailed view of how they work and what they actually do. The list below contains many features not all are yet or ever to be implemented.


### Frustum culling
**type: CPU**

**Impact: ★★★★☆**

Frustum culling is used to prevent rendering of everything outside the camera view. Effectively reducing draw calls and GPU workload.

### Occlusion culling
**type: CPU / GPU**

**Impact: ★★★★☆**

Skips hidden objects. If an object is hidden behind a wall, we skip its rendering. Center as it adds CPU/GPU overhead for visibility checks, so if this culling is really needed, it might be counterproductive.

### Portal culling 
**Tag: CPU**

**Impact: ★★★☆☆**

Culling objects based on segmented environments like caves or indoors. Visibility is determined by recursively rendering through connected portals. Objects outside of portals are then culled.

### Octree
**Tag: CPU**

**Impact: ★★★☆☆**

Octree is a form of spatial partitioning that subdivides 3d pace in to cubes. It's used to speed up Frustum culling, collision, ray cast, and visibility checks. It does this by limiting the actions operation to its relevant spatial region. Picture splitting the world in to section each section being split in 8 (an octagon), hens the name OC tree. OC for octagon.

### Batch rendering
**Tag: CPU / GPU**

**Impact: ★★★★★**

batch rendering is super important in larger renders. Its a technic used to combind objects into a single draw call when they share render state, shaders, and materials. Minimizes CPU to GPU overhead from excessive draw calls.

### Map Persistent buffers
**Tag: CPU / GPU**

**Impact: ★★★☆☆**


### Mipmapping

### Texture compression

### Anisotropic filtering

### CPU Multithreading / asynchrony

### GPU Asynchrony

### LOD Usage

### shader cache

### UBOs

### Forward+

### GPU Instancing

### Depth Pre-Pass

### Meshlet

### Temporal Upscaling
