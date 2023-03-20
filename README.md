# Graphics Project

## Building

### Windows (Visual Studio 2022)
```bat
git clone https://github.com/ronejfourn/graphics-project
cd graphics-project
.\setup.bat
.\graphics-project.sln
```
Visual Studio should open up and you can build/run the solution from there.

### Linux
```sh
git clone https://github.com/ronejfourn/graphics-project
cd graphics-project
chmod +x setup.sh
./setup.sh
make
cd build
./debug/graphics-project
```

## Controls
`W` or `Up`    move forwards
`S` or `Down`  move backwards
`A` or `Left`  move left
`D` or `Right` move right

`H` pan left
`J` tilt down
`K` tilt up
`L` pan right

`F` fast forward
`R` toggle sun movement
`T` toggle wireframe/fill

`1` toggle environment mapping
`2` toggle lighting
`3` toggle shadows
`4` toggle ambient occlusion
`5` toggle fog
`6` toggle back face culling
