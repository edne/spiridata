# Spiridata
a spiridatic machine


## Build
- Download and install OpenFramework
- Build `apps/projectGenerator/commandLine` in the OF directory and add it to
  your PATH
- Clone [ofxImGui](https://github.com/jvcleave/ofxImGui) in the `addons/` folder.
- Run `projectGenerator -a"ofxImGui" -f -o"your/openframeworks/path" .`
- `make`
- `make run`


## Quick examples
Work in progress, expect changes.  
(comma is whitespace, use suggested to improve readability)

Draw a cube
```
cube .
```

Draw a cube scaled by half
```
cube 0.5 scale .
```

Draw a cube scaled by the vaue of a slider named `:s`
```
cube :s slider scale .
```

Draw a cube and a square
```
cube square .
```

Draw a cube and a square, just the square is scaled
```
cube, square 0.5 scale .
```

Draw a cube and a square, both scaled
```
cube square ] 0.5 scale .
```

Draw a triangle
```
3 polygon .
```

Define a `triangle` command, and then use it
```
3 polygon @triangle
triangle .
```

Draw a cube on a framebuffer named `:layer`
```
cube :layer to
```

Draw the framebuffer named `:layer`
```
:layer from .
```

Video feedback
```
:layer from, square ] :layer to, :layer from .
```
