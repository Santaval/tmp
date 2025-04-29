# File System

## Requerimientos

- Una interfaz que pueda
   - realizar consultas al directorio
   - listar los bloques de los archivos
   - recuperar los archivos
- Un programa de file system que pueda
   - leer el file system si existe
   - retornar el directorio
   - retornar un archivo solicitado
   - agregar archivos al file system
   - eliminar archivos del file system

La parte de agregar y eliminar se no se implementa el la interfaz, solo en el
modelo de datos.

## Diagrama de Clases

Para detallar el programa, se crea el siguiente diagrama de clases que ilustra
los componentes del programa y cuáles son sus responsabilidades.

TODO(jesus): agregar imagen del diagrama de clases.

## Pseudocódigo

Para ilustra el funcionamiento general del programa, se provee un pseudocódigo
que representa las acciones y lógica principal del programa.

### Interface
``` pseudo
start main function
    analyze_arguments()
    file_sys <- init_file_system()
    directory <- file_sys.getDirectory()
    print_directory(directory)
    while true
        request <- input
        if input = EXIT_REQUEST
            break
        else if directory.contains(request)
            print(file_sys.getFile(request))
        else
            file not found

        end if
    end while
end main function
```