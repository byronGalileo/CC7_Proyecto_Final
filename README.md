# Sistema Operativo Bare-Metal - BeagleBone Black

##  Objetivo del proyecto

Este proyecto tiene como propósito construir un sistema operativo mínimo que corra directamente sobre el hardware del BeagleBone Black (basado en el procesador AM335x), sin depender de un sistema operativo previo o un entorno como Linux. El sistema se ejecuta en modo bare-metal, lo que implica control total del hardware, incluyendo memoria, interrupciones, y periféricos como UART y temporizadores.

En esta fase inicial se logró:

- Configurar el entorno de ejecución en modo Supervisor e IRQ.
- Establecer correctamente las direcciones de memoria para código, stack del sistema y stack de interrupciones.
- Configurar el temporizador DMTIMER2 para generar interrupciones periódicas.
- Capturar y manejar correctamente interrupciones en ARM.
- Implementar comunicación UART para propósitos de depuración y monitoreo.

---

## Organización de memoria

La memoria fue segmentada para cumplir con los requerimientos de diferentes modos de ejecución del procesador ARM (Supervisor, IRQ) y evitar solapamientos. Se utilizó un linker script (`memmap.ld`) que define las siguientes áreas:

```
Dirección        Región             Propósito
───────────────  ─────────────────  ──────────────────────────────────────
0x80000000       OS_CODE            Código del sistema operativo (64KB)
0x80007000       IRQ_STACK          Stack exclusivo del modo IRQ (4KB)
0x80008000       OS_STACK           Stack del modo Supervisor (4KB)
0x80010000       PROCESS1_CODE      Código del proceso de letras
0x80018000       PROCESS1_STACK     Stack del proceso 1
0x80020000       PROCESS2_CODE      Código del proceso de números
0x80028000       PROCESS2_STACK     Stack del proceso 2
```

Cada modo del CPU ARM posee su propio registro de stack pointer (`sp`). Si no se configura adecuadamente, cualquier operación de `push` durante una interrupción (modo IRQ) puede causar fallos.

---

##  Inicialización del sistema

Al arrancar el sistema, se realiza lo siguiente en el código ensamblador (`root.s`):

1. **Configuración del stack para modo Supervisor** (`_os_stack_top`).
2. **Cambio temporal al modo IRQ** y configuración de su stack (`_irq_stack_top`).
3. **Regreso a modo Supervisor** y habilitación global de interrupciones (`cpsie i`).
4. **Definición del vector de interrupciones** mediante `mcr p15, 0, r0, c12, c0, 0`.
5. **Llamada a `main()`** para iniciar el flujo principal en C.

---

##  Implementación de switching de tareas

### PCB: Process Control Block
Cada tarea en el sistema es representada mediante una estructura `PCB`:

```c
typedef struct {
    int pid;
    unsigned int *sp;
    unsigned int *stack;
    int state;
} PCB;
```

Esta estructura contiene el identificador, puntero de stack actual, dirección de stack y su estado. Se define un arreglo global `pcb[NUM_TASKS]` y una variable `current_task` para llevar el seguimiento del proceso activo.

### Función `init_task_stack`
Inicializa la pila de cada proceso con un contexto simulado: registros, cpsr, dirección de retorno (`lr`) y punto de entrada (`pc`). Deja `sp` apuntando al tope listo para restaurar.

```c
void init_task_stack(PCB *task, void (*entry)(void), unsigned int *stack_top);
```

### Función `context_switch`
Aplica Round-Robin simple para alternar entre procesos:

```c
void context_switch(void) {
    current_task = (current_task + 1) % NUM_TASKS;
}
```

El `irq_handler` guarda el contexto actual en `pcb[current_task].sp`, llama a `context_switch()`, y restaura el stack del nuevo proceso desde `pcb[new_task].sp`, usando instrucciones `str sp, [r1]` y `ldr sp, [r1]`.

---


## Problemas encontrados y soluciones

