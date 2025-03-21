import sys
import signal

TIMEOUT = 1  # seconds

# Configurar la señal de alarma para manejar el timeout
signal.signal(signal.SIGALRM, lambda signum, frame: sys.exit("Timeout alcanzado"))
signal.alarm(TIMEOUT)

print("Inicio")
print("Script Python para sumar dos números\n")

# Inicializar variables para los números
num1 = None
num2 = None

# Intentar leer los números desde STDIN

for arg in sys.argv[1:]:  # Ignorar el primer argumento que es el nombre del script
    try:
        # Verificar si el argumento tiene el formato 'num1=valor'
        key, value = arg.split("=")
        if key == "num1":
            num1 = float(value)
        elif key == "num2":
            num2 = float(value)
    except ValueError:
        print(f"Error: El argumento {arg} no tiene el formato esperado 'key=value'.")
        sys.exit(1)

if num1 != None and num2 != None:
    result = num1 + num2
    print(f"Resultado de la suma: {result}")
    exit(0)
stdin_data = sys.stdin.read().strip()

# Intentar leer números desde STDIN
if stdin_data:
    try:
        # Dividir por espacios y procesar los valores key=value
        for arg in stdin_data.split():
            if "=" in arg:
                key, value = arg.split("=")
                if key == "num1":
                    num1 = float(value)
                elif key == "num2":
                    num2 = float(value)
    except ValueError:
        print("Error al leer los números desde STDIN")

# Si no se recibieron los números por STDIN, intentar leerlos por ARGV

# Verificar que ambos números se han leído
if num1 is None or num2 is None:
    print("Error: No se recibieron los números adecuados.")
    sys.exit(1)

# Sumar los números
result = num1 + num2
print(f"Resultado de la suma: {result}")

print("\n\nFin del script")


