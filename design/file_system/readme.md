# File System

## Almacenamiento
Los bloques se asignan inicializados en 0.

Se utiliza memoria continua.

El bloque 0 se deja vacío y se utiliza para indicar un puntero nulo.

El directorio se ubica en el bloque 1.

Almacenamiento: medio megabyte = 512 kB

Bloques de 256 bytes

2048 bloques en total

Con 2 bytes basta para indexar el bloque más grande
- (2048 < 65536 = 2^16)

![Imagen del diagrama mostrando el file system con un archivo y las estructuras de datos](./diagrams/file_system_diagram.png)


## Directorio
Se diseñó la siguiente estructura de datos para almacenar el directorio.

Un bloque que contiene una tabla para guardar información de archivos que cada uno suma 36 B en total.
- nombre (32 B)
- bloque de inicio (2B)
- bloque final (2B)

Con 7 directorios se ocupa 252 B. Quedan 4 bytes libres que se pueden usar como punteros a otras estructuras iguales de ser necesarias. Esto le permite al directorio crecer dinámicamente.

Si no hay otros directorios, apuntan al bloque 0.

