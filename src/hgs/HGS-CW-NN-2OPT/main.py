import subprocess
import os
import sys

class CVRPManager:
    """Clase para gestionar la ejecución de comandos relacionados con el CVRP."""
    
    def __init__(self):
        """Inicializa las rutas de los directorios y crea el directorio build si no existe."""
        self.build_dir = os.path.abspath('./build')
        self.main_dir = os.path.abspath('.')
        os.makedirs(self.build_dir, exist_ok=True)  # Crea la carpeta build si no existe

    def _run_command(self, command, cwd=None, capture_output=True, show_output=True):
        """
        Ejecuta un comando de consola y maneja errores.

        Args:
            command (list): Lista con el comando y sus argumentos.
            cwd (str): Directorio de trabajo (opcional).
            capture_output (bool): Si True, captura stdout/stderr.
            show_output (bool): Si True, muestra stdout/stderr directamente en consola.

        Returns:
            bool: True si el comando se ejecuta correctamente, False si falla.
        """
        try:
            result = subprocess.run(
                command,
                cwd=cwd if cwd else self.main_dir,
                capture_output=capture_output,
                text=True,
                check=True
            )
            if capture_output and show_output:
                if result.stdout:
                    print("Salida del comando:")
                    print(result.stdout)
                if result.stderr:
                    print("Errores del comando:")
                    print(result.stderr)
            return True
        except subprocess.CalledProcessError as e:
            print(f"Error al ejecutar el comando: {e.stderr if capture_output else e}")
            return False
        except FileNotFoundError as e:
            print(f"No se encontró el comando o la carpeta: {e}")
            return False
        except Exception as e:
            print(f"Otro error: {e}")
            return False

    def clean_build(self):
        """Limpia el contenido de la carpeta build."""
        print(f"Limpiando contenido de: {self.build_dir}")
        return self._run_command(['rm', '-rf', '*'], cwd=self.build_dir)

    def run_cmake(self):
        """Ejecuta cmake en la carpeta build."""
        print(f"Ejecutando cmake en: {self.build_dir}")
        return self._run_command(['cmake', '..'], cwd=self.build_dir)

    def run_make(self):
        """Ejecuta make en la carpeta build."""
        print(f"Ejecutando make en: {self.build_dir}")
        return self._run_command(['make'], cwd=self.build_dir)

    def generate_instance(self):
        """Genera una instancia ejecutando generate_instance.py."""
        print("Generando instancia con generate_instance.py")
        return self._run_command(['python3', 'generate_instance.py'], cwd=self.main_dir)

    def run_hgs_cvrp(self):
        """
        Ejecuta hgs_cvrp en la carpeta build con los argumentos especificados.
        Muestra la salida directamente en consola.
        """
        print(f"Ejecutando hgs_cvrp en: {self.build_dir}")
        return self._run_command(
            ['./hgs_cvrp', '../instance.vrp', '../mySolution.sol', '-veh', '20'],
            cwd=self.build_dir,
            capture_output=False,
            show_output=True
        )

    def make_charts(self):
        """Genera gráficos ejecutando generate_chart.py."""
        print("Generando gráficos con generate_chart.py")
        return self._run_command(['python3', 'generate_chart.py'], cwd=self.main_dir)

def display_menu():
    """Muestra el menú de opciones."""
    print("\n=== Menú de Ejecución CVRP ===")
    print("1. Limpiar build, ejecutar cmake y make")
    print("2. Generar instancia")
    print("3. Ejecutar hgs_cvrp")
    print("4. Generar gráficos")
    print("5. Salir")
    print("==============================")

def main():
    """Función principal que ejecuta el menú interactivo."""
    manager = CVRPManager()
    
    while True:
        display_menu()
        opcion = input("Seleccione una opción (1-5): ").strip()
        
        if opcion == '1':
            if (manager.clean_build() and 
                manager.run_cmake() and 
                manager.run_make()):
                print("Limpieza y compilación completadas exitosamente.")
            else:
                print("Error durante la limpieza o compilación.")
        
        elif opcion == '2':
            if manager.generate_instance():
                print("Instancia generada exitosamente.")
        
        elif opcion == '3':
            if manager.run_hgs_cvrp():
                print("hgs_cvrp ejecutado exitosamente.")
        
        elif opcion == '4':
            if manager.make_charts():
                print("Gráficos generados exitosamente.")
        
        elif opcion == '5':
            print("Saliendo del programa.")
            sys.exit(0)
        
        else:
            print("Opción no válida. Por favor, seleccione una opción entre 1 y 5.")

if __name__ == "__main__":
    main()
