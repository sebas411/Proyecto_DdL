# Proyecto Diseño de lenguajes de programación

## Laboratorio B

### Dependencias
* GCC (para compilar)
* Graphviz (comando `dot`)

### Compilación y ejecución
Para compilar el programa:  
```
make
```
Para ejecutarlo:  
```
./lexer "[regexp]"
```
Para limpiar los archivos creados por la compilación y ejecución del programa:  
```
make clean
```
### Archivos generados
* graph.dot: grapho del automata creado en notación DOT
* graph.png: imágen del grapho generado por graphviz

#### Notas:
* El programa fue probado y funciona en linux utilizando `g++ (GCC) 12.2.1 20230201`
* Los autómatas generados del pre laboratorio A y B se encuentran bajo la carpeta `prelab`
