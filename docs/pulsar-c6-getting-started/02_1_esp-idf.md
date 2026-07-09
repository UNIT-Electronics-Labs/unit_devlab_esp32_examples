# Primeros pasos con ESP-IDF

ESP-IDF (Espressif IoT Development Framework) es el framework oficial de desarrollo para los chips de la serie ESP32. Incluye herramientas, librerías y APIs para facilitar el desarrollo de aplicaciones para dispositivos ESP32.

Esta sección ofrece una guía paso a paso para configurar el entorno ESP-IDF para el chip ESP32-C6, incluyendo instalación y ejemplos básicos de uso. Aunque el enfoque está en ESP32-C6, las pautas son aplicables a otros chips ESP32.

**Entorno soportado:** Ubuntu 20.04 o superior.

Para otros sistemas operativos, consulta la documentación oficial de ESP-IDF con las instrucciones específicas de cada plataforma.

<div class="note">

<div class="title">

Nota

</div>

**ESP-IDF** es compatible con Windows y macOS, pero el proceso de instalación puede variar. Consulta la documentación oficial para instrucciones detalladas.

<div style="text-align: center; margin-top: 10px;">
    <p>Consulta la documentación oficial de ESP-IDF para instrucciones por plataforma:</p>
    <a href="https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html" target="_blank" style="text-decoration: none;">
        <button style="background-color: #0078D7; color: white; border: none; padding: 10px 20px; border-radius: 5px; cursor: pointer;">
            Abrir documentación de ESP-IDF
        </button>
    </a>
</div>

</div>

<div class="attention">

<div class="title">

Atención

</div>

Se requiere una conexión estable a internet durante la instalación, ya que algunos pasos descargan archivos necesarios.

</div>

## Pasos de instalación

1.  **Instalar prerrequisitos** Asegúrate de tener instaladas todas las dependencias requeridas. Ejecuta estos comandos en una terminal:

    ``` bash
    sudo apt-get update
    sudo apt-get install git wget flex bison gperf python3 python3-pip python3-setuptools python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util device-tree-compiler
    ```

2.  **Clonar el repositorio ESP-IDF** Clona el repositorio de ESP-IDF desde GitHub. Opcionalmente puedes especificar una versión o rama.

    ``` bash
    git clone https://github.com/espressif/esp-idf.git
    ```

3.  **Configurar el entorno** Entra al directorio clonado de ESP-IDF y ejecuta el script de configuración para preparar las variables de entorno.

    ``` bash
    cd esp-idf
    ./install.sh
    . ./export.sh
    ```

    <div class="note">

    <div class="title">

    Nota

    </div>

    Para instalar herramientas para todos los chips soportados, usa el siguiente comando:

        ./install.sh --all

    </div>

4.  **Instalar herramientas adicionales** Para herramientas específicas de ESP32-C6, ejecuta:

    ``` bash
    ./install.sh --esp32c6
    ```

    <div class="note">

    <div class="title">

    Nota

    </div>

    El script `install.sh` descarga e instala las herramientas y dependencias requeridas para el chip ESP32-C6. La duración depende de la velocidad de internet.

    </div>

5.  **Verificar la instalación** Confirma la instalación revisando la versión de ESP-IDF:

    ``` bash
    idf.py --version
    ```

## Personalización de la ruta de instalación

Para personalizar la ruta de instalación de ESP-IDF, define la variable de entorno `IDF_PATH`. Por ejemplo:

``` bash
export IDF_PATH=/path/to/your/esp-idf
. $IDF_PATH/export.sh
. $IDF_PATH/install.sh
```

<div class="note">

<div class="title">

Nota

</div>

Reemplaza `/path/to/your/esp-idf` por el directorio de instalación deseado. Esto asegura que `IDF_PATH` apunte a la ubicación correcta y que los scripts `export.sh` e `install.sh` se ejecuten desde ahí.

</div>

## Primeros pasos con ESP-IDF

1.  **Crear un proyecto nuevo** Crea un directorio para tu proyecto ESP-IDF y entra en él:

    ``` bash
    mkdir my_project
    cd my_project
    ```

2.  **Generar una aplicación básica** Usa la herramienta `idf.py` para crear una plantilla básica:

    ``` bash
    idf.py create-project my_app
    ```

3.  **Compilar el proyecto** Entra al directorio del proyecto y compila la aplicación:

    ``` bash
    cd my_app
    idf.py build
    ```

4.  **Flashear la aplicación** Conecta la placa ESP32-C6 a la computadora y graba la aplicación:

    ``` bash
    idf.py -p /dev/ttyUSB0 flash
    ```

5.  **Monitorear la salida** Monitorea la salida de la placa ESP32-C6:

    ``` bash
    idf.py -p /dev/ttyUSB0 monitor
    ```

6.  **Modificar el código** Edita el código dentro del directorio `main` de tu proyecto. El archivo principal normalmente se llama `main.c` o `main.cpp`. Después de hacer cambios, vuelve a compilar y flashear.

7.  **Limpiar el proyecto** Para eliminar los artefactos de compilación, ejecuta:

    ``` bash
    idf.py fullclean
    ```

8.  **Actualizar ESP-IDF** Para actualizar ESP-IDF a la versión más reciente, entra al directorio de ESP-IDF y ejecuta:

    ``` bash
    git pull
    ./install.sh
    . ./export.sh
    ```

9.  **Desinstalar ESP-IDF** Para desinstalar ESP-IDF, elimina el repositorio clonado y limpia las variables de entorno relacionadas:

    ``` bash
    rm -rf esp-idf
    unset IDF_PATH
    unset PATH
    unset LD_LIBRARY_PATH
    unset PYTHONPATH
    unset CMAKE_PREFIX_PATH
    ```

10. **Explorar ejemplos de ESP-IDF** El repositorio de ESP-IDF incluye numerosos ejemplos que demuestran distintas funciones. Están en el directorio `examples`; puedes copiar y modificar cualquiera según tus necesidades.

11. **Consultar la documentación de ESP-IDF** Para información completa, referencias de API y guías, visita la [documentación oficial de ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html).

12. **Unirse a la comunidad ESP-IDF** Para soporte o discusiones, participa en la comunidad ESP-IDF en GitHub o en el foro de Espressif. La comunidad es activa y ofrece apoyo sobre distintos temas de desarrollo ESP32.
