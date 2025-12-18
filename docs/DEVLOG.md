# Devlog

## log 1
buildt 2 difrent systems, one clones a project template and has commands like create project, compile project, run project, set build commands, clear, exit.
the other is the actual project template, for now its just a basic OpenGL window. 

## log 2 
started work on the actual engine so far its just some basic rendering, features i have created so far are LOD system, frustum culling, Octree data structure, and inctence rendering / batch. also creted an optional fly cam module and an input mannager. started on a more detailed doc page: [View](HAND-GUIDE.md)

## log 3
build an engine singleton to help orginize and simply fly code, my goal is for almost everyting to be accsesed thure the engine:: no feature should have to be hard coded in, use hard code you own logics and any actual game / rendering feature is accses from engine::. i optimized the octree with some new features, like AABB queries and spheare based queries this helps for spesific features who require a more direct spatal form.  