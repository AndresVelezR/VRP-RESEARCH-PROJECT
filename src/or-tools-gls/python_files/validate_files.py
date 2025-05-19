import os
import re
import sys

NUM_NODES = 200
# Precisión para reescribir los números flotantes (ej. 5 decimales)
FLOAT_PRECISION = 5

def validate_and_normalize_coord_file(input_filepath="Coord.txt", output_filepath_temp="Coord_temp.txt"):
    """
    Valida y normaliza el archivo de coordenadas.
    Lee de input_filepath, escribe una versión normalizada en output_filepath_temp si es válido.
    Retorna True si el archivo es válido y se procesó, False si hay errores graves.
    """
    print(f"\nValidando y normalizando '{input_filepath}'...")
    lines_data = []
    error_messages = []
    actual_line_count = 0 # Contador de líneas físicas leídas

    if not os.path.exists(input_filepath):
        print(f"Error Crítico: El archivo '{input_filepath}' no existe.")
        return False

    with open(input_filepath, 'r') as f:
        for i, line_content in enumerate(f):
            actual_line_count = i + 1
            original_line = line_content.strip()

            if not original_line: # Si la línea está completamente vacía después de strip
                # Permitir líneas vacías al final, pero no si afectan el conteo de NUM_NODES
                if len(lines_data) < NUM_NODES :
                     error_messages.append(f"Error: Línea {actual_line_count}: La línea está vacía pero aún no se han leído {NUM_NODES} coordenadas.")
                continue # No procesar líneas completamente vacías

            # Reemplazar tabulaciones y múltiples espacios con un solo espacio
            normalized_line = re.sub(r'\s+', ' ', original_line).strip()
            parts = normalized_line.split(' ')
            # Filtrar partes vacías que podrían surgir si split genera cadenas vacías (aunque re.sub debería ayudar)
            parts = [p for p in parts if p]


            if len(parts) != 2:
                error_messages.append(f"Error: Línea {actual_line_count} ('{original_line}'): Se esperan 2 coordenadas, se encontraron {len(parts)} ({parts}).")
                continue

            try:
                x = float(parts[0])
                y = float(parts[1])
                lines_data.append((x, y))
            except ValueError:
                error_messages.append(f"Error: Línea {actual_line_count} ('{original_line}'): Contiene valores no numéricos ('{parts[0]}', '{parts[1]}').")
                continue
    
    if not lines_data and actual_line_count == 0: # Archivo completamente vacío
         error_messages.append(f"Error Crítico: El archivo '{input_filepath}' está vacío.")

    # Verificar el número total de líneas de datos válidas leídas.
    if len(lines_data) != NUM_NODES:
        error_messages.append(f"Error Crítico: Se esperaban {NUM_NODES} líneas de coordenadas válidas, pero se procesaron {len(lines_data)}.")

    if error_messages:
        print(f"Problemas encontrados en '{input_filepath}':")
        for msg in error_messages:
            print(f"- {msg}")
        # Si hay algún mensaje que empiece con "Error Crítico" o "Error:", no se reescribe.
        if any(msg.startswith("Error Crítico:") or msg.startswith("Error:") for msg in error_messages):
            return False

    # Si no hay errores críticos, escribir el archivo normalizado temporalmente
    try:
        with open(output_filepath_temp, 'w') as f_out:
            for x, y in lines_data:
                f_out.write(f"{x:.{FLOAT_PRECISION}f} {y:.{FLOAT_PRECISION}f}\n")
        print(f"Archivo '{input_filepath}' validado y normalizado. Temporalmente guardado como '{output_filepath_temp}'.")
        return True
    except IOError as e:
        print(f"Error Crítico: No se pudo escribir el archivo de salida temporal '{output_filepath_temp}'. Error: {e}")
        return False


