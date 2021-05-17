package coordinacion;

// Imports necesarios para usar RabbitMQ
import com.rabbitmq.client.Channel;
import com.rabbitmq.client.Connection;
import com.rabbitmq.client.ConnectionFactory;
import com.rabbitmq.client.Consumer;
import com.rabbitmq.client.DefaultConsumer;
import com.rabbitmq.client.Envelope;
import com.rabbitmq.client.AMQP;

// Imports necesarios para usar RMI
import java.io.IOException;
import java.rmi.Naming;
import java.rmi.RemoteException;

// Imports necesarios para invocar via RMI métodos del médico
import medico.MedicoInterface;

// Cola bloqueante para comunicar  con los gestores de quirofanos y de equipos 
import java.util.concurrent.ArrayBlockingQueue;

// ===================================================================
// Las cinco clases siguientes son hilos que se ejecutarán de forma concurrente
//
// - Coordinacion es el hilo que espera mensajes de RabbitMQ y según el mensaje
//   recibido, lo despacha a uno de los otros cuatro hilos
// - GestorReservaQuirofano es el hilo que gestiona las peticiones de reserva de quirofano
// - GestorLiberacionQuirofano es el hilo que gestiona las liberaciones de quirofano
// - GestorReservaEquipo es el hilo que gestiona las peticiones de reserva de equipo de quirofano
// - GestorLiberacionEquipo es el hilo que gestiona las liberaciones de equipo de quirofano
//
// El hilo de Coordinacion se comunica con los otros dos mediante colas bloqueantes
// que comparten.
//
// Los Gestores comparten una instancia de EstadoRecursos que usan para alterar
// los estados de cada tipo de recurso.


// Clase Coordinacion. Recibe mensajes por RabbitMQ, los analiza y mete en colas
// bloqueantes las solicitudes para que sean atendidas en los hilos Gestores
class Coordinacion extends Thread {
    private final static String NOMBRE_COLA_RABBIT = "solicitar_recurso";
    private ArrayBlockingQueue<Integer> cola_solicitar_q; //solicitar quirofano
    private ArrayBlockingQueue<Integer> cola_liberar_q;   //liberar quirofano
    private ArrayBlockingQueue<Integer> cola_solicitar_e; //solicitar equipo
    private ArrayBlockingQueue<Integer> cola_liberar_e;  //liberar equipo

    // El constructor recibe las colas bloqueantes que le permiten comunicarse
    // con los otros hilos
    public Coordinacion(ArrayBlockingQueue<Integer> cola_solicitar_q,
                       ArrayBlockingQueue<Integer> cola_liberar_q,
                       ArrayBlockingQueue<Integer> cola_solicitar_e,
                       ArrayBlockingQueue<Integer> cola_liberar_e
                       ){
        this.cola_solicitar_q = cola_solicitar_q;
        this.cola_liberar_q = cola_liberar_q;
        this.cola_solicitar_e = cola_solicitar_e;
        this.cola_liberar_e = cola_liberar_e;
    }

