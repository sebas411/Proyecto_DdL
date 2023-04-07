# Proyecto Diseño de lenguajes de programación

## Laboratorio C

### Dependencias
* GCC (para compilar)

### Compilación y ejecución
Para compilar el programa:  
``` bash
make yalex
```
Para ejecutarlo:  
``` bash
./yalex "filename"
```
Para limpiar los archivos creados por la compilación y ejecución del programa:  
``` bash
make clean
```

#### Notas:
* El programa fue probado y funciona en linux utilizando `g++ (GCC) 12.2.1 20230201`
* Se utiliza el carácter `~` para representar la concatenación. No es necesario añadir este carácter cuando se ingresa la expresión regular o la expresión a evaluar, el algoritmo de preprocesamiento lo añade automáticamente y en el caso de que se ingrese un carácter `~` en la expresión regular este automáticamente es escapado para no ser tomado en cuenta como concatenación.