def validate_and_normalize_dist_file(input_filepath="Dist.txt", output_filepath_temp="Dist_temp.txt"):
    """
    Valida y normaliza el archivo de la matriz de distancias.
    Lee de input_filepath, escribe una versión normalizada en output_filepath_temp si es válido.
    Retorna True si el archivo es válido y se procesó, False si hay errores graves.
    """
    print(f"\nValidando y normalizando '{input_filepath}'...")
    matrix_data = [] # Almacenará filas que son estructuralmente válidas (200 números)
    error_messages = []
    warning_messages = [] # Para problemas no críticos como asimetría o diagonal no cero
    actual_line_count = 0

    if not os.path.exists(input_filepath):
        print(f"Error Crítico: El archivo '{input_filepath}' no existe.")
        return False

    with open(input_filepath, 'r') as f:
        for i, line_content in enumerate(f):
            actual_line_count = i + 1
            original_line = line_content.strip()

            if not original_line:
                if len(matrix_data) < NUM_NODES:
                     error_messages.append(f"Error: Línea {actual_line_count}: La línea está vacía pero aún no se han leído {NUM_NODES} filas de la matriz.")
                continue

            normalized_line = re.sub(r'\s+', ' ', original_line).strip()
            parts = normalized_line.split(' ')
            parts = [p for p in parts if p] # Eliminar cadenas vacías

            if len(parts) != NUM_NODES:
                error_messages.append(f"Error: Línea {actual_line_count} (conteniendo '{original_line[:50]}...'): Se esperan {NUM_NODES} valores de distancia, se encontraron {len(parts)}.")
                continue # Saltar esta línea para el procesamiento de datos, pero contar el error
            
            current_row_floats = []
            valid_row_for_matrix_data = True
            for j, val_str in enumerate(parts):
                try:
                    dist = float(val_str)
                    current_row_floats.append(dist)
                except ValueError:
                    error_messages.append(f"Error: Línea {actual_line_count}, Valor en columna ~{j+1} ('{val_str}'): No es un valor numérico.")
                    valid_row_for_matrix_data = False
                    break 
            
            if valid_row_for_matrix_data:
                matrix_data.append(current_row_floats)
            # Si valid_row_for_matrix_data es False, la fila tuvo un error y no se añade a matrix_data, pero el error ya fue registrado.

    if not matrix_data and actual_line_count == 0:
         error_messages.append(f"Error Crítico: El archivo '{input_filepath}' está vacío.")
        
    if len(matrix_data) != NUM_NODES:
        error_messages.append(f"Error Crítico: Se esperaban {NUM_NODES} filas válidas en la matriz de distancias, pero se procesaron {len(matrix_data)} (de {actual_line_count} líneas físicas).")

    # Si hubo errores críticos estructurales, no continuar con advertencias o reescritura.
    if any(msg.startswith("Error Crítico:") or msg.startswith("Error:") for msg in error_messages):
        print(f"Errores encontrados en '{input_filepath}':")
        for msg in error_messages:
            print(f"- {msg}")
        return False

    # Si la estructura es correcta (200x200 de flotantes), verificar advertencias
    for i, row in enumerate(matrix_data):
        # Verificar diagonal (dist[i][i] == 0)
        if abs(row[i] - 0.0) > 1e-9: # Usar una pequeña tolerancia para flotantes
            warning_messages.append(f"Advertencia: Fila {i+1}, Columna {i+1}: Distancia diagonal dist[{i}][{i}] es {row[i]}, debería ser 0.0.")
        # Verificar simetría (dist[i][j] == dist[j][i])
        for j in range(i + 1, NUM_NODES):
            if abs(row[j] - matrix_data[j][i]) > 1e-3: # Tolerancia mayor para distancias
                 warning_messages.append(f"Advertencia: Asimetría detectada: dist[{i}][{j}] ({row[j]}) != dist[{j}][{i}] ({matrix_data[j][i]}).")

    if error_messages or warning_messages:
        print(f"Problemas y/o advertencias encontradas en '{input_filepath}':")
        for msg in error_messages: # Deberían estar vacíos si llegamos aquí
            print(f"- {msg}")
        for msg in warning_messages:
            print(f"- {msg}")
        # Continuar con la reescritura incluso si hay advertencias, ya que la estructura es válida.

    # Escribir el archivo normalizado temporalmente
    try:
        with open(output_filepath_temp, 'w') as f_out:
            for row_floats in matrix_data:
                formatted_row = " ".join([f"{dist:.{FLOAT_PRECISION}f}" for dist in row_floats])
                f_out.write(formatted_row + "\n")
        print(f"Archivo '{input_filepath}' validado y normalizado (con posibles advertencias). Temporalmente guardado como '{output_filepath_temp}'.")
        return True
    except IOError as e:
        print(f"Error Crítico: No se pudo escribir el archivo de salida temporal '{output_filepath_temp}'. Error: {e}")
        return False

def main():
    print("--- Script de Validación y Normalización de Archivos VRP ---")

    coord_input_file = "Coord.txt"
    coord_temp_file = "Coord_temp_validated.txt"
    dist_input_file = "Dist.txt"
    dist_temp_file = "Dist_temp_validated.txt"

    all_ok = True

    # Validar y normalizar Coord.txt
    if validate_and_normalize_coord_file(coord_input_file, coord_temp_file):
        try:
            os.replace(coord_temp_file, coord_input_file)
            print(f"Archivo '{coord_input_file}' actualizado exitosamente.")
        except Exception as e:
            print(f"Error Crítico al reemplazar '{coord_input_file}' con '{coord_temp_file}': {e}")
            print(f"El archivo normalizado está disponible como '{coord_temp_file}'.")
            all_ok = False
    else:
        print(f"Procesamiento de '{coord_input_file}' falló debido a errores críticos.")
        if os.path.exists(coord_temp_file): os.remove(coord_temp_file) # Limpiar temporal
        all_ok = False

    # Validar y normalizar Dist.txt solo si el anterior fue exitoso o si queremos procesar independientemente
    if all_ok: # Opcional: podrías decidir validar Dist.txt incluso si Coord.txt falló
        if validate_and_normalize_dist_file(dist_input_file, dist_temp_file):
            try:
                os.replace(dist_temp_file, dist_input_file)
                print(f"Archivo '{dist_input_file}' actualizado exitosamente.")
            except Exception as e:
                print(f"Error Crítico al reemplazar '{dist_input_file}' con '{dist_temp_file}': {e}")
                print(f"El archivo normalizado está disponible como '{dist_temp_file}'.")
                all_ok = False
        else:
            print(f"Procesamiento de '{dist_input_file}' falló debido a errores críticos o se encontraron advertencias importantes.")
            if os.path.exists(dist_temp_file): os.remove(dist_temp_file) # Limpiar temporal
            all_ok = False
    elif os.path.exists(dist_temp_file): # Si Coord.txt falló, asegurar que no quede un Dist_temp
        os.remove(dist_temp_file)


    if all_ok:
        print("\n--- Validación y Normalización Completadas ---")
        print(f"Los archivos '{coord_input_file}' y '{dist_input_file}' han sido validados y actualizados.")
        print("Ahora están listos para ser usados por el programa C++.")
        sys.exit(0) # Salida exitosa
    else:
        print("\n--- Validación y Normalización Fallida ---")
        print("Se encontraron errores críticos. Revisa los mensajes anteriores.")
        print("Los archivos originales pueden no haber sido modificados o estar en un estado inconsistente si solo uno falló.")
        sys.exit(1) # Salida con error

if __name__ == "__main__":
    main()