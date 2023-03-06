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
`W` move forwards  
`S` move backwards  
`A` move left  
`D` move right  

`H` pan left  
`J` tilt down  
`K` tilt up  
`L` pan right

`F` fast forward  
`T` toggle wireframe/fill