    // La función run es la que se ejecuta al poner en marcha el hilo
    public void run() {
        // Conectar con rabbitMQ
        ConnectionFactory factory = new ConnectionFactory();
        factory.setHost("localhost");
        try {
            Connection connection = factory.newConnection();
            Channel channel = connection.createChannel();
            channel.queueDeclare(NOMBRE_COLA_RABBIT, false, false, false, null);

            // Espera por peticiones en la cola rabbitMQ
            Consumer consumer = new DefaultConsumer(channel) {
                @Override
                public void handleDelivery(String consumerTag, Envelope envelope, AMQP.BasicProperties properties, byte[] body) throws IOException {
                    // ************************************************************
                    // Recepción y manejo del mensaje que llega por RabbitMQ
                    // ************************************************************

                    // Convertir en cadena el mensaje recibido
                    String solicitud = new String(body, "UTF-8");
                    System.out.println("Controlador: Recibido mensaje = " + solicitud);
                    // Ejemplos de mensajes recibidos 
                    //  - "SQ 2" (solicitud de quirófano del médico 2)
                    //  - "LQ 3" (liberación del quirófano 3)
                    //  - "SE 4" (solicitud de equipo del médido 4)
                    //  - "LE 1" (liberación del equipo 1)

                    // Procesar el mensaje separando sus partes por los espacios. 
                    //
                    // Si la primera parte es "SQ", extraer (de la segunda parte) el número de médico
                    // y enviárselo al GestorReservaQuirofano, y si es "SE" de forma análoga enviar
                    // el número de médico a GestorReservaEquipo
                    //
                    // Si la primera parte es "LQ" extraer (de la segunda parte) el número de quirófano
                    // y enviárselo al GestorLiberacionQuirofaon, y si es "LE" de forma análoga enviar
                    // el número de equipo al GestorLiberacioneEquipo
                    //
                    // En otro caso ignorar el mensaje (se puede emitir un WARNING por la salida estándar)

                    // A RELLENAR
                    //dividimos el mensaje
                    String mensaje[] = solicitud.split(" ");
                    int tmp = -1; //variable para alojar el id de medico, quirofano o equipo
		            try {
			            tmp = Integer.parseInt(mensaje[0]);
		            }catch (NumberFormatException e) {
			            System.err.println(e.getMessage());
		            }
                    if (tmp!=-1){ 
                        switch (mensaje[0]){
                            case "SQ": //solicitud de quirofano
                                // enviar el número de médico a GestorReservaEquipo
                                if(!this.cola_solicitar_q.add(tmp)){ //si devuelve false no inserto en la cosa
                                    System.err.println("Error insertar cola SF: " + tmp);
                                }
                                break;
                            case "SE": //solicitud de equipo de enfermería
                                // enviar el número de médico a GestorReservaEquipo
                                if(!this.cola_solicitar_e.add(tmp)){//si devuelve false no inserto en la cosa
                                    System.err.println("Error insertar cola SE: " + tmp);
                                }
                                break;
                            case "LQ": //solicitud de equipo de enfermería
                                // enviar el número de médico a GestorReservaEquipo
                                if(!this.cola_liberar_q.add(tmp)){//si devuelve false no inserto en la cosa
                                    System.err.println("Error insertar cola LQ: " + tmp);
                                }
                                break;
                            case "LE": //solicitud de equipo de enfermería
                                // enviar el número del equipo a GestorReservaEquipo
                                if(!this.cola_liberar_e.add(tmp)){//si devuelve false no inserto en la cosa
                                    System.err.println("Error insertar cola LQ: " + tmp);
                                }
                                break;
                            default:
                                System.err.println("Error en peticion Hospital \n(tipo de mensaje erroneo) "+ solicitud);
                        }
                    }else{
                        System.err.println("Error peticion hospital \n(formato número erroneo) "+ solicitud);
                    }
                    

                }
            };
            System.out.println("Coordinacion del hospital. Esperando solicitudes de recursos");
            channel.basicConsume(NOMBRE_COLA_RABBIT, true, consumer);
            
        } catch (Exception e) {  // No manejamos excepciones, simplemente abortamos
            e.printStackTrace();
            System.exit(1);
        }
    }
}

// Clase GestorReservaQuirofano espera en una cola bloqueante a que el Coordinador le envíe una solicitud de reserva de quirofano.
// Solicita al objeto estado_quirofanos que le asigne uno y se lo notifica al
// médico adecuado haciendo una invocación RMI
class GestorReservaQuirofano extends Thread {
    private ArrayBlockingQueue<Integer> cola;  // Cola bloqueante en que espera peticiones del Coordinador
    private EstadoRecursos estado_quirofanos;  // Objeto que mantiene el estado de los quirofanos

