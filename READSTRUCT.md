/*
## 📂 Estructura del Proyecto

```txt
├── build/
│   ├── os/             # Build del núcleo del sistema operativo
│   ├── process1/       # Build de procesos adicionales
│   └── process2/       # Build de procesos adicionales
├── os/
│   ├── main.c          # Punto de entrada principal del SO
│   ├── root.s          # Código en ensamblador, arranque e interrupciones
│   ├── memmap.ld       # Mapa de memoria para el linker
│   └── core/
│       ├── os.c/h      # Funciones base como rand y delay
│       ├── tasks.c/h   # Inicialización de tareas y estructuras PCB
│       ├── sched.c/h   # Cambio de contexto
│       └── timer.c/h   # Configuración del temporizador y su IRQ
├── drivers/
│   ├── uart.c/h        # Funciones de entrada/salida UART
│   └── io.c/h          # Acceso de bajo nivel PUT32 / GET32
├── lib/
│   ├── stdio.c/h       # PRINT, READ y parsing tipo printf/scanf
│   ├── string.c/h      # Conversión de string <-> número
│   └── console.c/h     # Entrada de línea desde UART (get_line)
```

## Componentes clave

- **`main.c`**: inicia las tareas y habilita interrupciones.
- **`root.s`**: define el `_start`, tabla de vectores e IRQ handler en ARM.
- **`os/core/`**: lógica principal de temporización, planificación y definición de tareas.
- **`drivers/`**: interfaces básicas para UART y acceso a memoria.
- **`lib/`**: utilidades reutilizables para entrada/salida e interpretación de datos.

## Build
Cada módulo tiene su propio `build.sh`, y el del SO se encuentra en `build/os/build.sh`. Este script:

- Ensambla `root.s`
- Compila todos los `.c` correspondientes
- Enlaza usando `memmap.ld`
- Genera binario (`main.bin`) y desensamblado (`main.list`)

## Dependencias
- `arm-none-eabi-gcc`
- `arm-none-eabi-as`
- `arm-none-eabi-objcopy`
- `arm-none-eabi-objdump`

## 📦 Output esperado
- `main.elf`: binario enlazado
- `main.bin`: imagen cruda para flasheo
- `main.list`: lista desensamblada para depuración

*/
