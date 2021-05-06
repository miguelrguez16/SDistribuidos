package medico;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.concurrent.ArrayBlockingQueue;

/*
La clase MedicoImpl expone vía RMI los metodos:

quirofanoConcedido(): que será invocado desde Coordinacion del hospital cuando se conceda
 un quirofano para operar al medico y recibira como parametro el quirofano concedido.

equipoConcedido(): que será invocado desde Coordinacion del hospital cuando se asigne 
   un equipo de quirofano al medico y recibira como parametro el equipo concedido.

Además proporciona los métodos getQuirofano() y getEquipo() para que puedan ser llamados
desde el simulador del hospital, para así obtener el quirofano y el equipo de quirofano
concedido respectivamente.

Estos métodos se ejecutan en diferentes hilos (ej.: quirofanoConcedido() se
ejecuta en el hilo que atiende peticiones RMI, mientras que getQuirofano() se
ejecuta en el hilo del simulador del hospital. Se comunican entre sí mediante
una cola bloqueante. getQuirofano() intenta leer de esa cola, y si está vacía
se bloquea. quirofanocedido() pone en esa cola el quirofano asignado, 
desbloqueando así al hilo que estaba en getQuirofano())
*/

public class MedicoImpl extends UnicastRemoteObject implements MedicoInterface {
    private ArrayBlockingQueue<Integer> cola;  // Cola bloqueante para comunicar los hilos


    public MedicoImpl() throws RemoteException {
        super();
        cola = new ArrayBlockingQueue<Integer>(1);  // Basta que tenga tamaño 1
    }

    @Override
    public void quirofanoConcedido(int quirofano) throws RemoteException {
        // Este método es invocado vía RMI desde la Coordinacion
        // Recibe el quirofano concedido y se limita a guardarlo en la cola interna
        
        // A RELLENAR
    }

    @Override
    public void equipoConcedido(int equipo) throws RemoteException {
        // Este método es invocado vía RMI desde la Coordinacion
        // Recibe el equipo concedido y se limita a guardarlo en la cola interna

        // A RELLENAR
    }

    public int getQuirofano() throws InterruptedException {
        // Este método es invocado desde el simulador
        // Espera a que aparezca el quirofano concedido en la cola interna
        // y retorna su valor

        // A RELLENAR
    }

    public int getEquipo() throws InterruptedException {
        // Este método es invocado desde el simulador
        // Espera a que aparezca el equipo asignado en la cola interna
        // y retorna su valor

        // A RELLENAR
    }
}