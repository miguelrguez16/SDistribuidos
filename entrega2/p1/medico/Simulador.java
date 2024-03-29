package medico;

// Imports necesarios para RabbitMQ
import com.rabbitmq.client.Channel;
import com.rabbitmq.client.Connection;
import com.rabbitmq.client.ConnectionFactory;

// Imports necesarios para RMI
import java.io.IOException;
import java.rmi.Naming;

public class Simulador {
  private final static String NOMBRE_COLA = "solicitar_recurso";
 
  
  public static void main(String[] argv) throws Exception {
    int id = 0;          // Identificador del medico
    int num_pacientes = 0;  // Número de pacientes a operar

    // =================================================
    // Obtener argumentos de línea de comandos, verificando que
    // hay el número adecuado de ellos y que son números enteros
    // Los argumentos han de ser el id_medico y el número de pacientes a operar
    // A RELLENAR
    //comprobar que son solo dos los argumentos
    if(argv.length==2){
            try {
                id = Integer.parseInt(argv[0]);
                num_pacientes = Integer.parseInt(argv[1]);
            }catch(NumberFormatException e){
                System.out.println("Error: formato de los argumentos incorrecto\n Deben ser numéricos");
                System.exit(1);
            } 
        }else{
            System.out.println("Error: Número de argumentos\nUso: Medico <id_medico> <num_pacientes>");
            System.exit(1);
        }
    // =================================================
    // Instanciar SecurityManager necesario para RMI
    if (System.getSecurityManager() == null) {
      System.setSecurityManager(new SecurityManager());
    }
    //identificador del medico
    String nome = "Medico_" + id;
    // =================================================
    // Parte principal, toda dentro de un try para capturar cualquier excepción
    try {
      // Arrancar el servidor RMI MedicoImpl y registrarlo en rmiregistry
      // dandole como nombre "Medico_id", según su id
      // A RELLENAR
      MedicoImpl medicu=new MedicoImpl();
      Naming.rebind(nome,medicu);
      //medicu.Naming.rebind(nome);
      // Conectar con Rabbit para poder enviar peticiones a la cola
      // A RELLENAR
      ConnectionFactory factory = new ConnectionFactory();
      factory.setHost("localhost");
      Connection connection = factory.newConnection();
      Channel channel = connection.createChannel();
      //conectamos a la cola
      //asi solo se hace una vez
      channel.queueDeclare(NOMBRE_COLA, false, false, false, null);
      // Realizar la simulación
      simular_operaciones(num_pacientes, channel, id, medicu);

      // Terminar
      System.out.println("Medico " + id + " finalizó sus intervenciones.");
      channel.close();
      connection.close();
      System.exit(0);
    } 
    catch (Exception e) {
      // Cualquier excepción simplemente se imprime
      System.out.println("Error en Medico_" + id + ": " + e.getMessage());
      e.printStackTrace();
    }
  }

  static double randRange(double min, double max) {
    // Función de utilidad para generar un double entre min y max
    return min + Math.random() * (max - min);
  }

  // =========================================================================
  // La función que simula las diferentes operaciones del medico
  // Requiere como parámetros:
  //
  //  - El número de pacientes a operar
  //  - El canal de comunicación con RabbitMQ para solicitar recursos
  //  - El id del médico, pues lo usa como parte del mensaje enviado a RabbitMQ
  //  - El objeto MedicoImpl, pues lo usa para obtener (esperar) el quirofano
  //    y equipo asignado
  // 
  // Repite num_operaciones veces el bucle de simulación  y retorna
  static void simular_operaciones(int num_pacientes, Channel channel, int id, MedicoImpl medico) 
              throws IOException, InterruptedException 
  {
    String msg;       // Para crear los mensajes a enviar por Rabbit
    int equipo, quirofano;      // Para obtener el recurso (quirofano o equipo) concedido
    double t;         // Tiempos de uso del quirofano.

    for (int paciente=0; paciente<num_pacientes; paciente++) {
  
      // Solicitamos quirofano para intervencion
      System.out.println(String.format("Medico %d solicita quirofano para operacion (paciente %d)", id, paciente));

      // Crear mensaje apropiado y ponerlo en la cola RabbitMQ
      // A RELLENAR
      String mensaxe = "SQ " + id;     
      channel.basicPublish("", NOMBRE_COLA, null, mensaxe.getBytes("UTF-8"));
      //depuracion
      //System.out.println(mensaxe);

      // Esperar por el quirofano concedido
      quirofano = medico.getQuirofano();


      // Solicitamos equipo de quirofano para intervencion
      System.out.println(String.format("Medico %d solicita equipo para operacion (paciente %d)", id, paciente));

      // Crear mensaje apropiado y ponerlo en la cola RabbitMQ
      // A RELLENAR
      mensaxe = "SE " + id;     
      channel.basicPublish("", NOMBRE_COLA, null, mensaxe.getBytes("UTF-8"));
      //System.out.println(mensaxe);

      // Esperar por el quirofano concedido
      equipo = medico.getEquipo();

      // Simular que se tarda un tiempo aleatorio en usar el quirofano y el equipo
      t = randRange(1, 3);
      System.out.println(String.format("Medico %d usa el quirofano %d y el equipo %d durante %g segundos (paciente %d)", 
                                        id, quirofano, equipo, t, paciente));
      Thread.sleep(Math.round(t*1000));
      System.out.println(String.format("Medico %d libera quirofano %d tras operar (paciente %d)",  id, quirofano, paciente));

      // Notificar liberación del quirofano
      // Crear mensaje apropiado y ponerlo en la cola RabbitMQ
      // A RELLENAR
      mensaxe = "LQ " + quirofano;     
      channel.basicPublish("", NOMBRE_COLA, null, mensaxe.getBytes("UTF-8"));
      //System.out.println(mensaxe);
      // No hay que esperar ninguna notificación tras liberar

      System.out.println(String.format("Medico %d libera equipo %d tras operar (paciente %d)",  id, equipo, paciente));

      // Notificar liberación del equipo
      // Crear mensaje apropiado y ponerlo en la cola RabbitMQ
      // A RELLENAR
      mensaxe = "LE " + equipo;     
      channel.basicPublish("", NOMBRE_COLA, null, mensaxe.getBytes("UTF-8"));
     // System.out.println(mensaxe);
      // No hay que esperar ninguna notificación tras liberar
    } // Volver al bucle a simular otra operacion

    // Terminados todos las operaciones, la función retorna
  }
}
