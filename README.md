## Proyecto Integrador de Redes y Sistemas Operativos CI-0123

## Integrantes
* Aarón Meza B74787
* Jesús Porras C26007
* Amanda Rodríguez C36725
* Aarón Santana C27373

## Sistema de archivos distribuido (pirofs® para aArt)
* Para este proyecto, se propone una conexión de un cliente y servidor a través de un intermediario, que se denominará Tenedor (o Holder). Este recibe en formato HTTP el pedido del cliente, y seguidamente le transfiere la solicitud al Servidor por medio del protocolo propuesto por el equipo, MKTP (Mouse Ke Tool Protocol). Las especificaciones de este protocolo se pueden encontrar en la carpeta de /design. 


## Compilación
* Con el Makefile facilitado, se puede correr en terminal desde la carpeta del repositorio el comando 'make run' (o make -j run', para más rapidez), y a partir de esto se correrá en bucle las solicitudes del cliente. Se puede ingresar Ctrl+C para acabar con el programa.


## Casos de prueba
Como el código propuesto está en bucle, se pueden hacer cuantas pruebas se deseen ingresando la solicitud. Está diseñado para que se reconozcan pedidos válidos como:
* '/resources' = para retornar el directorio.
* '/resources/..' = para retornar un archivo específico (si existe en tan dirección, si no retorna Not Found)
* otros = para retornar errores de bad request. 
