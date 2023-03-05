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
./lexer "regexp" ["input"]
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
* Se utiliza el carácter `~` para representar la concatenación. No es necesario añadir este carácter cuando se ingresa la expresión regular o la expresión a evaluar, el algoritmo de preprocesamiento lo añade automáticamente y en el caso de que se ingrese un carácter `~` en la expresión regular este automáticamente es escapado para no ser tomado en cuenta como concatenación.
* Las imágenes representantes de los autómatas se generan en la raíz del directorio con los siguientes nombres:
    * `nfa.png`: AFN
    * `dfa_sc.png`: AFD construcción por subconjuntos
    * `dfa_min_sc.png`: AFD construcción por subconjuntos minimizado
    * `dfa_dc.png`: AFD construcción directa
    * `dfa_min_dc.png`: AFD construcción directa minimizado