### 1.  El sistema se colgaba en `irq_handler`

**Síntoma:**  Solo se imprimía un carácter `'A'` en UART, indicando que la ejecución se detenía al hacer `push` dentro del manejador.

**Causa:**  El modo IRQ no tenía configurado su propio stack pointer (`sp_irq`). Por defecto, ARM lo deja apuntando a dirección 0 o basura.

**Solución:**
- Se definió una región `IRQ_STACK` en el linker script.
- Se agregó en `_start` el código que cambia a modo IRQ y configura `sp`.

---

### 2.  Las interrupciones no se disparaban

**Síntoma:**  A pesar de que el temporizador se inicializaba, no se veía la salida "[IRQ] Tick!".

**Causa:**  La interrupción ocurría, pero el CPU no podía ejecutar correctamente el `irq_handler` debido al error anterior del stack IRQ.

**Solución:**  Corregido al establecer `sp_irq` en `_start`, con lo que la instrucción `push {r0-r12, lr}` ya fue válida y se ejecutó correctamente.

---

### 3.  Uso inseguro de `uart_putc` y `uart_puts` dentro de interrupciones

**Síntoma:**  Incluso después de corregir el stack del modo IRQ, al llamar a `uart_putc` desde `timer_irq_handler`, el sistema se colgaba o entraba en bucles inesperados.

**Causa:**  Las funciones `uart_putc` y `uart_puts` hacían polling (espera activa) verificando registros como `UART_LSR`, pero el compilador introdujo instrucciones adicionales como:

```asm
ldrb r0, [r4, #1]!
cmp r0, #0
bne ...
```

Esto generaba bucles infinitos si el hardware no respondía como se esperaba o si se ejecutaban operaciones UART complejas desde una IRQ sin garantizar contexto válido o sincronización.

**Solución:**
- Se forzó el uso de `__attribute__((noinline))` para evitar que el compilador embebiera lógica indeseada dentro del handler.
- Se reemplazaron llamadas complejas por rutinas UART mínimas escritas en ensamblador directamente dentro de `irq_handler`.
- Se evitó el uso de `uart_puts` dentro de interrupciones críticas, delegando solo a funciones directas tipo `str r1, [r0]` para enviar un carácter.

---

##  Estructura del Proyecto

```txt
├── build/
|   ├── bin/            # Output bin files
│   ├── os/             # Build del núcleo del sistema operativo
│   ├── process1/       # Build de procesos adicionales
│   └── process2/       # Build de procesos adicionales
├── os/
│   ├── main.c          # Punto de entrada principal del SO
│   ├── root.s          # Arranque, IRQs y vectores en ARM ASM
│   └── memmap.ld       # Mapa de memoria
├──core/
│   ├── os.c/h          # delay_loop, rand, utilidades base
│   ├── tasks.c/h       # Inicialización de PCBs y stacks
│   ├── sched.c/h       # Scheduling simple round-robin
│   └── timer.c/h       # Timer DMTIMER2 y configuración de IRQ
├── process_letters/
│   ├── process1.c      # Código que imprime letras a-z
│   ├── memmap.ld       # Define código en 0x80010000 y stack en 0x80018000
│   └── root.s          # Define _start y stack local del proceso
├── process_numbers/
│   ├── process2.c      # Código que imprime números 0-9
│   ├── memmap.ld       # Define código en 0x80020000 y stack en 0x80028000
│   └── root.s          # Define _start y stack local del proceso
├── drivers/
│   ├── uart.c/h        # Comunicación serial UART
│   └── io.c/h          # Funciones PUT32 / GET32
├── lib/
│   ├── stdio.c/h       # PRINT y READ vía UART
│   ├── string.c/h      # itoa, atoi, atof y utilidades
│   └── console.c/h     # Lectura de línea desde UART
```

---

## Descripción de componentes

Cada carpeta del proyecto está nombrada en función de su propósito específico dentro del sistema:

