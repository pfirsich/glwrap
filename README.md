# glwrap
This is a C++ OpenGL wrapper mostly for personal use. It is not intended to abstract away OpenGL, but just make it more usable in C++ and in general. If you wish to use this, you still need to know how OpenGL works.

## glw
The `glw` namespace consists of **thin** wrappers over OpenGL objects (so thin that there should be little dispute about design choices). They can also store state that is stored inside the OpenGL object, so you can query it easily (e.g. for buffers: size of data store, for shader programs: attached shaders, for textures: tons of shit). Most of these objects are non-copiable and non-assignable, because the OpenGL objects they own are sort of like pointers in that you should only delete them once and they can't be copied easily. It contains the following classes:
* [Buffer](include/buffer.hpp) (Buffer Objects)
* [Framebuffer](include/framebuffer.hpp) (Framebuffer Objects)
* [Renderbuffer](include/renderbuffer.hpp) (Renderbuffer Objects)
* [Shader & ShaderProgram](include/shader.hpp) (Shader and Program Objects)
* [State](include/state.hpp) (A manager for some of OpenGLs global state)
* [Texture](include/texture.hpp) (Texture Objects)
* [VertexArray](include/vertexarray.hpp) (Vertex Array Objects)
* [VertexFormat](include/vertexformat.hpp) (oops, this should be in glwx, but I won't change it now)

and a bunch of enums and logging.

## glwx
The `glwx` namespace contains mostly high-level stuff that I need for most projects using OpenGL. Especially helpers to create the objects listed above (including from filesystem). The idea is to rather have not enough than too much (and introduce too much abstractions/design choices/opinions). The goal is still (for this whole library) to keep it as generic as I can, but include everything that I need all the time.
It includes:
* Helper classes:
    - [Aabb](include/glwx/aabb.hpp)
    - [Transform](include/glwx/transform.hpp)
* Higher-Level wrappers:
    - [DefaultBuffer, BufferData, VertexBuffer, IndexBuffer](include/glwx/buffers.hpp)
    - [RenderTarget](include/glwx/rendertarget.hpp)
    - [Primitive](include/glwx/primitive.hpp), [Mesh](include/glwx/mesh.hpp)
* Object creation helpers:
    - [makeQuadMesh, makeBoxMesh, makeSphereMesh](include/glwx/meshgen.hpp)
    - [makeShader, makeShaderProgram](include/glwx/shader.hpp)
    - [makeTexture, makeCubeTexture](include/glwx/texture.hpp)
* Window creation with SDL2 ([header](include/glwx/window.hpp))
* Helpers for OpenGL's debug API ([header](include/glwx/debug.hpp))
* Some math functions

## To Do
* The `Mesh` class is pretty useless, but the idea is to provide a `Primitive` that also owns it's buffers. In my engines at least the Buffers are often shared though, so maybe I want the mesh to have a `vector<shared_ptr<Buffer>>`? Most of the time you want a mesh to represent multiple drawcalls anyways. Would I need some Material abstraction to make this useful? In that case this would be entirely inappropriate for this library. Maybe I should just delete Mesh? Then I can't really use meshgen though.
* Support ivec/dvec vertex attributes (glVertexAttribIPointer/glVertexAttribLPointer)
* Text Rendering?
* Imgui integration? (this is very simple to do in a project though)
