## Funcionamiento y conexiones del servidor, tenedor y cliente
Antes de poder iniciar pedidos desde el cliente, se sabe que tanto el servidor como el tenedor deben haberse iniciado y conectado correspondientemente. Luego de que estos hayan sido conectados, el cliente puede enviar solicitudes.
![](img/connections.png)


Sin embargo, entre tenedor y servidor no existe una jerarquía que defina cuál de los dos pueda ejecutarse primero. En tal caso, es necesario diseñar una comunicación dual que pueda soportar que la otra parte esté conectada o desconectada. Para ello, se propone un flujo sencillo de la inicialización de programas, que aplicaría tanto para el servidor como para el tenedor, siendo A y B las partes involucradas.
![](img/diagram.png)


Siguiendo el diagrama:
* Una vez que se corre, A le envía un mensaje a B y espera. Si el mensaje fue recibido correctamente (retorna 0), significa que B está conectado también.
* Si el mensaje enviado no fue recibido, B no está disponible, por lo que A deberá aguardar pasivamente hasta recibir un mensaje de B.
* Si recibe un mensaje se habrá conectado exitosamente.
* A partir de este momento, A y B se quedarán inactivos (pero corriendo) esperando solicitudes de parte de algún cliente que se conecte al servidor.

## Propuesta de protocolo
Para desarrollar este proyecto, se plantea la idea de una comunicación simple e intuitiva, como va siendo la de HTTP. En esta ocasión, se necesita una funcionalidad general de leer mensajes recibidos, que incluirán el archivo .txt si la solicitud fue válida, o bien, un mensaje de error en caso contrario.

### Cliente
Desde el punto de vista del cliente, existirán dos tipos de mensajes a enviar:
* **ALL**: Solicitar una lista completa de todos los archivos .txt disponibles (es decir, al alcance del tenedor, que a su vez busca en los servidores a los que está conectado).
* **ONE [_file.txt_]**: Solicitar directamente un archivo .txt específico. Aquí, se retornaría el contenido entero del archivo pedido. El formato que se esperaría a partir de esta bandera sería ONE archivo.txt. Si no se especifica el nombre del archivo, podría descartarse la solicitud, o bien, retornar el primer archivo hallado en el directorio por defecto.

### Servidor
El servidor, como no tiene comunicación directa con el cliente, solo envía mensajes al tenedor, que pueden ser los siguientes:
* **FULL [_response_]**: en este caso indicaría que recibido un pedido válido y le enviará de vuelta al tenedor su directorio o bien el contenido del archivo pedido.
* **NONE**: en caso de haber solicitado un archivo en particular y que no esté en su directorio.
* **FAIL**: recibió un pedido inválido (esto es provisional pues no debería suceder, dado que el tenedor sirve como muro que captura errores)

### Tenedor
El tenedor podrá acceder a buscar lo solicitado en los servidores disponibles, enviando estos mensajes:
* **PULL**: Si recibió ALL del cliente, el tenedor jalará el directorio del servidor actual para mostrarle después la lista al cliente
* **UNIT [_file.txt_]**: Si recibió ONE del cliente, para este caso también jalará el directorio personal del servidor, con tal de averiguar si el archivo existe o no en él. Si existe, también jalará su contenido.

Por otro lado, como retorno al cliente el tenedor podrá enviarle lo siguiente:
* **DONE [_contents_]**: si recibe FULL del servidor, obtuvo exitosamente lo que se le solicitó. Esta bandera iría seguida del contenido requerido, ya sea la lista de archivos o el contenido de un archivo en particular.
* **FAIL**: hubo alguna falla con la solicitud, sea por un error en la conexión o porque se digitó un pedido inválido. El servidor le envió FAIL.
* **NONE**: es decir, el archivo solicitado en ONE no existía o no estaba disponible. El servidor había retornado NONE.
_Nota_: Para simplificar, las últimas dos son banderas coincidentes entre servidor y tenedor para transmitir el mensaje al cliente.

Se muestran casos potenciales de solicitudes de cliente:
**1)** Solicitud de lista
![](img/all.png)

**2)** Solicitud de archivo
![](img/one.png)


**3)** Solicitud no hallada
![](img/none.png)

**4)** Pedido inválido
![](img/error.png)