    // El constructor recibe la cola y el estado de los quirofanos
    public GestorReservaQuirofano(ArrayBlockingQueue<Integer> cola, EstadoRecursos estado_quirofanos) {
        this.cola = cola;
        this.estado_quirofanos = estado_quirofanos;
    }

    // El método run es el que se ejecuta al arrancar el hilo
    public void run() {
        try {
            while (true) {  // Bucle infinito
                // Esperar solicitud del Coordinador en la cola bloqueante
                // A RELLENAR
                int idMedico = null;
                while(idMedico==null){
                    idMedico = cola.take();
                }
                // Obtener quirofano libre de EstadoRecursos. Esta llamada es bloqueante
                // y no se prosigue hasta que haya quirofano libre.
                // A RELLENAR
                int quirofano = estado_quirofanos.buscar_recurso("");
                // Notificarlo a través de RMI al médico adecuado
                // Primero se obtiene la instancia remota (del Medico_id) que corresponda
                String name = "Medico_" + idMedico;
                MedicoImpl medicu = (MedicoImpl) Naming.lookup(name);
                // y luego se invoca su método quirofanoConcedido()
                
                try {
                    // A RELLENAR
                    medicu.quirofanoConcedido(quirofano);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
            System.exit(2);
        }
    }
}

// Clase GestorReservaEquipo espera en una cola bloqueante a que el Coordinador le envíe una solicitud de reserva de equipo.
// Solicita al objeto estado_equipos que le asigne uno y se lo notifica al
// médico adecuado haciendo una invocación RMI
class GestorReservaEquipo extends Thread {
    private ArrayBlockingQueue<Integer> cola;  // Cola bloqueante en que espera peticiones del Coordinador
    private EstadoRecursos estado_equipos;  // Objeto que mantiene el estado de los equipos de quirofano

    // El constructor recibe la cola y el estado de los equipos
    public GestorReservaEquipo(ArrayBlockingQueue<Integer> cola, EstadoRecursos estado_equipos) {
        this.cola = cola;
        this.estado_equipos = estado_equipos;
    }

    // El método run es el que se ejecuta al arrancar el hilo
    public void run() {
        try {
            while (true) {  // Bucle infinito
                // Esperar solicitud del Coordinador en la cola bloqueante
                // A RELLENAR

                // Obtener equipo libre de EstadoRecursos. Esta llamada es bloqueante
                // y no se prosigue hasta que haya un equipo libre.
                // A RELLENAR

                // Notificarlo a través de RMI al médico adecuado
                // Primero se obtiene la instancia remota (del Medico_id) que corresponda
                // y luego se invoca su método quirofanoConcedido()                
                try {
                    // A RELLENAR
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
            System.exit(2);
        }
    }
}

// Clase GestorLiberacionQuirofano espera en una cola bloqueante a que el 
// Coordinador le envíe una petición de liberación de quirofano.
// Solicita al objeto estado_quirofanos que la libere. No necesita notificar nada
class GestorLiberacionQuirofano extends Thread {
    private ArrayBlockingQueue<Integer> cola;  // Cola bloqueante para recibir comandos del Coordinador
    private EstadoRecursos estado_quirofanos;  // Objeto que mantiene el estado de los quirofanos

    // El constructor recibe la cola y el estado de los quirofanos
    public GestorLiberacionQuirofano(ArrayBlockingQueue<Integer> cola, EstadoRecursos estado_quirofanos) {
        this.cola = cola;
        this.estado_quirofanos = estado_quirofanos;
    }

    // El método run es el que se ejecuta al arrancar el hilo
    public void run() {
        try {
            while (true) {  // Bucle infinito de espera
                // Esperar solicitud de quirofano a liberar
                Integer solicitud = cola.take();

                // Liberarlo a través del objeto estado_quirofanos
                estado_quirofanos.liberar_recurso("QUIROFANO",solicitud);
            }
        } catch (Exception e) {
            e.printStackTrace();
            System.exit(2);
        }
    }
}

// Clase GestorLiberacionEquipo espera en una cola bloqueante a que el 
// Coordinador le envíe una petición de liberación de equipo.
// Solicita al objeto estado_equipos que la libere. No necesita notificar nada
class GestorLiberacionEquipo extends Thread {
    private ArrayBlockingQueue<Integer> cola;  // Cola bloqueante para recibir comandos del Coordinador
    private EstadoRecursos estado_equipos;  // Objeto que mantiene el estado de los equipos

    // El constructor recibe la cola y el estado de los equipos
    public GestorLiberacionEquipo(ArrayBlockingQueue<Integer> cola, EstadoRecursos estado_equipos) {
        this.cola = cola;
        this.estado_equipos = estado_equipos;
    }

    // El método run es el que se ejecuta al arrancar el hilo
    public void run() {
        try {
            while (true) {  // Bucle infinito de espera
                // Esperar solicitud de equipo a liberar
                Integer solicitud = cola.take();

                // Liberarlo a través del objeto estado_equipos
                estado_equipos.liberar_recurso("EQUIPO",solicitud);
            }
        } catch (Exception e) {
            e.printStackTrace();
            System.exit(2);
        }
    }
}

// Clase principal que instancia los hilos anteriores y los arranca
public class Hospital {   
    public static void main(String[] argv) throws Exception {
        int max_quirofanos;    // Estas variables se leen de línea de comandos
        int max_equipos;
        int max_medicos;

        // Lectura de la línea de comandos
        // Se debe comprobar que se reciben tres parámetros (max_medicos, max_quirofanos 
        // y max_pistas) emitiendo un error si falta alguno no si no son de tipo numérico
        // A RELLENAR

        // Primero se crean las colas internas de sincronización entre hilos
        ArrayBlockingQueue<Integer> cola_solicitar_q =  new ArrayBlockingQueue<Integer>(max_medicos);
        ArrayBlockingQueue<Integer> cola_liberar_q = new ArrayBlockingQueue<Integer>(max_medicos);
        ArrayBlockingQueue<Integer> cola_solicitar_e =  new ArrayBlockingQueue<Integer>(max_medicos);
        ArrayBlockingQueue<Integer> cola_liberar_e = new ArrayBlockingQueue<Integer>(max_medicos);

        // Objeto que mantiene y modifica el estado de los quirofanos
        EstadoRecursos estado_quirofanos=new EstadoRecursos(max_quirofanos);
        // Objeto que mantiene y modifica el estado de los equipo de enfermeria
        EstadoRecursos estado_equipos = new EstadoRecursos(max_equipos);     
        
        // Manager de seguridad para RMI
        if (System.getSecurityManager() == null) {
            System.setSecurityManager(new SecurityManager());
        }

        // Ahora se crean los hilos, pasándole a cada uno los parámetros apropiados
        Coordinacion coordinacion = new Coordinacion(cola_solicitar_q, cola_liberar_q,cola_solicitar_e,cola_liberar_e);
        GestorReservaQuirofano gestorreservas_q = new GestorReservaQuirofano(cola_solicitar_q, estado_quirofanos);
        GestorLiberacionQuirofano gestorliberaciones_q = new GestorLiberacionQuirofano(cola_liberar_q, estado_quirofanos);
        GestorReservaEquipo gestorreservas_e = new GestorReservaEquipo(cola_solicitar_e, estado_equipos);
        GestorLiberacionEquipo gestorliberaciones_e = new GestorLiberacionEquipo(cola_liberar_e, estado_equipos);
        
        // Arrancamos los hilos
        coordinacion.start();  
        gestorreservas_q.start();
        gestorliberaciones_q.start();
        gestorreservas_e.start();
        gestorliberaciones_e.start();

        // Esperamos a que finalice el coordinador (nunca finalizará, hay que parar con Ctrl+C)
        coordinacion.join();
    }
}