### `os/`
Contiene el sistema operativo central, con su punto de entrada principal (`main.c`), el código de arranque (`root.s`) y el linker script (`memmap.ld`).

### `core/`
 Dentro de `core/`, se agrupan funcionalidades específicas del kernel:
- `tasks/` define la estructura y stack de procesos.
- `sched/` gestiona el cambio de tareas (`context_switch` y `context_switch_and_run`) y define la tarea actual con `current_task`.
- `timer/` configura el temporizador e IRQ.
- `wd/` desactiva el watchdog timer que, por defecto, reinicia el sistema en el BeagleBone Black.
- `procesor/` implementa manejo de excepciones (`svc_handler`, `data_abort_handler`) y utilidades como `print_cpu_mode()` para debug.


### `process_letters/` y `process_numbers/`
Estas carpetas contienen procesos independientes que serán ejecutados por el sistema operativo. Cada uno tiene:
- Un `memmap.ld` que lo posiciona en memoria de forma fija.
- Un `root.s` para configurar su propio stack.
- Un `.c` con su lógica: `letters` imprime caracteres de la 'a' a la 'z'; `numbers` imprime del 0 al 9.

El nombre `process_letters` y `process_numbers` refleja la tarea específica de cada programa, facilitando su identificación en el sistema y la depuración.


### `drivers/`
Abstracción de acceso a hardware:
- `uart/`: comunicación serial.
- `io/`: acceso crudo a memoria.

### `lib/`
Contiene bibliotecas reutilizables:
- `stdio`: printf/scanf simplificados.
- `string`: manejo básico de cadenas.
- `console`: entrada de línea desde UART.


---

##  Build

Cada módulo tiene su propio `build.sh`, y el del SO se encuentra en `build/os/build.sh`. Este script:

- Ensambla `root.s`
- Compila todos los `.c` correspondientes
- Enlaza usando `memmap.ld`
- Genera binario (`main.bin`) y desensamblado (`main.list`)

###  Estructura de salidas:
Los binarios generados por cada proceso y por el sistema operativo son almacenados en `build/bin/`, lo que permite centralizar todos los `.bin` en una sola carpeta accesible para flasheo y pruebas.

---

## Validación y pruebas

Para depurar y verificar el sistema, se usaron múltiples estrategias:

- **Prints progresivos en `irq_handler`** para validar el flujo completo del handler (inicio, guardado de contexto, llamada a `timer_irq_handler`, cambio de contexto, restauración y finalización).
- **Visualización de registros de stack (`sp`) y direcciones de PCB** para asegurar que el `context_switch` aplicara correctamente los valores de `pcb[n].sp`.
- **Uso de `objdump` para revisar que `_start` estuviera en la dirección deseada** (`0x80000000`, `0x80010000`, `0x80020000` según el binario).

Durante las pruebas, se identificó también que:

- El linker no alineaba automáticamente `_start` al comienzo del binario, por lo que se agregó la directiva `KEEP(*(.vectors))` en `memmap.ld` para asegurar que la tabla de vectores (y `_start`) esté en el tope.
- Si el stack de un proceso no estaba correctamente reservado o apuntado desde `init_task_stack()`, se generaban errores de segmentación silenciosos.

---

## Implementación del PCB y Switching de Tareas

- Cada proceso tiene un `PCB` que almacena su puntero de stack (`sp`) y estado.
- `pcb[]` estático en `tasks.c` contiene los PCBs de todos los procesos.
- `current_task` indica el proceso en ejecución.
- `init_task_stack()` configura los stacks iniciales.

El switching ocurre así:
1. En `irq_handler` se guarda el contexto en el stack del proceso actual y se guarda su `sp` en su `PCB`.
2. Se llama a `context_switch()` que incrementa `current_task` (Round-Robin).
3. `context_switch_and_run()` usa el `sp` del nuevo proceso para restaurar su contexto y ejecutar.

Esto permite alternar entre procesos automáticamente tras cada interrupción de temporizador.

---
