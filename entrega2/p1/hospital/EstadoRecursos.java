package coordinacion;

// Primitivas de sincronización
import java.util.concurrent.Semaphore;

// Clase que mantiene el estado de los recursos, y permite modificarlo a
// través de sus métodos. Ya que estos métodos pueden ser llamados
// desde diferentes hilos, se sincronizan entre sí mediante
// bloques syncrhonized (exclusión mutua) y mediante un semáforo (para
// señalar cuándo hay recurso libre)

class EstadoRecursos {
    private int estado[];   // Array de enteros: 0  libre, 1 ocupado
    private int size;       // Numero de recursos
    private Semaphore num_huecos;  // Semáforo para señalar existencia de recursos libres

    public EstadoRecursos(int size) {
        // El constructor inicializa el array y el semáforo
        num_huecos = new Semaphore(size);
        this.size = size;
        estado = new int[size];
        for (int i=0; i<size; i++) {
            estado[i] = 0;
        }
    }

    // Función de utilidad para mostrar el estado de los recursos
    synchronized void print_recursos(String msg) {
        System.out.print(msg + ": [");
        for (int p : estado) {
            System.out.print(p);
        }
        System.out.println("]");
    }

    // Función que espera a que haya un recurso libre y retorna su numero
    int buscar_recurso(String msg) throws InterruptedException {
        int encontrado = -1;
        print_recursos(msg);

        // Espera en el semáforo a que haya un recurso libre
        num_huecos.acquire();
        synchronized(this) {                                // Exclusión mutua
            // Accede al array buscando un sitio libre
            for (int i=0; i < size; i++) {
                if (estado[i] == 0) {
                    estado[i] = 1;
                    encontrado = i;
                    break;
                }
            }
        }                                                  // Fin exclusión

        print_recursos(String.format("EstadoRecursos. Encontrado %s %d",msg, encontrado));
        return encontrado;
    }

    // Pone un 0 en el recurso y señala en el semáforo
    synchronized void liberar_recurso (String msg,int id_recurso){  // La función entera opera en exclusión mutua
        if (estado[id_recurso] == 0) {
            System.out.println("WARNING: Intento de liberar "+msg+" "+id_recurso+" que estaba libre");
            return;
        }
        estado[id_recurso] = 0;
        num_huecos.release();
        print_recursos(String.format("EstadoRecursos. Liberado %s %d", msg, id_recurso));
    }
}
