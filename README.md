# Sistema Operativo Bare-Metal - BeagleBone Black

## 🧠 Objetivo del proyecto

Este proyecto tiene como propósito construir un sistema operativo mínimo que corra directamente sobre el hardware del BeagleBone Black (basado en el procesador AM335x), sin depender de un sistema operativo previo o un entorno como Linux. El sistema se ejecuta en modo bare-metal, lo que implica control total del hardware, incluyendo memoria, interrupciones, y periféricos como UART y temporizadores.

En esta fase inicial se logró:

- Configurar el entorno de ejecución en modo Supervisor e IRQ.
- Establecer correctamente las direcciones de memoria para código, stack del sistema y stack de interrupciones.
- Configurar el temporizador DMTIMER2 para generar interrupciones periódicas.
- Capturar y manejar correctamente interrupciones en ARM.
- Implementar comunicación UART para propósitos de depuración y monitoreo.

---

## 🧱 Organización de memoria

La memoria fue segmentada para cumplir con los requerimientos de diferentes modos de ejecución del procesador ARM (Supervisor, IRQ) y evitar solapamientos. Se utilizó un linker script (`memmap.ld`) que define las siguientes áreas:

```
Dirección        Región             Propósito
───────────────  ─────────────────  ──────────────────────────────────────
0x80000000       OS_CODE            Código del sistema operativo (64KB)
0x80007000       IRQ_STACK          Stack exclusivo del modo IRQ (4KB)
0x80008000       OS_STACK           Stack del modo Supervisor (4KB)
```

Cada modo del CPU ARM posee su propio registro de stack pointer (`sp`). Si no se configura adecuadamente, cualquier operación de `push` durante una interrupción (modo IRQ) puede causar fallos.

---

## 🔧 Inicialización del sistema 

Al arrancar el sistema, se realiza lo siguiente en el código ensamblador (`root.s`):

1. **Configuración del stack para modo Supervisor** (`_os_stack_top`).
2. **Cambio temporal al modo IRQ** y configuración de su stack (`_irq_stack_top`).
3. **Regreso a modo Supervisor** y habilitación global de interrupciones (`cpsie i`).
4. **Definición del vector de interrupciones** mediante `mcr p15, 0, r0, c12, c0, 0`.
5. **Llamada a `main()`** para iniciar el flujo principal en C.

---

## ⛔ Problemas encontrados y soluciones

### 1. ❌ El sistema se colgaba en `irq_handler`

**Síntoma:**  
Solo se imprimía un carácter `'A'` en UART, indicando que la ejecución se detenía al hacer `push` dentro del manejador.

**Causa:**  
El modo IRQ no tenía configurado su propio stack pointer (`sp_irq`). Por defecto, ARM lo deja apuntando a dirección 0 o basura.

**Solución:**  
- Se definió una región `IRQ_STACK` en el linker script.
- Se agregó en `_start` el código que cambia a modo IRQ y configura `sp`.

---

### 2. ❌ Las interrupciones no se disparaban

**Síntoma:**  
A pesar de que el temporizador se inicializaba, no se veía la salida "[IRQ] Tick!".

**Causa:**  
La interrupción ocurría, pero el CPU no podía ejecutar correctamente el `irq_handler` debido al error anterior del stack IRQ.

**Solución:**  
Corregido al establecer `sp_irq` en `_start`, con lo que la instrucción `push {r0-r12, lr}` ya fue válida y se ejecutó correctamente.

---

### 3. ❌ Uso inseguro de `uart_putc` y `uart_puts` dentro de interrupciones

**Síntoma:**  
Incluso después de corregir el stack del modo IRQ, al llamar a `uart_putc` desde `timer_irq_handler`, el sistema se colgaba o entraba en bucles inesperados.

**Causa:**  
Las funciones `uart_putc` y `uart_puts` hacían polling (espera activa) verificando registros como `UART_LSR`, pero el compilador introdujo instrucciones adicionales como:

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

## 📌 Funciones clave del sistema

- `timer_init()`  
  Inicializa el DMTIMER2, configura el valor de recarga, habilita auto-reload y permite la interrupción por overflow.

- `enable_irq()`  
  Borra el bit I del `cpsr` para permitir interrupciones IRQ.

- `irq_handler` (ASM)  
  Handler global de IRQ que imprime caracteres en UART para debug y llama al `timer_irq_handler` en C.

- `timer_irq_handler()`  
  Limpia los registros del temporizador (`TISR`, `INTC_CONTROL`) y escribe en UART.

- `uart_putc` / `uart_puts`  
  Funciones básicas para salida serial mediante UART0.


## ⏭️ Siguientes pasos

- Implementar tabla de PCB y switching entre tareas.
- Definir secciones de código y stack por proceso.
- Agregar scheduling Round-Robin con soporte completo de context switching.
- Implementar modo User y FIQ para pruebas avanzadas de privilegios.
