# Entorno de escritorio

La configuración del entorno es el primer paso para trabajar con la placa PULSAR C6. Los siguientes pasos te guiarán durante el proceso.

1.  Instalar el software requerido
2.  Configurar el entorno de desarrollo
3.  Instalar las librerías requeridas
4.  Configurar la placa

## Instalar el software requerido

Se requiere el siguiente software para comenzar a trabajar con la placa PULSAR C6:

1.  **Python 3.7 o superior**: Python es necesario para ejecutar scripts y herramientas usadas con la placa PULSAR C6.
2.  **Git**: Git es necesario para clonar repositorios relacionados con la placa PULSAR C6.
3.  **MinGW**: MinGW es una adaptación nativa para Windows de GNU Compiler Collection (GCC), con librerías y archivos de cabecera para compilar aplicaciones nativas en Windows.
4.  **Visual Studio Code**: Visual Studio Code es el editor recomendado para escribir y compilar código.

Esta sección te guía durante la instalación del software requerido.

## Python 3.7 o superior

Python es el lenguaje necesario para ejecutar scripts y herramientas.

Para instalar Python, sigue estas instrucciones:

1.  Descarga el instalador de Python desde:

    > <a href="https://www.python.org/downloads/" target="_blank">sitio web de Python</a>

2.  Ejecuta el instalador y sigue las instrucciones.

<figure>
<img src="/sphinx-static/python.png" class="align-center"
style="width:80.0%" alt="/sphinx-static/python.png" />
<figcaption>Agregar Python al PATH</figcaption>
</figure>

<div class="attention">

<div class="title">

Atención

</div>

Asegúrate de marcar la opción "Add Python to PATH" durante la instalación.

</div>

Abre una terminal y ejecuta el siguiente comando para verificar la instalación:

``` bash
python --version
```

Si la instalación fue correcta, verás el número de versión de Python.

## Git

Git es un sistema de control de versiones necesario para clonar repositorios. Para instalarlo, sigue estas instrucciones:

1.  Descarga el instalador de Git desde

    > <a href="https://git-scm.com/downloads" target="_blank">sitio web de Git</a>

2.  Ejecuta el instalador y sigue las instrucciones.

3.  Abre una terminal y ejecuta el siguiente comando para verificar la instalación:

``` bash
git --version
```

Si la instalación fue correcta, verás el número de versión de Git.

## MinGW

MinGW es una adaptación nativa para Windows de GNU Compiler Collection (GCC), con librerías y archivos de cabecera redistribuibles para compilar aplicaciones nativas. Proporciona un conjunto de herramientas de código abierto para desarrollo en Windows sin depender de DLL externas de C-Runtime. MinGW no intenta ofrecer un entorno POSIX completo; para ese caso se recomienda Cygwin.

Para instalar MinGW, sigue estas instrucciones:

1.  Descarga el instalador de MinGW desde

    > <a href="#" target="_blank">sitio web de MinGW</a>

2.  Ejecuta el instalador y sigue las instrucciones.

<figure>
<img src="/sphinx-static/mingw.png" class="align-center"
style="width:80.0%" alt="/sphinx-static/mingw.png" />
<figcaption>Instalador de MinGW</figcaption>
</figure>

<div class="note">

<div class="title">

Nota

</div>

Durante la instalación, asegúrate de seleccionar los siguientes paquetes:

- mingw32-base
- mingw32-gcc-g++
- msys-base

</div>

<figure>
<img src="/sphinx-static/mingw2.png" class="align-center"
style="width:80.0%" alt="/sphinx-static/mingw2.png" />
<figcaption>Instalación de MinGW</figcaption>
</figure>

3.  Abre una terminal y ejecuta el siguiente comando para verificar la instalación:

``` bash
mingw --version
```

Si la instalación fue correcta, verás el número de versión de MinGW.

### Configuración de variables de entorno

En Windows es necesario un paso adicional: abrir las variables de entorno y editar la variable de entorno:

    C:\MinGW\bin

### Localizar el archivo

Después de instalar MinGW, localiza el archivo `mingw32-make.exe`. Normalmente se encuentra en `C:/MinGW/bin`. Una vez localizado, renómbralo como `make.exe`.

> Localización del archivo `mingw32-make.exe`

### Renombrarlo

Después de localizar `mingw32-make.exe`, renómbralo como `make.exe`. Este cambio es necesario porque muchos scripts de compilación esperan que el comando se llame `make`.

> Renombrar `mingw32-make.exe` a
> `make.exe`

<div class="warning">

<div class="title">

Advertencia

</div>

Si encuentras problemas, crea una copia del archivo y renombra esa copia como `make.exe`.

</div>

### Agregar la ruta a la variable de entorno

Luego agrega la ruta del directorio `bin` de MinGW a las variables de entorno del sistema. Esto permite que el comando `make` sea reconocido desde cualquier terminal.

1.  Abre el buscador de inicio, escribe "env" y selecciona "Editar las variables de entorno del sistema".

2.  En la ventana de Propiedades del sistema, haz clic en "Variables de entorno".

3.  En Variables de entorno, dentro de "Variables del sistema", selecciona "Path" y haz clic en "Editar".

4.  En la ventana de edición, haz clic en "Nuevo" y agrega la ruta:

        C:\MinGW\bin

    Agregar el directorio bin de MinGW a las variables de entorno

## Visual Studio Code

Visual Studio Code es el editor recomendado para escribir y compilar el código.

Para instalar Visual Studio Code, sigue estas instrucciones:

1.  Descarga el instalador de Visual Studio Code desde

    > <a href="https://code.visualstudio.com/download" target="_blank">sitio web de Visual Studio Code</a>

2.  Ejecuta el instalador y sigue las instrucciones.

<figure>
<img src="/sphinx-static/vscode.png" class="align-center"
style="width:80.0%" alt="/sphinx-static/vscode.png" />
<figcaption>Instalador de Visual Studio Code</figcaption>
</figure>

<div class="note">

<div class="title">

Nota

</div>

Durante la instalación, asegúrate de marcar la opción "Open with Code".

</div>

3.  Abre una terminal y ejecuta el siguiente comando para verificar la instalación:

``` bash
code --version
```

4.  Instala extensiones para Visual Studio Code:

    > <figure>
    > <img src="/sphinx-static/vscode_gf.png" class="align-center"
    > style="width:80.0%" alt="/sphinx-static/vscode_gf.png" />
    > <figcaption>Extensiones de Visual Studio Code</figcaption>
    > </figure>

## Instalación del Arduino IDE

Arduino IDE es una plataforma de código abierto muy usada para construir y programar proyectos basados en microcontroladores. Ofrece una interfaz amigable y muchas librerías que simplifican el desarrollo.

Para instalar Arduino IDE, sigue las instrucciones correspondientes a tu sistema operativo en la

<a href="https://wiki.uelectronics.com/tutoriales/inicio-arduino" target="_blank">guía de instalación de paquetes de UNIT Electronics</a>.

## Instalación de Thonny IDE

Thonny es un IDE de Python diseñado para principiantes. Tiene una interfaz simple y soporte integrado para MicroPython, por lo que es una buena opción para programar la placa PULSAR C6.

Sigue las instrucciones correspondientes a tu sistema operativo en la

<a href="https://wiki.uelectronics.com/tutoriales/micropython" target="_blank">guía de compatibilidad de MicroPython</a>.
<br> </br>
