Tarea 2 — Lottery Scheduling en XV6

##  1. Funcionamiento y lógica de la implementación

En esta tarea se reemplazó el planificador Round-Robin de XV6 por un esquema de **Lottery Scheduling**, en el cual cada proceso posee una cantidad de *tickets* que determinan la probabilidad de ser elegido para ejecutar.  
Cuantos más tickets tiene un proceso, mayor es su probabilidad de “ganar la lotería” y, por tanto, de acceder a la CPU.

### Lógica del scheduler
El nuevo `scheduler()` en `kernel/proc.c` sigue la siguiente secuencia:
1. **Suma total de tickets:** se recorren todos los procesos en estado `RUNNABLE`, acumulando su número de tickets (forzando un mínimo de 1).
2. **Selección aleatoria:** se genera un número `r = rand() % total`.
3. **Elección del ganador:** se recorre nuevamente la tabla de procesos acumulando tickets hasta que el total acumulado es ≥ `r`.
4. **Ejecución:** el proceso ganador cambia su estado a `RUNNING` y se ejecuta. Se incrementa un contador `wins` cada vez que un proceso es seleccionado.

Este mecanismo asegura que, en promedio, la proporción de tiempo de CPU asignada a cada proceso sea proporcional a su número de tickets.

---

##  2. Modificaciones realizadas

| Archivo | Cambios principales |
|----------|--------------------|
| **`kernel/proc.h`** | Se añadieron los campos `int tickets;` y `int wins;` a `struct proc`. |
| **`kernel/proc.c`** | Se inicializan `tickets=10` y `wins=0` en `allocproc()`. Se modificó `scheduler()` para implementar la lotería. |
| **`kernel/sysproc.c`** | Se agregaron las funciones `sys_settickets()`, `sys_gettickets()` y `sys_getwins()` para exponer la información al espacio de usuario. |
| **`kernel/syscall.c` / `kernel/syscall.h`** | Se definieron los nuevos números de syscall (`SYS_settickets`, `SYS_gettickets`, `SYS_getwins`) y su mapeo en la tabla `syscalls[]`. |
| **`user/user.h` y `user/usys.pl`** | Se declararon las funciones de usuario `settickets(int)`, `gettickets(void)`, `getwins(int pid)`. |
| **`user/winstats.c`** | Programa auxiliar para consultar el número de *wins* del proceso actual. |
| **`user/demo.c`** | Programa de prueba principal: crea varios procesos, les asigna tickets distintos y muestra sus *wins*. |
| **`Makefile`** | Se añadieron los ejecutables `_ticktest`, `_winstats` y `_demo` a la variable `UPROGS`. |

---

## ️ 3. Dificultades encontradas y soluciones implementadas

| Dificultad | Solución |
|-------------|-----------|
| Error en la syscall `getwins`: “void value not ignored as it ought to be” | Se corrigió el tipo de retorno de `argint()` en `defs.h`, y se validó correctamente el argumento `pid`. |
| Conflicto de tipos entre `argint` y `argaddr` | Se revisó la declaración original de XV6 y se restauró la firma correcta. |
| Error de compilación por argumentos faltantes | Se corrigió la llamada a `getwins()` agregando `getpid()` como parámetro. |
| Validación de tickets nulos o negativos | Se agregó condición `if (n < 1) n = 1;` en `sys_settickets()`. |
| Pruebas poco determinísticas | Se implementó el programa `demo` para mostrar tendencia estadística esperada. |

---

##  4. Posibles problemas del Lottery Scheduling

1. **No determinismo:** El planificador introduce aleatoriedad, por lo que los resultados no son reproducibles.
2. **Starvation improbable pero posible:** Si los tickets son muy desbalanceados, procesos con pocos tickets podrían esperar mucho tiempo.
3. **Sobrecarga computacional:** Calcular el total y generar números aleatorios en cada iteración puede ser más costoso que Round-Robin.
4. **Falta de equidad en procesos cortos:** Procesos breves pueden no recibir CPU suficiente antes de finalizar.
5. **Escalabilidad limitada:** Con muchos procesos, recorrer toda la tabla puede ser costoso.

