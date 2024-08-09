## Path Tracer

A CPU path tracer written in C++ that renders 3D scenes using the Monte Carlo method.  
It is strongly based on the book [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html) by Peter Shirley, but it also supports some additional features such as Phong shading and OBJ file loading.  
A more detailed description of the project can be found in the [report](docs/PathTracer.pdf).

![cover](assets/raytracer.png?raw=true "cover")


---
### Install and Run

This project uses the header-only libraries GLM and TinyObjLoader, both included in the lib/ directory. The compilation is done with g++ and the build is handled by make, using the Makefile provided in the root directory.   Enabling OpenMP to run the code in multi-threaded mode can drastically reduce the time taken to render images.

    make          # compile code and generate binary in build/raytracer
    make run      # run in single-thread
    make run_mt   # run in multi-thread using OpenMP

The output image will be saved as `build/output.ppm`.

Multiple scenes are available in the `src/main.cpp` file. To render a different one, change the `scene` variable in the `main` function and recompile the code.

---
### Hybrid Path Tracing

The path tracer supports two modes of operation: pure path tracing and hybrid path tracing.  
The pure path tracer uses only Physics-Based-Rendering (PBR) materials, which are more accurate but computationally expensive.
The hybrid mode mixes path tracing materials with Phong materials, which are faster to compute.  

The following types of materials are supported:
* PBR materials: Diffuse, Metal, Dielectric.
* Phong: Phong (diffuse + specular + ambient), PhongMirror (reflective).

![hybrid_path_tracer](assets/hybrid_path_tracer.png?raw=true "hybrid_path_tracer")


---
### Lights

This implementation supports three types of lights: area, point and ambient.
Point and area lights are achieved by adding the LightMat material to any object in the scene. Area lights produce soft shadows and are more realistic than point lights, but they are also more computationally expensive. 

![lights](assets/lights.png?raw=true "lights")


---
### Examples

![example3](assets/example3.png?raw=true "example3")
![example1](assets/example1.png?raw=true "example1")
![example2](assets/example2.png?raw=true "example2")
![example4](assets/example4.png?raw=true "example4")
