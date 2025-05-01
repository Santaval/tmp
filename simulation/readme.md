# Simulación de Comunicación Cliente-Holder-Servidor

##  Descripción
Sistema simulado con tres componentes principales que se comunican mediante buffers compartidos y semáforos:

- **Cliente**: Interfaz de usuario para enviar solicitudes
- **Holder**: Intermediario que gestiona la comunicación
- **Servidor**: Procesa solicitudes y devuelve recursos


## Protocolo de Comunicación
### Formatos de Mensaje
| Dirección | Formato |
|-----------|---------|
| Cliente → Holder | `BEGIN/GET/<recurso>/END` |
| Holder → Servidor | `BEGIN/GET/<recurso>/END` o `BEGIN/OBJECTS/END` |
| Servidor → Holder | `BEGIN/OK/<data>/END` o `BEGIN/ERROR/<code>/<msg>/END` |

### Buffers Compartidos
| Buffer | Claves Usadas |
|--------|---------------|
| client_holder | `CLIENT_REQUEST`, `CLIENT_RESPONSE` |
| holder_server | `SERVER_REQUEST`, `SERVER_RESPONSE` |

## Compilación y Ejecución

    make        
    make run    
    make clean

## Solicitud de archivos

Al momento de ejecutar el programa se mostrará el siguiente mensaje:

    [*] Enter HTTP command:

Para solicitar el directorio con las imágenes se debe ingresar:

    OBJECTS

Para solicitar una imagen se debe ingresar el nombre del archivo, por ejemplo:

    MichaelMouse.txt