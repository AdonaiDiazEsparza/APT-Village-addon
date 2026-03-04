# APT-Villa-addon
Repositorio de los recursos del Addon para el APT Village. Este es un pequeño recuerdo que se les da a las personas que participen en alguno de nuestras platicas.

<center>

<img src=images/Top.jpg width=25%>
<img src=images/back.jpg width=25%>
</center>
## Hardware
- La iluminación consta de 11 Leds (6 de color rojo, 2 de color blanco y 3 traseros de color rojo).
- El chip progragramable es un CH32v003F4P6

## Firmware
### Programación
Para poder programar el chip es necesario utilizar un programador especializado, nosotros recomendamos el programador de WCH-link, pero puede funcionar otro programador que tenga soporte para chips CH32 o para arquitectura RISC-V

### Compilación
Para compilar el firmware del chip usa el comando ```make build``` para generar los archivos ```.bin``` y ```.hex```. 
Es necesario instalar los paquetes necesarios:

```shell
apt-get install build-essential libnewlib-dev gcc-riscv64-unknown-elf libusb-1.0-0-dev libudev-dev gdb-multiarch
```
Si deseas programar el chip utiliza el comando ```make flash```. 
Mas información al respecto del desarrollo de firmware con chips CH32vX, puedes ver los ejemplos en el repositorio de [CH32FUN](https://github.com/cnlohr/ch32fun).